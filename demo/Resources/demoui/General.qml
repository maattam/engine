import QtQuick 2.0

Rectangle {
    id: general
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
               font.bold: true; text: "General settings" }

        CheckBoxAttribute {
            name: "Deferred rendering"
            checked: true
        }

        CheckBoxAttribute {
            id: gbuffer
            name: "Show GBuffer"
            checked: false
        }

        CheckBoxAttribute {
            name: "Show bounding boxes"
            checked: false
        }

        CheckBoxAttribute {
            name: "Show wireframe"
            checked: false
        }

        ComboBoxAttribute {
            id: scene
            name: "Scene"
        }

        function onValueChange(name, value) {
            if(name === "deferred rendering") {
                gbuffer.visible = value;
            }

            general.valueChanged(name, value);
        }
    }

    function setScenes(scenes, current) {
        scene.model = scenes;

        if(current !== undefined) {
            scene.setSelected(current);
        }
    }
}
