import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: template
    width: 200
    height: 45
    color: "transparent"

    property string name
    property alias maximumValue: slider.maximumValue
    property alias minimumValue: slider.minimumValue
    property alias value: slider.value

    Column {
        width: parent.width; height: parent.height
        spacing: 5

        Text {
            width: 80; height: 20
            font.pointSize: 11;
            font.family: "Consolas"
            color: "white";

            text: "%1 (%2)".arg(name).arg(slider.value.toFixed(2))
        }

        Slider {
            id: slider
            width: 200
            height: 20
            maximumValue: maximum
            minimumValue: minimum
            value: valueInitial

            onValueChanged: {
                var target = template.parent;
                if(target !== null) {
                    target.onValueChange(name.toLowerCase(), value);
                }
            }
        }
    }
}
