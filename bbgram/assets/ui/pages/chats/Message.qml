import bb.cascades 1.2

Container {
    id: me
    property bool incoming: false
    property string text: ""
    property variant date: null
    
    property bool selected : false
    onSelectedChanged: {
        overlay.visible = me.selected
    }
    
    layout: DockLayout {
    }
    Container {
        minHeight: 100
        leftPadding: 20
        rightPadding: 20
        topPadding: 10
        bottomPadding: 10
        preferredWidth: Infinity
        Container {
            horizontalAlignment: me.incoming ? HorizontalAlignment.Left : HorizontalAlignment.Right
            layout: DockLayout {}
            rightPadding: me.incoming ? 80 : 0
            leftPadding: me.incoming ? 0 : 80
            ImageView {
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                imageSource: incoming ? "asset:///images/msg_in.amd" : "asset:///images/msg_out.amd"
            }
            Container {            
                leftPadding: me.incoming ? 40 : 20
                rightPadding: me.incoming ? 20 : 40
                topPadding: 10
                bottomPadding: 14
                layout: DockLayout {}
                Label {
                    text: me.text + "<b>            &nbsp;</b>"
                    multiline: true
                    textFormat: TextFormat.Html
                    attachedObjects: [
                        LayoutUpdateHandler {
                            onLayoutFrameChanged: {
                            }
                        
                        }
                    ]
                }
                Label {
                    horizontalAlignment: HorizontalAlignment.Right
                    verticalAlignment: VerticalAlignment.Bottom
                    text: Qt.formatDateTime(me.date, "hh:mm")
                    
                    textStyle {
                        color: Color.Gray
                        fontSize: FontSize.XSmall
                    }
                }
            }
        
        }
    }
    
    Container {
        id: overlay
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        background: Color.Blue
        opacity: 0.2
        visible: false
    }
}