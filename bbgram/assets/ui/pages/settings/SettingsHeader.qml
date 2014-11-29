import bb.cascades 1.2

Container {
    id: me
    property string text: "Text"
    
    layout: DockLayout {}
    horizontalAlignment: HorizontalAlignment.Fill
    preferredHeight: 60
    topPadding: 16
    Label {
        translationX: 20
        text: me.text 
        textStyle {
            fontSize: FontSize.XSmall
            fontWeight: FontWeight.W500
        }
    }
    Container {
        background: Color.create("#268DCA")
        preferredHeight: 4
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Bottom
    }
}