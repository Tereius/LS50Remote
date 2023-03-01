import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import com.kef 1.0

Item {

    GridLayout {

        anchors.fill: parent
        columns: 3

        RoundButton {

            Layout.alignment: Qt.AlignLeft

            enabled: device.connected

            icon.name: "power_settings_new"
            onClicked: {
                device.powerOff()
            }
        }

        Row {

            spacing: 5

            Layout.alignment: Qt.AlignHCenter

            ButtonGroup {
                id: btnGroup
                exclusive: true
            }

            RoundButton {
                checkable: true
                checked: device.input === KefDevice.Network
                enabled: device.connected
                icon.name: "wifi"
                ButtonGroup.group: btnGroup
                onClicked: {
                    if (checked)
                        device.input = KefDevice.Network
                }
            }

            RoundButton {
                checkable: true
                checked: device.input === KefDevice.Bluetooth
                         || device.input === KefDevice.BluetoothNC
                enabled: device.connected
                icon.name: "bluetooth"
                icon.color: {
                    if (checked) {
                        if (device.input === KefDevice.BluetoothNC)
                            return "red"
                    }
                    return ""
                }
                ButtonGroup.group: btnGroup
                onClicked: {
                    if (checked)
                        device.input = KefDevice.Bluetooth
                }
            }

            RoundButton {
                checkable: true
                checked: device.input === KefDevice.Aux
                enabled: device.connected
                text: "AUX"
                ButtonGroup.group: btnGroup
                onClicked: {
                    if (checked)
                        device.input = KefDevice.Aux
                }
            }

            RoundButton {
                checkable: true
                checked: device.input === KefDevice.Optical
                enabled: device.connected
                text: "OPT"
                ButtonGroup.group: btnGroup
                onClicked: {
                    if (checked)
                        device.input = KefDevice.Optical
                }
            }

            RoundButton {
                checkable: true
                checked: device.input === KefDevice.Usb
                enabled: device.connected
                text: "PC"
                ButtonGroup.group: btnGroup
                onClicked: {
                    if (checked)
                        device.input = KefDevice.Usb
                }
            }
        }

        RoundButton {

            Layout.alignment: Qt.AlignRight

            icon.name: "settings"
            flat: true
            onClicked: {
                stack.push(Qt.resolvedUrl("settings.qml"))
            }
        }

        Text {
            text: volumeSlider.value
            Layout.alignment: Qt.AlignHCenter
            enabled: device.connected
        }

        Slider {

            id: volumeSlider

            Layout.fillWidth: true
            enabled: device.connected

            from: 0
            value: device.connected ? device.volume : 0
            to: 100
            stepSize: 1

            onMoved: {

                device.volume = value
            }

            MouseArea {

                anchors.fill: parent

                propagateComposedEvents: true

                onPressed: {
                    mouse.accepted = false
                }

                onWheel: {
                    var steps = wheel.angleDelta.y / 15
                    if (steps > 0)
                        device.volume += 1
                    else
                        device.volume -= 1
                }
            }
        }

        RoundButton {
            flat: true

            Layout.alignment: Qt.AlignRight
            enabled: device.connected

            checkable: true
            checked: device.muted
            icon.name: device.muted ? "volume_off" : "volume_up"

            onClicked: {
                if (checked) {
                    device.muted = true
                } else {
                    device.muted = false
                }
            }
        }
    }
}
