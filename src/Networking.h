#pragma once
#include "info.h"
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QTimer;
class QTcpSocket;

#define nw Networking::getGlobal()

#define DEFAULT_USER_AGENT \
	QString("%1 %2.%3.%4").arg(INFO_PROJECTNAME).arg(INFO_VERSION_MAJOR).arg(INFO_VERSION_MINOR).arg(INFO_VERSION_PATCH).toLatin1()

class Networking : public QObject {

	Q_OBJECT
	Q_PROPERTY(bool connected READ isConnected NOTIFY connectionStateChanged)

public:
	Networking(QObject *pParent = nullptr);
	virtual ~Networking();
	static Networking *getGlobal();
	static QNetworkAccessManager *getManager() { return mpNetworkAccess; }

	void connectToHost(const QString &rHostName, qint16 port);
	void disconnectFromHost();
	bool isConnected() const { return mConnected; }

	void sendTcp(const QByteArray data);

signals:
	void connectionStateChanged(bool connected);
	void reveicedTcp(QByteArray data);

private:
	static QNetworkAccessManager *mpNetworkAccess;
	QTimer *mpPollTimer;
	QTcpSocket *mpSocket;
	int mReconnectMs;
	bool mConnected;
};
