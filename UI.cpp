

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import UntitledProject

Rectangle {
    id: rectangle
    width: Constants.width
    height: Constants.height
    color: "#c0bfbf"

    Button {
        id: button2
        x: 718
        y: 900
        width: 360
        height: 134
        text: qsTr("Compara y busca plagio")
        highlighted: true
        flat: true
        font.styleName: "Black"
        font.bold: true
        font.pointSize: 20
    }

    Frame {
        id: frame
        x: 86
        y: 106
        width: 596
        height: 794

        TextField {
            id: textField
            x: 0
            y: 0
            width: 572
            height: 770
            opacity: 0.5
            text: "Escriba texto aqui"
            verticalAlignment: Text.AlignTop
            font.pointSize: 15
            placeholderTextColor: "#88000000"
            placeholderText: qsTr("Inserte texto aqui")
        }

        Loader {
            id: loader
            x: 181
            y: 275
            width: 200
            height: 200

            Image {
                id: image
                x: 0
                y: 14
                width: 200
                height: 191
                opacity: 0.25
                source: "../../../../Downloads/pngwing.com.png"
                fillMode: Image.PreserveAspectFit
            }
        }
    }

    Frame {
        id: frame1
        x: 1140
        y: 110
        width: 642
        height: 794

        Loader {
            id: loader1
            x: 227
            y: 285
            width: 200
            height: 200

            Image {
                id: image1
                x: 0
                y: 0
                width: 200
                height: 191
                opacity: 0.5
                source: "../../../../Downloads/pngwing.com.png"
                fillMode: Image.PreserveAspectFit
            }
        }
    }

    Button {
        id: button
        x: 86
        y: 913
        width: 215
        height: 40
        text: qsTr("Cargar texto escrito")
        font.pointSize: 15
        highlighted: true
    }

    Button {
        id: button1
        x: 321
        y: 913
        width: 215
        height: 40
        text: qsTr("Cargar archivo subido")
        font.pointSize: 15
        highlighted: true
    }

    Button {
        id: button3
        x: 1325
        y: 913
        width: 254
        height: 40
        text: qsTr("Confirmar archivo subido")
        font.pointSize: 15
        highlighted: true
    }

    Text {
        id: _text
        x: 143
        y: 72
        color: "#ffffff"
        text: qsTr("Sube o escribe el archivo comparar aqui")
        font.pixelSize: 25
        font.styleName: "Bold Italic"
        font.family: "Arial"
    }

    Text {
        id: _text1
        x: 1267
        y: 72
        color: "#ffffff"
        text: qsTr("Sube el archivo fuente aqui")
        font.pixelSize: 25
        font.styleName: "Bold Italic"
        font.family: "Arial"
    }
}
