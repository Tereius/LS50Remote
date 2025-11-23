#pragma once
#include <QObject>

class QNetworkAccessManager;
class QTimer;
class QTcpSocket;

class Networking : public QObject {

	Q_OBJECT
	Q_PROPERTY(bool connected READ isConnected NOTIFY connectionStateChanged)

 public:
	explicit Networking(QObject *pParent = nullptr);
	~Networking() override;

	void connectToHost(const QString &rHostName, qint16 port);
	void disconnectFromHost();
	bool isConnected() const { return mConnected; }

	void sendTcp(const QByteArray &data);

 signals:
	void connectionStateChanged(bool connected);
	void reveicedTcp(QByteArray data);

 private:
	QTimer *mpPollTimer;
	QTcpSocket *mpSocket;
	int mReconnectMs;
	bool mConnected;
};
