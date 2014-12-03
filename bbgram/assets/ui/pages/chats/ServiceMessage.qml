import bb.cascades 1.2

Container {
    id: me
    
    property string text: ListItemData.text
    
    preferredWidth: Infinity
    Container {
        topPadding: 25
        bottomPadding: 25
        horizontalAlignment: HorizontalAlignment.Center
        layout: DockLayout {            
        }
        ImageView {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            imageSource: "asset:///images/date.amd"
        }
        Container {            
            leftPadding: 20
            rightPadding: 20
            topPadding: 4
            bottomPadding: 8
            Label {
                text: me.text
                textStyle.color: Color.White
                textStyle.textAlign: TextAlign.Center
                multiline: true;
            }
        }
    }
}