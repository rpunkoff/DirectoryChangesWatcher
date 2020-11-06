pragma Singleton
import QtQuick 2.0

QtObject {
    readonly property color primaryColor: "#BB86FC"
    readonly property color primaryVariantColor: "#3700B3"
    readonly property color secondaryColor: "#03DAC6"
    readonly property color secondaryVariant: "#018786"
    readonly property color backgroundColor: "#2e2f30"
    readonly property color panelsColor: "#3c3d3b"
    readonly property color errorColor: "#CF6679"
    readonly property color textColor:  "#e5e1c2"
    readonly property color scrollColor: textColor
    readonly property color themeDefaultColor: "#000000"
    readonly property color themeInvertedColor: "#FFFFFF"

    readonly property real defaultOpacity: 1
    readonly property real backgroundOpacity: 0.95
    readonly property real emphasisOpacity: 0.87
    readonly property real secondaryOpacity: 0.6
    readonly property real disabledOpacity: 0.38

    readonly property int defaultOffset: 15
    readonly property int mediumOffset: 10
    readonly property int smallOffset: 5
    readonly property int tinyOffset: 2

    readonly property int bigSpacing: 20
    readonly property int mediumSpacing: 10
    readonly property int smallSpacing: 7

    readonly property int labelFontSize: 10

    readonly property int defaultBorderWidth: 1
    readonly property int defaultBorderRadius: 3

    readonly property int panelHeight: 30
}
