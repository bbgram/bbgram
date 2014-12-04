import bb.cascades 1.2

ListView {
    id: me
    property bool groupChat: false
    property variant messages: null
    
    layout: StackListLayout {
        orientation: LayoutOrientation.BottomToTop
    }

    dataModel: messages ? messages : null
    
    verticalAlignment: VerticalAlignment.Bottom
    stickToEdgePolicy: ListViewStickToEdgePolicy.Beginning
    //snapMode: SnapMode.LeadingEdge
    scrollIndicatorMode: ScrollIndicatorMode.None
    
    multiSelectHandler {
        actions: [
            DeleteActionItem {
                onTriggered: {
                    for (var i = 0; i < me.selectionList().length; i++)
                    {
                        _owner.deleteMessage(me.messages.data(me.selectionList()[i]).id);
                    }
                
                }
            }
        ]
    }
    
    listItemComponents: [
        ListItemComponent {
            type: "message"
            Message {
                id: chatMessage
                author: ListItem.view.groupChat && !ListItemData.our ? ListItemData.from : null
                ListItem.onSelectionChanged: {
                    chatMessage.selected = selected;
                }
            }
        },
        ListItemComponent {
            type: "service_message"
            ServiceMessage {
            }
        },
        ListItemComponent {
            type: "photo"
            Photo {
            }
        },
        ListItemComponent {
            type: "header"
            Container {
                visible: false
                preferredHeight: 0
            }
        }
    ]
    
    onSelectionChanged: {
        me.multiSelectHandler.status = "Selected: " + me.selectionList().length
    }
    
    gestureHandlers: [
        LongPressHandler {    
            onLongPressed: {
                me.multiSelectHandler.active = true
            }
        }
    ]
}