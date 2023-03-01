import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

Item {

    GridLayout {

        columns: 3

        anchors.fill: parent

        RoundButton {

            Layout.alignment: Qt.AlignLeft

            flat: true

            icon.name: "chevron_left"
            onClicked: {
                stack.pop()
            }
        }

        RowLayout {

            Layout.alignment: Qt.AlignHCenter
            spacing: 0

            RoundButton {

                flat: true
                icon.name: "report_problem"
                icon.color: "#FFDE23"
                visible: !device.connected
            }

            Label {

                text: qsTr("Couldn't connect to LS50")
                color: "#FF3223"
                visible: !device.connected
            }
        }

        RoundButton {

            Layout.alignment: Qt.AlignRight

            flat: true

            icon.name: "save"
            onClicked: {
                device.host = hostTextField.text
            }
        }

        Label {

            Layout.alignment: Qt.AlignRight

            text: qsTr("Host")
        }

        TextField {

            id: hostTextField
            Layout.fillWidth: true

            placeholderText: qsTr("IP or Host name")

            Component.onCompleted: {
                if (device.host.length > 0)
                    text = device.host
            }
        }
    }
}
