#pragma once
#include <QObject>


class KefDevice : public QObject {

	Q_OBJECT
	Q_PROPERTY(int volume READ getVolume WRITE setVolume NOTIFY volumeChanged)
	Q_PROPERTY(AudioInput input READ getInput WRITE setInput NOTIFY inputChanged)
	Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)

public:
	enum AudioInput { Network = 0, Bluetooth, Aux, Optical, Usb };
	Q_ENUM(AudioInput)

	KefDevice(QObject *pParent = nullptr);
	int getVolume() const;
	void setVolume(int volume);
	AudioInput getInput() const;
	void setInput(AudioInput input);
	bool isMuted() const;
	void setMuted(bool muted);

signals:
	void volumeChanged(int volume);
	void inputChanged(AudioInput input);
	void mutedChanged(bool muted);

private:
	int mVolume;
	AudioInput mInput;
	bool mMuted;
};
