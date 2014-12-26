import bb.cascades 1.2

Container {
    id: me
    property string text: "Text"
    property bool checked: false
    
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
            checked: me.checked
            onCheckedChanged: {
                if (me.checked != checked)
                    me.checked = checked
            }
        }
    }
    Divider { verticalAlignment: VerticalAlignment.Bottom }
}