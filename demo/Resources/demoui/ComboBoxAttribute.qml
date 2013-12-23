import QtQuick 2.0
import QtQuick.Controls 1.1

Rectangle {
    id: template
    width: 200
    height: childrenRect.height
    color: "transparent"

    property string name
    property alias model: comboBox.model

    Row {
        width: parent.width;
        spacing: 5

        Text {
            height: 20
            font.pointSize: 11;
            font.family: "Consolas"
            color: "white";

            text: name + ":"
        }

        ComboBox {
            id: comboBox
            width: 120
            height: 20

            onCurrentIndexChanged: {
                var target = template.parent
                if(target) target.onValueChange(name.toLowerCase(), textAt(currentIndex))
            }
        }
    }
}
