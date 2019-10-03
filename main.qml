import QtQuick 2.9
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import com.ics.hb 1.0

Window {
    id: root_win

    visible: true
    width: 1280
    height: 800
    title: qsTr("Point Monitor")

    Item {
        id: tool_bar

        x: parent.x
        y: parent.y
        width: parent.width
        height: 40

        Rectangle  {
            id: tool_bar_frame
            anchors.fill: tool_bar
            border.color: "black"
            color: "lightgray"
            state: "settings"

                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            anchors.rightMargin: 2;
                            anchors.bottomMargin: 2;

            ToolBar {
                id: toolBar
                x: 2
                y: 2
                width: tool_bar_frame.width - 1
                height: tool_bar.height - 6

                ToolButton {
                    id: tb_settings
                    anchors.left: parent.left
                    anchors.leftMargin: 15;
                    anchors.top: parent.top
                    anchors.topMargin: 4;
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 4;

                    text: qsTr("Settings")
                    onClicked: tool_bar_frame.state = "settings"
                }

                ToolSeparator {
                    id: toolSeparator1
                    anchors.left: tb_settings.right
                    anchors.leftMargin: 10;
                    anchors.top: parent.top
                    anchors.topMargin: 2;
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 2;
                    width: 14
                }

                ToolButton {
                    id: tb_discret_points
                    anchors.left: toolSeparator1.right
                    anchors.leftMargin: 10;
                    anchors.top: parent.top
                    anchors.topMargin: 4;
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 4;

                    text: qsTr("Discrete points")
                    onClicked: tool_bar_frame.state = "discret_point"
                }

                ToolSeparator {
                    id: toolSeparator2
                    anchors.left: tb_discret_points.right
                    anchors.leftMargin: 10;
                    anchors.top: parent.top
                    anchors.topMargin: 2;
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 2;
                    width: 14
                }

                ToolButton {
                    id: tb_analog_points
                    anchors.left: toolSeparator2.right
                    anchors.leftMargin: 10;
                    anchors.top: parent.top
                    anchors.topMargin: 4;
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 4;

                    text: qsTr("Analog points")
                    onClicked: tool_bar_frame.state = "analog_point"
                }

                ToolSeparator {
                    id: toolSeparator3
                    anchors.left: tb_analog_points.right
                    anchors.leftMargin: 10;
                    anchors.top: parent.top
                    anchors.topMargin: 3;
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 3;
                    width: 14
                }
            }

            states: [
                State {
                    name: "settings"
                    PropertyChanges { target: connection_tab; visible: true}
                    PropertyChanges { target: dpoint_tab; visible: false}
                    PropertyChanges { target: apoint_tab; visible: false}
                },
                State {
                    name: "discret_point"
                    PropertyChanges { target: connection_tab; visible: false}
                    PropertyChanges { target: dpoint_tab; visible: true}
                    PropertyChanges { target: apoint_tab; visible: false}
                },
                State {
                    name: "analog_point"
                    PropertyChanges { target: connection_tab; visible: false}
                    PropertyChanges { target: dpoint_tab; visible: false}
                    PropertyChanges { target: apoint_tab; visible: true}
                }
            ]
        }
    }

    Item {
        id: status_bar

        width: parent.width
        height: 30
        x: 0
        y:  parent.height - height

        Rectangle  {
            id: status_frame
            gradient: Gradient {
                GradientStop {
                    position: 0
                    color: "#d3d3d3"
                }

                GradientStop {
                    position: 1
                    color: "#000000"
                }
            }
            anchors.fill: parent
            state: "disconnected"

            border.color: "black"

            Text {
                id: status_text
                text: "Not Connected"
                color:"red";
                anchors { right: parent.right; verticalCenter : parent.verticalCenter; rightMargin: 10 }
                font.pointSize: 12; font.bold: false;
            }

            states: [
                State {
                    name: "connected"
                    PropertyChanges { target: status_text; text:"Connected"; color:"white"}
                },
                State {
                    name: "disconnected"
                    PropertyChanges { target: status_text; text:"Not Connected"; color:"red"}
                }
            ]

            ConnectionModels{
               id: data_connection
            }

            Connections {
                target: data_connection
                onConnectionChanged: {
                    if (connection_state) {
                        status_frame.state = "connected"
                        console.debug("data connection OK")
                    } else {
                        status_frame.state = "disconnected"
                        console.debug("data connection ERROR")
                    }
                }
            }
        }
    }

    Item {
        id: work_field

        width: parent.width
        height:parent.height - tool_bar.height - status_bar.height - 2
        x: 0
        y:  parent.y + tool_bar.height + 1

        Rectangle  {
            id: work_frame
            color: "lightgray"
            anchors.fill: work_field
            border.color: "black"
        }
    }

    Item {
        id: connection_tab
        visible: true
        anchors.fill: work_field

        Rectangle  {
            id: settings_frame
            color: "lightgray"
            anchors.fill: connection_tab
            border.color: "black"
            state: "connect"

            ConnectionButton{
               id: conn_button
            }

            Connections {
                target: conn_button
                onConnectionChanged: {
                    if (connection_state) {
                        settings_frame.state = "disconnect"
                    } else {
                        settings_frame.state = "connect"
                    }
                }
            }

            states: [
                State {
                    name: "connect"
                    PropertyChanges { target: conn_ctrl; text: "Connect"}
                },
                State {
                    name: "disconnect"
                    PropertyChanges { target: conn_ctrl; text: "Disconnect"}
                }
            ]

            Button {
                id: conn_ctrl
                x: 519
                y: 343
                anchors { right: parent.right; bottom : parent.bottom; rightMargin: 20; bottomMargin: 20;}
                onClicked: {
                    if (settings_frame.state == "connect") {
                        conn_button.setConnectionRequest(true, server_ip.text, server_port.text)
                    } else {
                        conn_button.setConnectionRequest(false, server_ip.text, server_port.text)
                    }
                }

                contentItem: Text {
                    text: conn_ctrl.text
                    font: conn_ctrl.font
                    opacity: enabled ? 1.0 : 0.3
                    color: conn_ctrl.down ? "green" : "black"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
                background: Rectangle {
                    implicitWidth: 100
                    implicitHeight: 40
                    opacity: enabled ? 1 : 0.3
                    border.color: conn_ctrl.down ? "#17a81a" : "black"
                    border.width: 1
                    radius: 2
                }
            }

            Text {
                id: text1
                width: 80
                height: 15
                anchors { right: parent.right; bottom : parent.bottom; rightMargin: 250; bottomMargin: 210; }
                text: qsTr("Server Port")
                font.pixelSize: 15
            }

            Text {
                id: text2
                width: 80
                height: 15
                anchors { right: parent.right; bottom : parent.bottom; rightMargin: 250; bottomMargin: 150; }
                text: qsTr("Server IP")
                font.pixelSize: 15
            }

            TextField {
                id: server_port
                text: qsTr("8096")
                anchors.right: parent.right
                anchors.rightMargin: 20
                horizontalAlignment: Text.AlignRight
                anchors { bottom : parent.bottom; bottomMargin: 200;}
            }

            TextField {
                id: server_ip
                text: qsTr("192.168.88.10")
                horizontalAlignment: Text.AlignRight
                anchors { right: parent.right; bottom : parent.bottom; rightMargin: 20; bottomMargin: 140; }
            }
        }
    }

    Item {
        id: dpoint_tab
        visible: true
        anchors.fill: work_field

        Rectangle  {
            id: dpoint_frame
            color: "lightgray"
            anchors.fill: dpoint_tab
            border.color: "black"

            GridView {
                id: dpoint_view

                anchors.margins: 10
                anchors.fill: parent
                cellHeight: 40
                cellWidth: 80
                model: discret_model
                clip: true

                highlight: Rectangle {
                    color: "green"
                }

                delegate: Item {
                    property var view: GridView.dpoint_view
                    property var isCurrent: GridView.isCurrentItem

                    height: dpoint_view.cellHeight
                    width: dpoint_view.cellWidth

                    Rectangle {
                        id: d_item
                        anchors.margins: 2
                        anchors.fill: parent
                        border {
                            color: "black"
                            width: 1
                        }

                        Rectangle  {
                            color: "white"
                            anchors.left: d_item.left
                            anchors.top: d_item.top
                            width: 25
                            height: 15
                            border.color: "black"

                            Text {
                                anchors.centerIn: parent
                                renderType: Text.NativeRendering
                                font.pointSize: 8; font.bold: false;
                                text: Name
                            }
                        }

                        Rectangle  {
                           function getColor(status)
                            {
                                if ("" === status) return "lightgrey";
                                if ("0" === status) return "lightgrey";
                                if ("1" === status) return "lightgreen";
                                if ("2" === status) return "red";
                                if ("4" === status) return "red";
                                return "lightgreen";
                            }

                            anchors.right: d_item.right
                            width: 25
                            height: 15
                            color: getColor(Status)
                            border.color: "black"

                            Text {
                                anchors.centerIn: parent
                                renderType: Text.NativeRendering
                                font.pointSize: 8; font.bold: false;
                                text: Status
                            }
                        }

                        Text {
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            anchors.rightMargin: 2;
                            anchors.bottomMargin: 2;
                            renderType: Text.NativeRendering
                            text: Value
                        }

                        MouseArea {
                            id: d_item_mouse
                            anchors.fill: parent
                            hoverEnabled: true

                            onClicked: {
                                dpoint_view.currentIndex = model.index
                            }
                            onDoubleClicked: {
                                if(mouse.button === Qt.LeftButton) {
                                    var component = Qt.createComponent("update_dpoint.qml")
                                    var window    = component.createObject(dpoint_view, {'number':Name,
                                                                                         'value':Value,
                                                                                         'cell_x':dpoint_frame.x + dpoint_frame.width / 2,
                                                                                         'cell_y':dpoint_frame.y + dpoint_frame.height / 2 })
                                    window.show()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Item {
        id: apoint_tab
        visible: true
        anchors.fill: work_field

        Rectangle  {
            id: apoint_frame
            color: "lightgray"
            anchors.fill: apoint_tab
            border.color: "black"

            GridView {
                id: apoint_view

                anchors.margins: 10
                anchors.fill: parent
                cellHeight: 40
                cellWidth: 80
                model: analog_model
                clip: true

                highlight: Rectangle {
                    color: "green"
                }

                delegate: Item {
                    property var view: GridView.apoint_view
                    property var isCurrent: GridView.isCurrentItem

                    height: apoint_view.cellHeight
                    width: apoint_view.cellWidth

                    Rectangle {
                        id: a_item
                        anchors.margins: 2
                        anchors.fill: parent
                        border {
                            color: "black"
                            width: 1
                        }

                        Rectangle  {
                            color: "white"
                            anchors.left: a_item.left
                            anchors.top: a_item.top
                            width: 25
                            height: 15
                            border.color: "black"

                            Text {
                                anchors.centerIn: parent
                                renderType: Text.NativeRendering
                                font.pointSize: 8; font.bold: false;
                                text: Name
                            }
                        }

                        Rectangle  {
                           function getColor(status)
                            {
                                if ("" === status) return "lightgrey";
                                if ("0" === status) return "lightgrey";
                                if ("1" === status) return "lightgreen";
                                if ("2" === status) return "red";
                                if ("4" === status) return "red";
                                return "lightgreen";
                            }

                            anchors.right: a_item.right
                            width: 25
                            height: 15
                            color: getColor(Status)
                            border.color: "black"

                            Text {
                                anchors.centerIn: parent
                                renderType: Text.NativeRendering
                                font.pointSize: 8; font.bold: false;
                                text: Status
                            }
                        }

                        Text {
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            anchors.rightMargin: 2;
                            anchors.bottomMargin: 2;
                            renderType: Text.NativeRendering
                            text: Value
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: apoint_view.currentIndex = model.index;
                            onDoubleClicked: {
                                if(mouse.button === Qt.LeftButton) {
                                    var component = Qt.createComponent("update_apoint.qml")
                                    var window    = component.createObject(dpoint_view, {'number':Name,
                                                                                         'value':Value,
                                                                                         'cell_x':apoint_frame.x + apoint_frame.width / 2,
                                                                                         'cell_y':apoint_frame.y + apoint_frame.height / 2 })
                                    window.show()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
