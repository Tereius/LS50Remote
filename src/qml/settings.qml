import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import com.kef

Item {

    GridLayout {

        columns: 3

        anchors.fill: parent

        RoundButton {

            Layout.alignment: Qt.AlignLeft

            flat: true

            icon.name: "chevron-left"
            onClicked: {
                stack.pop();
            }
        }

        RowLayout {

            Layout.alignment: Qt.AlignHCenter
            spacing: 0

            RoundButton {

                flat: true
                icon.name: "alert"
                icon.color: "#FFDE23"
                visible: !KefDevice.connected
            }

            Label {

                text: qsTr("Couldn't connect to LS50")
                color: "#FF3223"
                visible: !KefDevice.connected
            }
        }

        RoundButton {

            Layout.alignment: Qt.AlignRight

            flat: true

            icon.name: "content-save"
            onClicked: {
                KefDevice.host = hostTextField.text;
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
                if (KefDevice.host.length > 0)
                    text = KefDevice.host;
            }
        }
    }
}
