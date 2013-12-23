import QtQuick 2.0

Rectangle {
    id: tonemap
    width: column.width + 24
    height: column.height + 24
    color: "#99000000"
    border.width: 0

    signal valueChanged(string name, variant value)

    Column {
        id: column
        anchors.centerIn: parent

        spacing: 5

        Text { font.pointSize: 14; font.family: "Consolas"; color: "white";
               font.bold: true; text: "Tonemap settings" }

        SliderAttribute {
            name: "Bloom threshold"
            minimumValue: 0
            maximumValue: 5
            value: 1.2
        }

        SliderAttribute {
            name: "Bloom factor"
            minimumValue: 0
            maximumValue: 5
            value: 0.25
        }

        SliderAttribute {
            name: "Bright level"
            minimumValue: 0
            maximumValue: 10
            value: 5
        }

        SliderAttribute {
            name: "Gamma"
            minimumValue: 1
            maximumValue: 3
            value: 2.2
        }

        CheckBoxAttribute {
            name: "Automatic exposure"
            checked: true
        }

        SliderAttribute {
            id: exposure
            name: "Exposure"
            minimumValue: 0
            maximumValue: 5
            value: 1
            visible: false
        }

        function onValueChange(name, value) {
            if(name === "automatic exposure") {
                exposure.visible = !value
                if(value) {
                    tonemap.valueChanged("exposure", -1)
                }

                else {
                    tonemap.valueChanged("exposure", exposure.value)
                }
            }

            else {
                tonemap.valueChanged(name, value)
            }
        }
    }
}
