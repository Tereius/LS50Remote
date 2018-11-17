import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Universal 2.3
import com.kef 1.0

ApplicationWindow {

	id: window
	width: column.childrenRect.width + column.leftPadding + column.rightPadding
	height: column.childrenRect.height + column.topPadding + column.bottomPadding
	minimumWidth: column.childrenRect.width + column.leftPadding + column.rightPadding
	minimumHeight: column.childrenRect.height + column.topPadding + column.bottomPadding
	visible: false
	flags: Qt.Window
	title: "LS50 Remote Control"

	screen: Qt.application.screens[0]
	x: screen.virtualX + screen.width - width
	y: screen.virtualY + screen.height - height

	KefDevice {
		id: device
	}

	Column {

		id: column

		spacing: 10
		padding: 20

		ButtonGroup {
			id: btnGroup
			exclusive: true
		}

		Row {

			spacing: 5

			RoundButton {
				checkable: true
				checked: true
				icon.name: "ic_wifi"
				ButtonGroup.group: btnGroup
				onCheckedChanged: {
					if (checked)
						device.input = KefDevice.Network
				}
			}

			RoundButton {
				checkable: true
				icon.name: "ic_bluetooth"
				ButtonGroup.group: btnGroup
				onCheckedChanged: {
					if (checked)
						device.input = KefDevice.Bluetooth
				}
			}

			RoundButton {
				checkable: true
				text: "AUX"
				ButtonGroup.group: btnGroup
				onCheckedChanged: {
					if (checked)
						device.input = KefDevice.Aux
				}
			}

			RoundButton {
				checkable: true
				text: "OPT"
				ButtonGroup.group: btnGroup
				onCheckedChanged: {
					if (checked)
						device.input = KefDevice.Optical
				}
			}

			RoundButton {
				checkable: true
				text: "PC"
				ButtonGroup.group: btnGroup
				onCheckedChanged: {
					if (checked)
						device.input = KefDevice.Usb
				}
			}
		}

		Row {
			Slider {

				from: 0
				value: 0
				to: 100

				onMoved: {

					device.volume = value
				}
			}

			RoundButton {
				flat: true
				checkable: true
				checked: device.muted
				icon.name: "ic_volume_up"

				onClicked: {
					if (checked) {
						icon.name = "ic_volume_off"
						device.muted = true
					} else {
						icon.name = "ic_volume_up"
						device.muted = false
					}
				}
			}
		}
	}
}
