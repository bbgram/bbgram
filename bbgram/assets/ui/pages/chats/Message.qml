import bb.cascades 1.2

Container {
    id: me
    property bool incoming: false
    property string text: ""
    
    minHeight: 100
    leftPadding: 20
    rightPadding: 20
    topPadding: 10
    bottomPadding: 10
    
    preferredWidth: Infinity
    Container {
        horizontalAlignment: me.incoming ? HorizontalAlignment.Left : HorizontalAlignment.Right
        layout: DockLayout {}
        rightPadding: me.incoming ? 100 : 0
        leftPadding: me.incoming ? 0 : 100
        ImageView {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            imageSource: incoming ? "asset:///images/msg_in.amd" : "asset:///images/msg_out.amd"
        }
        Container {
            leftPadding: 30
            rightPadding: 40
            topPadding: 10
            bottomPadding: 14
            Label {
                
                text: me.text
                multiline: true
            }    
        }
    
    }
}