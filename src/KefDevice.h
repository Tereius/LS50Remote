#pragma once
#include "Networking.h"
#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>


class QTimer;
class Networking;

class KefDevice : public QObject {

	Q_OBJECT
	QML_SINGLETON;
	QML_NAMED_ELEMENT(KefDevice)
	Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
	Q_PROPERTY(QString host READ getHost WRITE setHost NOTIFY hostChanged)
	Q_PROPERTY(int volume READ getVolume WRITE setVolume NOTIFY volumeChanged)
	Q_PROPERTY(AudioInput input READ getInput WRITE setInput NOTIFY inputChanged)
	Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)

 public:
	enum AudioInput { Network = 0, Bluetooth, BluetoothNC, Aux, Optical, Usb };
	Q_ENUM(AudioInput)

	explicit KefDevice(QObject *pParent = nullptr);
	~KefDevice() override;
	Q_INVOKABLE void powerOff() const;
	bool isConnected() const;
	int getVolume() const;
	void setVolume(int volume);
	AudioInput getInput() const;
	void setInput(AudioInput input);
	bool isMuted() const;
	void setMuted(bool muted);
	QString getHost() const;
	void setHost(const QString &rHost);

 signals:
	void connectedChanged(bool connected);
	void volumeChanged(int volume);
	void inputChanged(AudioInput input);
	void mutedChanged(bool muted);
	void hostChanged(const QString &rHost);

 private slots:
	void receivedPollMessage(QByteArray msg);
	void pollForChanged() const;

 private:
	int mVolume;
	AudioInput mInput;
	bool mMuted;
	QString mHost;
	QTimer *mpPollTimer;
	Networking *mpNetworking;
};
