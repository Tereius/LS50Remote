import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import com.kef

Item {

    GridLayout {

        anchors.fill: parent
        columns: 3

        RoundButton {

            Layout.alignment: Qt.AlignLeft

            enabled: KefDevice.connected

            icon.name: "power"
            onClicked: {
                KefDevice.powerOff();
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
                checked: KefDevice.input === KefDevice.Network
                enabled: KefDevice.connected
                icon.name: "wifi"
                ButtonGroup.group: btnGroup
                onClicked: {
                    if (checked)
                        KefDevice.input = KefDevice.Network;
                }
            }

            RoundButton {
                checkable: true
                checked: KefDevice.input === KefDevice.Bluetooth || KefDevice.input === KefDevice.BluetoothNC
                enabled: KefDevice.connected
                icon.name: "bluetooth"
                icon.color: {
                    if (checked) {
                        if (KefDevice.input === KefDevice.BluetoothNC)
                            return "red";
                    }
                    return palette.buttonText;
                }
                ButtonGroup.group: btnGroup
                onClicked: {
                    if (checked)
                        KefDevice.input = KefDevice.Bluetooth;
                }
            }

            RoundButton {
                checkable: true
                checked: KefDevice.input === KefDevice.Aux
                enabled: KefDevice.connected
                text: "AUX"
                font.pointSize: 10
                ButtonGroup.group: btnGroup
                onClicked: {
                    if (checked)
                        KefDevice.input = KefDevice.Aux;
                }
            }

            RoundButton {
                checkable: true
                checked: KefDevice.input === KefDevice.Optical
                enabled: KefDevice.connected
                text: "OPT"
                font.pointSize: 10
                ButtonGroup.group: btnGroup
                onClicked: {
                    if (checked)
                        KefDevice.input = KefDevice.Optical;
                }
            }

            RoundButton {
                checkable: true
                checked: KefDevice.input === KefDevice.Usb
                enabled: KefDevice.connected
                text: "PC"
                font.pointSize: 10
                ButtonGroup.group: btnGroup
                onClicked: {
                    if (checked)
                        KefDevice.input = KefDevice.Usb;
                }
            }
        }

        RoundButton {

            Layout.alignment: Qt.AlignRight

            icon.name: "cog-outline"
            flat: true
            onClicked: {
                stack.push(Qt.resolvedUrl("settings.qml"));
            }
        }

        Text {
            text: volumeSlider.value
            Layout.alignment: Qt.AlignHCenter
            enabled: KefDevice.connected
        }

        Slider {
            id: volumeSlider

            Layout.fillWidth: true
            enabled: KefDevice.connected

            from: 0
            value: KefDevice.connected ? KefDevice.volume : 0
            to: 100
            stepSize: 1

            onMoved: {
                KefDevice.volume = value;
            }

            MouseArea {

                anchors.fill: parent

                propagateComposedEvents: true

                onPressed: mouse => {
                    mouse.accepted = false;
                }

                onWheel: wheel => {
                    var steps = wheel.angleDelta.y / 15;
                    if (steps > 0)
                        KefDevice.volume += 1;
                    else
                        KefDevice.volume -= 1;
                }
            }
        }

        RoundButton {
            flat: true

            Layout.alignment: Qt.AlignRight
            enabled: KefDevice.connected

            checkable: true
            checked: KefDevice.muted
            icon.name: KefDevice.muted ? "volume-off" : "volume-high"

            onClicked: {
                if (checked) {
                    KefDevice.muted = true;
                } else {
                    KefDevice.muted = false;
                }
            }
        }
    }
}
