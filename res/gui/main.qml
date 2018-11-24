import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import Qt.labs.settings 1.0
import com.kef 1.0

ApplicationWindow {

	id: window
	width: 365
	height: 120
	minimumWidth: 365
	minimumHeight: 120
	maximumWidth: 265 * 2
	maximumHeight: 120 * 2
	visible: false
	flags: Qt.Window
	title: "LS50 Remote Control"

	screen: Qt.application.screens[0]

	Settings {
		id: settings
		property alias window_x: window.x
		property alias window_y: window.y
		property alias window_width: window.width
		property alias window_height: window.height
	}

	KefDevice {
		id: device
	}

	StackView {
		id: stack
		initialItem: "qrc:/gui/control.qml"
		anchors.fill: parent
		anchors.margins: 10
	}
}
