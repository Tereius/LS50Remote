#include "KefDevice.h"
#include "Networking.h"
#include <QSettings>


KefDevice::KefDevice(QObject *pParent /*= nullptr*/) : QObject(pParent), mVolume(0), mInput(Network), mMuted(false), mHost() {

	connect(nw, &Networking::connectionStateChanged, this, [this](bool connected) { emit connectedChanged(connected); });

	QSettings settings;
	setHost(settings.value("host").toString());
}

bool KefDevice::isConnected() const {

	return nw->isConnected();
}

int KefDevice::getVolume() const {

	return mVolume;
}

void KefDevice::setVolume(int volume) {

	mVolume = volume;
	mMuted = false;
	auto hex = QString("532581%1").arg(volume, 2, 16, QLatin1Char('0')).toLocal8Bit(); // %11 is the placeholder
	nw->sendTcp(QByteArray::fromHex(hex));
	emit mutedChanged(mMuted);
	emit volumeChanged(mVolume);
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
		nw->sendTcp(QByteArray::fromHex(command.toLocal8Bit()));
	}
	emit inputChanged(mInput);
}

bool KefDevice::isMuted() const {
	return mMuted;
}

void KefDevice::setMuted(bool muted) {

	if(muted) {
		auto hex = QString("532581%1").arg(128, 2, 16, QLatin1Char('0')).toLocal8Bit(); // %11 is the placeholder
		nw->sendTcp(QByteArray::fromHex(hex));
	} else {
		auto hex = QString("532581%1").arg(mVolume, 2, 16, QLatin1Char('0')).toLocal8Bit(); // %11 is the placeholder
		nw->sendTcp(QByteArray::fromHex(hex));
	}
	mMuted = muted;
	emit mutedChanged(mMuted);
}

void KefDevice::powerOff() {

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
