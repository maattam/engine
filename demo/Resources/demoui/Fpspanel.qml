import QtQuick 2.0

Item {
    width: 150
    height: 50

    Column {
        spacing: 5

        Text { font.family: "Consolas"; font.pointSize: 11; color: "white"
               id: textFps
               text: "FPS: 0" }

        Text { font.family: "Consolas"; font.pointSize: 11; color: "white"
               id: textFrameTime
               text: "Frame time: 0 ms" }
    }

    function updateFrametime(frametime) {
        textFrameTime.text = "Frame time: %1 ms".arg(frametime)
        textFps.text = "FPS: %1".arg((1000 / frametime).toFixed(1))
    }
}
