import QtQuick 2.9
import QtQuick.Controls.Styles 1.4
import QtQuick.Window 2.2
import QtQuick.Controls 2.4

Window {
    id: dpoint_win

    property string number
    property string value
    property string cell_x
    property string cell_y

    x: cell_x
    y: cell_y
    width: 250
    height: 100
    flags: Qt.SubWindow
    title: qsTr("point update")

    visible: true

    Item {
        id: apoint_item
        anchors.fill: parent

        Rectangle  {
            id: apoint_frame
            anchors.fill: apoint_item
            border.color: "black"
            color: "lightgray"

            Text {
                id: text_value
                width: 80
                height: 15
                anchors { right: parent.right; verticalCenter : parent.verticalCenter; rightMargin: 150;}
                text: qsTr("new value")
                font.pixelSize: 15
            }

            TextField {
                id: new_value
                width: 100
                height: 30
                font.pointSize: 8; font.bold: false;
                text: qsTr(value)
                horizontalAlignment: text_value.AlignRight
                anchors { right: parent.right; verticalCenter : parent.verticalCenter; rightMargin: 20;}

                Keys.onReleased: {
                    if (event.key === Qt.Key_Return) {
                       update_model.updateDigitalPoint_signal(number, new_value.text);
                       dpoint_win.close()
                    }
                }
            }
        }
    }
}


