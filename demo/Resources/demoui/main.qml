import QtQuick 2.0
import Engine 1.0

RenderTarget {
    width: 1280
    height: 720

    Item {
        objectName: "stats"
        width: 200
        height: 58
        anchors.right: parent.right
        anchors.top: parent.top

        Text {
            id: textFps
            text: "FPS: 0"
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.top: parent.top
            anchors.topMargin: 8
            transformOrigin: Item.Center
            font.family: "Helvetica"
            font.pointSize: 12
            color: "red"
        }

        Text {
            id: textFrameTime
            text: "Frame time: 0 ms"
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.top: parent.top
            anchors.topMargin: 32
            transformOrigin: Item.Center
            font.family: "Helvetica"
            font.pointSize: 12
            color: "red"
        }

        function updateFrametime(frametime) {
            textFrameTime.text = "Frame time: %1 ms".arg(frametime)
            textFps.text = "FPS: %1".arg((1000 / frametime).toFixed(1))
        }
    }
}
