#include "Networking.h"
#include <QCoreApplication>
#include <QGlobalStatic>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTcpSocket>
#include <QTimer>


Q_GLOBAL_STATIC(Networking, globalNetworking)


Networking::Networking(QObject *pParent /*= nullptr*/) :
 QObject(pParent),
 mpPollTimer(new QTimer()),
 mpPendingSseReply(nullptr),
 mpSocket(new QTcpSocket()),
 mReconnectMs(3000),
 mConnected(false) {

	mpPollTimer->setTimerType(Qt::VeryCoarseTimer);
}

Networking::~Networking() {

	mpPollTimer->deleteLater();
	disconnectFromHost();
}

Networking *Networking::getGlobal() {

	return globalNetworking;
}

void Networking::connectToHost(const QString &rHostName, qint16 port) {

	disconnectFromHost();
	connect(mpSocket, &QTcpSocket::connected, this,
	        [this, rHostName]() {
		        qInfo() << "Connected to host" << rHostName;
		        mConnected = true;
		        emit connectionStateChanged(mConnected);
		        mpPollTimer->stop();
		        mpPollTimer->setInterval(mReconnectMs);
	        },
	        Qt::QueuedConnection);

	connect(mpSocket, &QTcpSocket::disconnected, this,
	        [this, rHostName]() {
		        qInfo() << "Disconnected from host" << rHostName;
		        mConnected = false;
		        emit connectionStateChanged(mConnected);
		        mpPollTimer->start(mReconnectMs);
	        },
	        Qt::QueuedConnection);

	connect(mpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this,
	        [this](QAbstractSocket::SocketError socketError) { qInfo() << "TCP socket error:" << mpSocket->errorString(); },
	        Qt::QueuedConnection);

	connect(mpPollTimer, &QTimer::timeout, this,
	        [this, rHostName, port]() {
		        qInfo() << "Trying to reconnect to host" << rHostName;
		        mpSocket->connectToHost(rHostName, port);
	        },
	        Qt::QueuedConnection);

	mpPollTimer->setInterval(60000);
	mpSocket->connectToHost(rHostName, port);
}

void Networking::disconnectFromHost() {

	mConnected = false;
	emit connectionStateChanged(mConnected);
	mpPollTimer->stop();
	mpPollTimer->disconnect(this);
	mpSocket->disconnect(this);
	mpSocket->close();
	mpSocket->disconnectFromHost();
}

void Networking::sendTcp(const QByteArray data) {

	mpSocket->write(data);
}

QNetworkAccessManager *Networking::mpNetworkAccess = new QNetworkAccessManager(qApp);
