import bb.cascades 1.2

ListView {
    id: me
    property variant peer: null
    property variant owner: _owner
    
    layout: StackListLayout {
        orientation: LayoutOrientation.BottomToTop
    }
    
    dataModel: peer ? peer.messages : null
    
    verticalAlignment: VerticalAlignment.Bottom
    stickToEdgePolicy: ListViewStickToEdgePolicy.Beginning
    //snapMode: SnapMode.LeadingEdge
    scrollIndicatorMode: ScrollIndicatorMode.None
    
    multiSelectHandler {
        actions: [
            ActionItem {
                id: fwdAction
                title: "Forward"
                imageSource: "asset:///images/menu_forward.png"
                enabled: peer ? peer.type < 3 : false
                
                function forwardMessages(chat, messages) {
                    _owner.forwardMessages(messages, chat);
                    Application.scene.openChat(chat);
                }
                
                onTriggered: {
                    var messages = [];
                    var list = me.selectionList();
                    for (var i = 0; i < list.length; i++) {
                        messages.push(me.dataModel.data(list[i]));
                    }
                    var sheet = chatPickerSheetDef.createObject();
                    sheet.userData = messages;
                    sheet.chatSelected.connect(forwardMessages);
                    sheet.open();
                }
            },
            ActionItem {
                id: copyAction
                title: "Copy"
                imageSource: "asset:///images/menu_copy.png"
                
                onTriggered: {
                    var messages = [];
                    var list = me.selectionList();
                    for (var i = 0; i < list.length; i++) {
                        messages.push(me.dataModel.data(list[i]));
                    }
                    _owner.copyMessagesToClipboard(messages);
                }
            },
            DeleteActionItem {
                onTriggered: {
                    for (var i = 0; i < me.selectionList().length; i++) {
                        _owner.deleteMessage(me.dataModel.data(me.selectionList()[i]).id);
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
                author: (ListItem.view.peer && ListItem.view.peer.type == 2  && !ListItemData.our) ? ListItemData.from : null
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
    
    attachedObjects: [
        ListScrollStateHandler {
            onAtEndChanged: {
                if (dataModel && firstVisibleItem.length > 0 && atEnd)
                    peer.loadAdditionalHistory()
            }
        },
        ComponentDefinition {
            id: chatPickerSheetDef
            source: "ChatPicker.qml"
        }
    ]
}