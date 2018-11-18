#include "KefDevice.h"
#include "Networking.h"
#include "info.h"
#include "qt_windows.h"
#include <QAbstractSocket>
#include <QApplication>
#include <QCommandLineParser>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGlobalStatic>
#include <QIcon>
#include <QMenu>
#include <QMutex>
#include <QMutexLocker>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QSystemTrayIcon>
#include <QTextStream>


namespace {
static QFile log_file;
static QMutex mutex;
static QString version(QString("%1.%2.%3").arg(INFO_VERSION_MAJOR).arg(INFO_VERSION_MINOR).arg(INFO_VERSION_PATCH));
} // namespace

static void fatal_dbug_msg_handler(QtMsgType type, const QMessageLogContext &rContext, const QString &rMessage) {

	QMutexLocker mutex_locker(&mutex);

	if(type == QtFatalMsg) {
		QCoreApplication::instance()->exit(1);
	}
}

static void dbug_msg_handler(QtMsgType type, const QMessageLogContext &rContext, const QString &rMessage) {

	QMutexLocker mutex_locker(&mutex);

	auto text = QString("[%1]").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss"));

	switch(type) {
		case QtDebugMsg:
#ifdef NDEBUG
			text += QString(" DEBUG    : %1").arg(rMessage);
#else
			text += QString(" DEBUG    (%1: %2): %3").arg(rContext.file).arg(rContext.line).arg(rMessage);
#endif // NDEBUG
			break;

		case QtInfoMsg:
#ifdef NDEBUG
			text += QString(" INFO    : %1").arg(rMessage);
#else
			text += QString(" INFO     (%1: %2): %3").arg(rContext.file).arg(rContext.line).arg(rMessage);
#endif // NDEBUG
			break;

		case QtWarningMsg:
#ifdef NDEBUG
			text += QString(" WARNING  : %1").arg(rMessage);
#else
			text += QString(" WARNING  (%1: %2): %3").arg(rContext.file).arg(rContext.line).arg(rMessage);
#endif
			break;

		case QtCriticalMsg:
#ifdef NDEBUG
			text += QString(" CRITICAL : %1").arg(rMessage);
#else
			text += QString(" CRITICAL (%1: %2): %3").arg(rContext.file).arg(rContext.line).arg(rMessage);
#endif
			break;

		case QtFatalMsg:
#ifdef NDEBUG
			text += QString(" FATAL    : %1").arg(rMessage);
#else
			text += QString(" FATAL    (%1: %2): %3").arg(rContext.file).arg(rContext.line).arg(rMessage);
#endif
			text += "\nApplication will be terminated due to Fatal-Error.";
			break;
	}

	QTextStream tStream(&log_file);
	tStream << text << "\n";

#ifdef _WIN32
	OutputDebugString(reinterpret_cast<LPCSTR>(qPrintable(text.append("\n"))));
#elif defined Q_OS_LINUX || defined Q_OS_MAC
	auto txt = text.append("\n").toStdString();
	fputs(txt.c_str(), stderr);
	fflush(stderr);
#endif // OS_WIN32

	if(type == QtFatalMsg) {
		QCoreApplication::instance()->exit(1);
	}
}

void writeSettings() {

	QSettings settings;
	settings.setValue("version", version);
	settings.sync();
}

int main(int argc, char *argv[]) {

	QSettings::setDefaultFormat(QSettings::IniFormat);
	QCoreApplication::setApplicationName(INFO_PROJECTNAME);
	QCoreApplication::setApplicationVersion(version);
	QCoreApplication::setOrganizationName("");
	QCoreApplication::setOrganizationDomain(INFO_DOMAIN);
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

	QApplication app(argc, argv);

#if !defined(NDEBUG) || defined(PORTABLE_MODE)
	QDir storagePath(QCoreApplication::applicationDirPath() + "/" + QCoreApplication::applicationName());
#else
	QDir storagePath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
#endif
	QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, storagePath.absolutePath());
	QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, storagePath.absolutePath());
	writeSettings();
	qInfo() << "Using settings path:" << storagePath.absolutePath();

	QCommandLineParser parser;
	parser.addOption({"u", "Uninstall persistent data."});
	parser.parse(app.arguments());

	if(parser.isSet("u")) {

		// Delete the persistence
		exit(storagePath.removeRecursively() ? 0 : 1);
	}

	QQuickStyle::setStyle("default");
	QIcon::setThemeName("default");

	log_file.setFileName(storagePath.absoluteFilePath(QCoreApplication::applicationName() + ".log"));
	auto success = log_file.open(QIODevice::WriteOnly | QIODevice::Text);
	if(success) {
		qInstallMessageHandler(dbug_msg_handler);
	} else {
		qWarning() << "Couldn't open log file: " << log_file.errorString() << log_file.fileName();
		qInstallMessageHandler(fatal_dbug_msg_handler);
	}

	qmlRegisterType<KefDevice>("com.kef", 1, 0, "KefDevice");
	qRegisterMetaType<QAbstractSocket::SocketError>();

	QQmlApplicationEngine engine;
	engine.load(QUrl(QLatin1String("qrc:///gui/main.qml")));
	if(engine.rootObjects().isEmpty()) return -1;
	auto window = qobject_cast<QQuickWindow *>(engine.rootObjects().first());
	window->setIcon(QIcon(":/gui/ic_settings_remote_white.png"));

	if(QSystemTrayIcon::isSystemTrayAvailable()) {
		app.setQuitOnLastWindowClosed(false);
		auto tray = new QSystemTrayIcon(qApp);
		tray->setIcon(QIcon(":/gui/ic_settings_remote_white.png"));
		tray->setVisible(true);
		auto menu = new QMenu();
		menu->addAction(QObject::tr("show"), window, &QQuickWindow::show);
		menu->addAction(QObject::tr("exit"), &app, &QGuiApplication::quit);
		tray->setContextMenu(menu);
		QObject::connect(tray, &QSystemTrayIcon::activated, qApp, [window](QSystemTrayIcon::ActivationReason reason) {
			if(reason == QSystemTrayIcon::Trigger) window->setVisible(true);
		});
	} else {
		app.setQuitOnLastWindowClosed(true);
		window->setVisible(true);
	}

	return app.exec();
}
