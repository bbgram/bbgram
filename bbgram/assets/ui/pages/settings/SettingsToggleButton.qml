import bb.cascades 1.2

Container {
    id: me
    property string text: "Text"
    
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
        ToggleButton {
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Center
        }
    }
    Divider { verticalAlignment: VerticalAlignment.Bottom }
}