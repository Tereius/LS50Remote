import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import com.kef

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

    StackView {
        id: stack
        initialItem: Qt.resolvedUrl("settings.qml")
        anchors.fill: parent
        anchors.margins: 10
        Component.onCompleted: {
            stack.push(Qt.resolvedUrl("control.qml"));
        }
    }
}
