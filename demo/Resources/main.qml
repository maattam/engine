import QtQuick 2.0
import Engine 1.0

RenderTarget {
    width: 800
    height: 600

    Text {
		text: "QML Integration works! TODO: Some UI :P"
		font.family: "Helvetica"
		font.pointSize: 24
		color: "red"

        verticalAlignment: Text.AlignLeft
        horizontalAlignment: Text.AlignTop
    }
}
