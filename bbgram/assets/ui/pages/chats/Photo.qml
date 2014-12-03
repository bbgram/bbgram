import bb.cascades 1.2
import bbgram.control.lib 0.1

Container {
    id: me
    
    property bool incoming: !ListItemData.our
    property variant date: ListItemData.dateTime
    property bool unread: ListItemData.unread
    
    leftPadding: incoming ? 20 : 100
    rightPadding: incoming ? 100 : 20
    preferredWidth: Infinity
    implicitLayoutAnimationsEnabled: false
    Container {
        implicitLayoutAnimationsEnabled: false
        topPadding: 15
        bottomPadding: 15
        
        horizontalAlignment: incoming ? HorizontalAlignment.Left : HorizontalAlignment.Right
        layout: DockLayout {            
        }
        ImageView {
            implicitLayoutAnimationsEnabled: false
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            imageSource: incoming ? "asset:///images/media_in.amd" : "asset:///images/media_out.amd"
        }
        Container {
            implicitLayoutAnimationsEnabled: false
            id: content
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            layout: DockLayout {
            }
            leftPadding: 12
            rightPadding: 12
            topPadding: 12
            bottomPadding: 12
            MediaViewer {
                id: mediaViewer
                message: ListItemData
                maxWidth: 200
                /*horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill  */
            }
            Container {
                implicitLayoutAnimationsEnabled: false
                layout: DockLayout {                
                }
                horizontalAlignment: HorizontalAlignment.Right
                verticalAlignment: VerticalAlignment.Bottom
                rightPadding: 10
                bottomPadding: 10
                ImageView {
                    implicitLayoutAnimationsEnabled: false
                    imageSource: "asset:///images/system_black.amd"
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                }
                Container {
                    implicitLayoutAnimationsEnabled: false
                    leftPadding: 8
                    rightPadding: 8
                    bottomPadding: 4
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    Label {
                        implicitLayoutAnimationsEnabled: false
                        text: Qt.formatDateTime(me.date, "hh:mm")
                        
                        textStyle{
                            color: Color.White
                            fontSize: FontSize.XSmall   
                        }
                        rightMargin: 5
                    }
                    ImageView {
                        implicitLayoutAnimationsEnabled: false
                        visible: !incoming
                        imageSource: me.unread ? "asset:///images/check_white.png" : "asset:///images/check_2_white.png"
                        
                        verticalAlignment: VerticalAlignment.Center
                        leftMargin: 5
                    }
                }
            
            }
        }
    }
    attachedObjects: [
        LayoutUpdateHandler {
            onLayoutFrameChanged: {
                mediaViewer.maxWidth = layoutFrame.width * 0.8
            }
        }
    ]
}