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

			icon.name: "ic_power_settings_new"
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
				checked: true
				enabled: device.connected
				icon.name: "ic_wifi"
				ButtonGroup.group: btnGroup
				onCheckedChanged: {
					if (checked)
						device.input = KefDevice.Network
				}
			}

			RoundButton {
				checkable: true
				enabled: device.connected
				icon.name: "ic_settings"
				ButtonGroup.group: btnGroup
				onClicked: {
					stack.push()
				}
			}

			RoundButton {
				checkable: true
				enabled: device.connected
				text: "AUX"
				ButtonGroup.group: btnGroup
				onCheckedChanged: {
					if (checked)
						device.input = KefDevice.Aux
				}
			}

			RoundButton {
				checkable: true
				enabled: device.connected
				text: "OPT"
				ButtonGroup.group: btnGroup
				onCheckedChanged: {
					if (checked)
						device.input = KefDevice.Optical
				}
			}

			RoundButton {
				checkable: true
				enabled: device.connected
				text: "PC"
				ButtonGroup.group: btnGroup
				onCheckedChanged: {
					if (checked)
						device.input = KefDevice.Usb
				}
			}
		}

		RoundButton {

			Layout.alignment: Qt.AlignRight

			icon.name: "ic_settings"
			flat: true
			onClicked: {
				stack.push("qrc:/gui/settings.qml")
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
			value: 0
			to: 100
			stepSize: 1

			onMoved: {

				device.volume = value
			}
		}

		RoundButton {
			flat: true

			Layout.alignment: Qt.AlignRight
			enabled: device.connected

			checkable: true
			checked: device.muted
			icon.name: device.muted ? "ic_volume_off" : "ic_volume_up"

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
