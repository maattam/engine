import QtQuick 2.0

Item {
    width: 150
    height: 50

    Column {
        id: column
        spacing: 5
    }

    function watchValue(name, value, unit) {
        if(column === undefined) {
            return;
        }

        var textItem;

        for(var i = 0; i < column.children.length; ++i) {
            var textObject = column.children[i];

            if(textObject.text.search(name) !== -1) {
                textItem = textObject;
                break;
            }
        }

        if(textItem === undefined) {
            textItem = Qt.createQmlObject('import QtQuick 2.0; Text { font.family: "Consolas"; font.pointSize: 10; color: "white" }',
                                                column);
        }

        textItem.text = name + ": " + value.toFixed(2);
        if(unit !== undefined) {
            textItem.text += " " + unit;
        }
    }

    function clearValues() {
        for(var i = 0; i < column.children.length; ++i) {
            var textObject = column.children[i];
            textObject.destroy();
        }
    }
}
