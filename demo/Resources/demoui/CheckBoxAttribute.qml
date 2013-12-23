import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: template
    width: 200
    height: 20
    color: "transparent"

    property string name
    property alias checked: checkBox.checked

    Row {
        width: parent.width; height: parent.height
        spacing: 5

        Text {
            height: 20
            font.pointSize: 11;
            font.family: "Consolas"
            color: "white";

            text: name + ":"
        }

        CheckBox {
            anchors.verticalCenter: parent.verticalCenter
            id: checkBox

            onCheckedChanged: {
                var target = template.parent
                if(target) target.onValueChange(name.toLowerCase(), checked)
            }
        }
    }
}
