import bb.cascades 1.2

Container {
    id: me
    property variant author: null//{firstName: "Adam", lastName: "Smith", color:"#FFEE00"}
    property bool incoming: !ListItemData.our
    property string text: ListItemData.text
    property variant date: ListItemData.dateTime
    property bool unread: ListItemData.unread
    property variant fwdFrom: ListItemData.forwardedFrom
    property variant fwdDate: ListItemData.forwardedDate
    
    property bool selected : false
    onSelectedChanged: {
        overlay.visible = me.selected
    }

    layout: StackLayout {            
    }
   
    ControlDelegate {
        source: "messages/DateHeader.qml"
        delegateActive: me.ListItem.indexInSection == me.ListItem.sectionSize - 1
        
        horizontalAlignment: HorizontalAlignment.Center
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
                    
                    ControlDelegate {
                        sourceComponent: senderAvatarCompDef
                        delegateActive: author != null
                        minWidth: author != null ? 90 : 0
                    }
                    Container { // body
                        layout: StackLayout {
                            orientation: LayoutOrientation.TopToBottom
                        }
                        ControlDelegate {
                            sourceComponent: senderNameCompDef
                            delegateActive: author != null
                            
                            bottomMargin: 4
                        }
                        ControlDelegate {
                            sourceComponent: fwdNameCompDef
                            delegateActive: fwdFrom != null
                        }
                        Container {   
                            //preferredHeight: Infinity
                            horizontalAlignment: HorizontalAlignment.Fill 
                            verticalAlignment: VerticalAlignment.Fill
                            layout: DockLayout {}
                            ControlDelegate {
                                id: textDelegate
                                source: "messages/Text.qml"
                                delegateActive: ListItemData.mediaType != 6
                            }
                            ControlDelegate {
                                id: contactDelegate
                                source: "messages/Contact.qml"
                                delegateActive: ListItemData.mediaType == 6
                            }
                            Container { // date and read status
                                horizontalAlignment: HorizontalAlignment.Right
                                verticalAlignment: VerticalAlignment.Bottom
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                Label {
                                    text: ListItemData.formatDateTime(me.date, "hh:mm")
                                    textStyle {
                                        base: incoming ? dateInTextStyle.style : dateOutTextStyle.style
                                    }
                                    verticalAlignment: VerticalAlignment.Center
                                    rightMargin: 0
                                }
                                ControlDelegate {
                                    sourceComponent: checkGreenCompDef
                                    delegateActive: !incoming
                                    verticalAlignment: VerticalAlignment.Center
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
    
    attachedObjects: [
        TextStyleDefinition {
            id: dateTextStyle
            fontSize: FontSize.XSmall
        },
        TextStyleDefinition {
            id: dateInTextStyle
            base: dateTextStyle.style
            color: Color.Gray
        },
        TextStyleDefinition {
            id: dateOutTextStyle
            base: dateTextStyle.style
            color: Color.create('#75B166')
        },
        ComponentDefinition {
            id: senderAvatarCompDef
            Container { // avatar
                rightPadding: 10
                topPadding: 8
                bottomPadding: 8
                ImageView {
                    imageSource: author ? author.photo : ""
                    //imageSource: "asset:///images/placeholders/user_placeholder_purple.png"                            
                    minWidth: 80
                    minHeight: 80                
                    preferredWidth: 80
                    preferredHeight: 80
                }
            }
        },
        ComponentDefinition {
            id: senderNameCompDef
            Label {
                text: author ? author.firstName + " " + author.lastName : ""
                textStyle.color: author ? Color.create(author.color) : Color.Black
            }
        },
        ComponentDefinition {
            id: fwdNameCompDef
            Label {
                text: "Forwarded message<br/>From <b>" + fwdFrom.firstName + " " + fwdFrom.lastName + "</b>"
                multiline: true
                textFormat: TextFormat.Html
                textStyle.color: Color.Blue
            }
        },
        ComponentDefinition {
            id: checkGreenCompDef
            ImageView {
                imageSource: unread ? "asset:///images/check_green.png" : "asset:///images/check_2_green.png"
                leftMargin: 0
            }
        }
    ]
}