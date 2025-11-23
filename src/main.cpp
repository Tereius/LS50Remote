#include "AdvancedQmlApplicationEngine.h"
#include "KefDevice.h"
#include "QtApplicationBase.h"
#include "info.h"
#include <QAbstractSocket>
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QImageReader>
#include <QLoggingCategory>
#include <QMenu>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QSettings>
#include <QStandardPaths>
#include <QString>
#include <QSystemTrayIcon>
#include <QTranslator>
#include <QtGlobal>


void writeSettings() {

	QSettings settings;
	settings.setValue("version", INFO_VERSIONSTRING);
	if(!settings.contains("logging")) {
#ifndef NDEBUG
		settings.setValue("logging", QString("default.debug = true"));
#else
		settings.setValue("logging", QString("default.debug = false"));
#endif
	}
	QLoggingCategory::setFilterRules(settings.value("logging").toString());
	settings.sync();
}

int main(int argc, char **argv) {

	qunsetenv("QT_STYLE_OVERRIDE");
	qunsetenv("QT_QUICK_CONTROLS_STYLE");

	QtApplicationBase<QApplication> app(argc, argv);

	auto fontsDir = QDir(":/qt/qml/com/kef/fonts");
	for(const auto font : fontsDir.entryList()) {
		auto id = QFontDatabase::addApplicationFont(":/qt/qml/com/kef/fonts/" + font);
		qInfo() << "Added font" << QFontDatabase::applicationFontFamilies(id);
	}

	qApp->setFont(QFont("Roboto"));
	qInfo() << qApp->font();
	QQuickStyle::setStyle("Basic");
	QIcon::setThemeName("material");

	qInfo() << QLocale().uiLanguages();
	QTranslator translator;
	// look up e.g. :/i18n/myapp_de.qm
	if(translator.load(QLocale(), QLatin1String(INFO_PROJECTNAME), "_", ":/i18n"))
		app.installTranslator(&translator);
	else
		qWarning() << "Couldn't load i18n for locale:" << QLocale().uiLanguages();

	app.installTranslator(&translator);

	qRegisterMetaType<QAbstractSocket::SocketError>();

	AdvancedQmlApplicationEngine qmlEngine;

#ifdef QT_DEBUG
	auto qmlMainFile = QString("com/kef/qml/main.qml");
	if(QFile::exists(qmlMainFile)) {
		qInfo() << "QML hot reloading enabled";
		qmlEngine.setHotReload(true);
		qmlEngine.loadRootItem(qmlMainFile, false);
	} else {
		qmlEngine.setHotReload(false);
		qmlEngine.loadRootItem("qrc:/qt/qml/com/kef/qml/main.qml", false);
	}
#else
	qmlEngine.setHotReload(false);
	qmlEngine.loadRootItem("qrc:/qt/qml/com/kef/qml/main.qml", false);
#endif

	auto window = qobject_cast<QQuickWindow *>(qmlEngine.rootObjects().first());
	if(!window) {
		qFatal("Missing root object");
	}

	if(QSystemTrayIcon::isSystemTrayAvailable()) {
		window->setIcon(QIcon(":/qt/qml/com/kef/images/LS50Remote_mono.svg"));
		app.setQuitOnLastWindowClosed(false);
		auto tray = new QSystemTrayIcon(qApp);
		tray->setIcon(QIcon(":/qt/qml/com/kef/images/LS50Remote_mono.svg"));
		tray->setVisible(true);
		auto menu = new QMenu();
		menu->addAction(QObject::tr("show"), window, &QQuickWindow::show);
		menu->addAction(QObject::tr("exit"), &app, &QGuiApplication::quit);
		QObject::connect(window, &QWindow::visibilityChanged, qApp, [window](QWindow::Visibility visibility) {
			if(visibility == QWindow::Minimized) window->setVisible(false);
		});
		tray->setContextMenu(menu);
		QObject::connect(tray, &QSystemTrayIcon::activated, qApp, [window](QSystemTrayIcon::ActivationReason reason) {
			if(reason == QSystemTrayIcon::Trigger) {
				window->setVisible(true);
				window->setVisibility(QWindow::Windowed);
				window->requestActivate();
			}
		});
	} else {
		app.setQuitOnLastWindowClosed(true);
		window->setVisible(true);
	}

	return app.exec();
}
