import QtQuick 2.0
import Engine 1.0

RenderTarget {
//Rectangle {
    width: 1280
    height: 720

    Item {
        id: ui
        focus: true
        anchors.fill: parent

        Column {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 12
            spacing: 5

            Text { font.family: "Consolas"; font.pointSize: 11; color: "white"
                   text: "Toggle UI: Esc" }
        }

        Stats {
            id: stats
            objectName: "stats"
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 12
        }

        Column {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 12

            spacing: 12

            General {
                objectName: "general"
            }

            Tonemap {
                objectName: "tonemap"
            }
        }
    }

    Keys.onPressed: {
        if(event.key === Qt.Key_Escape) {
            ui.visible = !ui.visible;
        }
    }
}
