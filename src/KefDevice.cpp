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
	auto hex = QString("532581%1159").arg(volume, 2, 16, QLatin1Char('0')).toLocal8Bit(); // %11 is the placeholder
	nw->sendTcp(QByteArray::fromHex(hex));
	emit mutedChanged(mMuted);
	emit volumeChanged(mVolume);
}

KefDevice::AudioInput KefDevice::getInput() const {

	return mInput;
}

void KefDevice::setInput(AudioInput input) {

	mInput = input;
	QString command;
	switch(input) {
		case KefDevice::Network:
			command = "5330811282";
			break;
		case Usb:
			break;
		case KefDevice::Bluetooth:
			command = "53308119ad";
			break;
		case KefDevice::Aux:
			command = "5330811a9b";
			break;
		case KefDevice::Optical:
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
		auto hex = QString("532581%1159").arg(128, 2, 16, QLatin1Char('0')).toLocal8Bit(); // %11 is the placeholder
		nw->sendTcp(QByteArray::fromHex(hex));
	} else {
		auto hex = QString("532581%1159").arg(mVolume, 2, 16, QLatin1Char('0')).toLocal8Bit(); // %11 is the placeholder
		nw->sendTcp(QByteArray::fromHex(hex));
	}
	mMuted = muted;
	emit mutedChanged(mMuted);
}

void KefDevice::powerOff() {

	nw->sendTcp(QByteArray::fromHex("5330819b0b"));
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
