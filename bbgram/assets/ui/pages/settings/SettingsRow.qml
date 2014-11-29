import bb.cascades 1.2

Container {
    id: me
    property string text: "Text"
    property bool arrowVisible: true
    signal clicked()
    
    layout: DockLayout {}
    Container {
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Fill
        preferredHeight: 110
        layout: DockLayout { }
        leftPadding: 20
        rightPadding: 20
        
        Label {
            text: me.text
            textStyle {
                fontSize: FontSize.Large
            }
            horizontalAlignment: HorizontalAlignment.Left
            verticalAlignment: VerticalAlignment.Center
        }
        ImageView {
            imageSource: "asset:///images/setting_arrow.png"
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Center
            visible: me.arrowVisible
        }
    }
    Divider { verticalAlignment: VerticalAlignment.Bottom }
    
    ImageButton {
        pressedImageSource: "asset:///images/frame.amd"
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        visible: true
        
        onClicked: {
            me.clicked()
        }
    }
 }