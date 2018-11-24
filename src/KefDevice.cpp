#include "KefDevice.h"
#include "Networking.h"
#include <QDebug>
#include <QSettings>
#include <QTimer>


KefDevice::KefDevice(QObject *pParent /*= nullptr*/) :
 QObject(pParent),
 mVolume(0),
 mInput(Network),
 mMuted(false),
 mHost(),
 mpPollTimer(new QTimer()) {

	QSettings settings;
	mpPollTimer->setInterval(3000);
	setHost(settings.value("host").toString());
	connect(nw, &Networking::connectionStateChanged, this, [this](bool connected) {
		if(connected) {
			pollForChanged();
			mpPollTimer->start();
		} else {
			mpPollTimer->stop();
		}
		emit connectedChanged(connected);
	});
	connect(nw, &Networking::reveicedTcp, this, &KefDevice::receivedPollMessage);
	connect(mpPollTimer, &QTimer::timeout, this, &KefDevice::pollForChanged);
}

KefDevice::~KefDevice() {

	nw->disconnect(this);
	mpPollTimer->deleteLater();
}

bool KefDevice::isConnected() const {

	return nw->isConnected();
}

int KefDevice::getVolume() const {

	return mVolume;
}

void KefDevice::setVolume(int volume) {

	if(volume < 0) volume = 0;
	if(volume > 127) volume = 127;

	if(volume != mVolume) {
		mpPollTimer->start();
		mVolume = volume;
		mMuted = false;
		auto hex = QString("532581%1").arg(volume, 2, 16, QLatin1Char('0')).toLocal8Bit();
		qDebug() << "changed volume to" << volume << ":" << hex;
		nw->sendTcp(QByteArray::fromHex(hex));
		emit mutedChanged(mMuted);
		emit volumeChanged(mVolume);
	}
}

KefDevice::AudioInput KefDevice::getInput() const {

	return mInput;
}

// play pause: 53:31:81:81

// input: 47:30:80 -> 52308112ec (Network)
// 47:3d:80 -> 52:3d:81:c6:5f
// 47:27:80 -> 52:27:81:84:b3
// 47:28:80 -> 52:28:81:86:fa
// 47:29:80 -> 52:29:81:86:91
// 47:2a:80 -> 52:2a:81:84:22
// 47:2b:80 -> 52:2b:81:89:6a
// 47:2c:80 -> 52:2c:81:88:7b
// 47:2d:80 -> 52:2d:81:8a:1e
//
// 47:25:80 -> 52:25:81:1e:aa (volume)

void KefDevice::setInput(AudioInput input) {

	if(input != mInput) {
		mpPollTimer->start();
		mInput = input;
		QString command;
		switch(input) {
			case KefDevice::Network:
				command = "53308112";
				break;
			case KefDevice::Usb:
				command = "5330811c";
				break;
			case KefDevice::Bluetooth:
				command = "53308119";
				break;
			case KefDevice::Aux:
				command = "5330811a";
				break;
			case KefDevice::Optical:
				command = "5330811b";
				break;
			default:
				break;
		}
		if(!command.isNull()) {
			qDebug() << "changed input:" << command;
			nw->sendTcp(QByteArray::fromHex(command.toLocal8Bit()));
			QTimer::singleShot(1000, this, &KefDevice::pollForChanged);
		}
		emit inputChanged(mInput);
	}
}

bool KefDevice::isMuted() const {
	return mMuted;
}

void KefDevice::setMuted(bool muted) {

	mpPollTimer->start();
	if(muted) {
		auto hex = QString("532581%1").arg(128, 2, 16, QLatin1Char('0')).toLocal8Bit();
		qDebug() << "enabled mute, current volume" << mVolume << ":" << hex;
		nw->sendTcp(QByteArray::fromHex(hex));
	} else {
		auto hex = QString("532581%1").arg(mVolume, 2, 16, QLatin1Char('0')).toLocal8Bit();
		qDebug() << "disabled mute, setting volume" << mVolume << ":" << hex;
		nw->sendTcp(QByteArray::fromHex(hex));
	}
	mMuted = muted;
	emit mutedChanged(mMuted);
}

void KefDevice::powerOff() {

	qDebug() << "power off:" << hex;
	nw->sendTcp(QByteArray::fromHex("5330819b"));
}

QString KefDevice::getHost() const {

	return mHost;
}

void KefDevice::setHost(const QString &rHost) {

	mHost = rHost;
	QSettings settings;

	settings.setValue("host", mHost);

	nw->connectToHost(mHost, (qint16)50001);
	emit hostChanged(mHost);
}

void KefDevice::receivedPollMessage(QByteArray msg) {

	auto hexMsg = QString::fromLatin1(msg.toHex());
	if(hexMsg.startsWith("523081")) {
		// Input
		auto input = hexMsg.mid(6, 2).toInt(nullptr, 16);
		auto oldInput = getInput();
		switch(input) {
			case 0x12:
				mInput = KefDevice::Network;
				break;
			case 0x1c:
				mInput = KefDevice::Usb;
				break;
			case 0x1f:
				mInput = KefDevice::BluetoothNC;
				break;
			case 0x19:
				mInput = KefDevice::Bluetooth;
				break;
			case 0x1a:
				mInput = KefDevice::Aux;
				break;
			case 0x1b:
				mInput = KefDevice::Optical;
				break;
			default:
				qWarning() << "Couldn't decode input response:" << hexMsg;
				break;
		}
		if(oldInput != mInput) emit inputChanged(mInput);
	} else if(hexMsg.startsWith("522581")) {
		// Volume
		bool ok = false;
		auto volume = hexMsg.mid(6, 2).toInt(&ok, 16);
		if(ok) {
			if(volume < 128) {
				auto oldVolume = mVolume;
				mVolume = volume;
				if(oldVolume != mVolume) emit volumeChanged(mVolume);
				if(mMuted) {
					mMuted = false;
					emit mutedChanged(mMuted);
				}
			} else {
				// mute
				auto oldMuted = mMuted;
				mMuted = true;
				if(oldMuted != mMuted) emit mutedChanged(mMuted);
			}
		} else {
			qWarning() << "Couldn't decode volume response:" << hexMsg;
		}
	} else {
		//	qWarning() << "Couldn't decode response:" << hexMsg;
	}
}

void KefDevice::pollForChanged() {

	if(isConnected()) {
		nw->sendTcp(QByteArray::fromHex("473080"));
		nw->sendTcp(QByteArray::fromHex("472580"));
	}
}
