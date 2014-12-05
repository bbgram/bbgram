import bb.cascades 1.2

Container {
    id: me
    property variant author: null //{firstName: "Adam", lastName: "Smith", color:"#FFEE00"}
    property bool incoming: !ListItemData.our
    property string text: ListItemData.text
    property variant date: ListItemData.dateTime
    property bool unread: ListItemData.unread
    
    property bool selected : false
    onSelectedChanged: {
        overlay.visible = me.selected
    }
    
    property bool withHeader: ListItem.indexInSection == ListItem.sectionSize - 1
    
    onCreationCompleted: {
        if (ListItemData.mediaType == 6)
            contactDelegate.delegateActive = true
        else
            textDelegate.delegateActive = true
    }
    
    layout: StackLayout {            
    }
    Container {
        visible: me.withHeader
        topPadding: me.withHeader ? 15 : 0
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
                text: Qt.formatDate(date, "MMMM d")
                textStyle.color: Color.White
            }
        }
        bottomMargin: 15
    
    }
    
    
    Container { // message body
        layout: DockLayout {
        }
        Container {
            minHeight: 80
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
                Container { // message
                    leftPadding: me.incoming ? 40 : 20
                    rightPadding: me.incoming ? 20 : 25
                    topPadding: 10
                    bottomPadding: 16
                    
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    
                    Container { // avatar
                        visible: author != null
                        ImageView {
                            image: author ? author.photo : null
                            //imageSource: "asset:///images/placeholders/user_placeholder_purple.png"                            
                            minWidth: 80
                            minHeight: 80                
                            preferredWidth: 80
                            preferredHeight: 80
                        }
                        rightMargin: 20
                        topPadding: 10
                        bottomPadding: 8
                    
                    }
                    Container { // body
                        layout: StackLayout {
                            orientation: LayoutOrientation.TopToBottom
                        }
                        Label {
                            visible: author != null
                            text: author ? (author.firstName + " " + author.lastName) : ""
                            textStyle.color: author ? Color.create(author.color) : Color.Black
                            bottomMargin: 4
                        
                        }
                        Container {   
                            //preferredHeight: Infinity
                            horizontalAlignment: HorizontalAlignment.Fill 
                            verticalAlignment: VerticalAlignment.Fill
                            layout: DockLayout {}
                            ControlDelegate {
                                id: textDelegate
                                source: "messages/Text.qml"
                                delegateActive: false
                            }
                            ControlDelegate {
                                id: contactDelegate
                                source: "messages/Contact.qml"
                                delegateActive: false
                            }
                            Container { // date and read status
                                horizontalAlignment: HorizontalAlignment.Right
                                verticalAlignment: VerticalAlignment.Bottom
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                Label {
                                    text: Qt.formatDateTime(me.date, "hh:mm")
                                    
                                    textStyle {
                                        color: me.incoming ? Color.Gray : Color.create('#75B166')
                                        fontSize: FontSize.XSmall
                                    }
                                    verticalAlignment: VerticalAlignment.Center
                                    rightMargin: 0
                                }
                                ImageView {
                                    visible: !incoming
                                    imageSource: unread ? "asset:///images/check_green.png" : "asset:///images/check_2_green.png"
                                    
                                    verticalAlignment: VerticalAlignment.Center
                                    leftMargin: 0
                                }
                            }
                        }
                    }
                }
            }
        
        
        
        }
        Container {
            id: overlay
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            background: Color.create('#31A3DD')
            opacity: 0.2
            visible: false
        }
    }
}