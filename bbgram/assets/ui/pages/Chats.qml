import bb.cascades 1.2
import bb.system 1.0

import "chats"

NavigationPane {
    id: navigationPane

    onPushTransitionEnded: {
        if (page.onPush != undefined)
            page.onPush()

        if (page.objectName == "Chat")
            while (navigationPane.count() > 2)
            navigationPane.remove(navigationPane.at(1))
    }    
    onPopTransitionEnded: {
        if (page.onPop != undefined)
        page.onPop()
        //page.destroy()
    }
    
    Page {
        titleBar: TitleBar {
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                Container {
                    layout: DockLayout { }
                    leftPadding: 20
                    rightPadding: 20
                    Label {
                        text: "Chats"
                        textStyle {
                            color: Color.White
                            fontSize: FontSize.Large
                        }
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Center
                        layoutProperties: StackLayoutProperties { spaceQuota: 1 }
                    }
                    ImageButton {
                        horizontalAlignment: HorizontalAlignment.Right
                        verticalAlignment: VerticalAlignment.Center
                        defaultImageSource: "asset:///images/header_search.png"
                    }
                }
            }
        }
        
        actions: [
            ActionItem {
                imageSource: "asset:///images/menu_chat.png"
                title: "New Chat"
                ActionBar.placement: ActionBarPlacement.InOverflow
                
                function newChatSlot(users, sheet, text)
                {
                    if (users.length > 0)
                    {
                        Application.scene.openChat(users[0])
                    }
                    sheet.done.disconnect(newChatSlot)
                }
                
                onTriggered: {
                    var sheet = contactPickerSheetDef.createObject()
                    sheet.caption = "New Chat"
                    sheet.acceptText = "Select"
                    sheet.multiselect = false
                    
                    sheet.done.connect(newChatSlot);
                    
                    sheet.open()
                }
            },
            
            ActionItem {
                imageSource: "asset:///images/menu_group.png"
                title: "New Group"
                ActionBar.placement: ActionBarPlacement.InOverflow
                
                function groupCreatedSlot(groupChat)
                {
                    _owner.groupCreated.disconnect(groupCreatedSlot)
                    
                    if (groupChat)
                        Application.scene.openChat(groupChat)
                }
                
                function newGroupSlot(users, sheet, text)
                {
                    if (users.length > 0)
                    {
                        _owner.groupCreated.connect(groupCreatedSlot)
                        _owner.createGroup(users, text)
                    }
                    sheet.done.disconnect(newGroupSlot)
                }
                
                onTriggered: {
                    var sheet = contactPickerSheetDef.createObject()
                    sheet.caption = "New Group"
                    sheet.acceptText = "Select"
                    sheet.multiselect = true
                    sheet.textHintText = "Enter Group Name"
                    sheet.textFieldVisible = true
                    
                    sheet.done.connect(newGroupSlot);
                    
                    sheet.open()
                }
            },
            
            ActionItem {
                imageSource: "asset:///images/menu_secretchat.png"
                title: "New Secret Chat"
                ActionBar.placement: ActionBarPlacement.InOverflow
                
                onTriggered: {
                    var sheet = contactPickerSheetDef.createObject()
                    sheet.caption = "New Secret Chat"
                    sheet.acceptText = "Select"
                    sheet.multiselect = false
                    
                    //callback
                    
                    sheet.open()
                }
            },
            
            ActionItem {
                imageSource: "asset:///images/menu_broadcast.png"
                title: "New Broadcast"
                ActionBar.placement: ActionBarPlacement.InOverflow
                
                onTriggered: {
                    var sheet = contactPickerSheetDef.createObject()
                    sheet.caption = "New Broadcast"
                    sheet.acceptText = "Select"
                    sheet.multiselect = true
                    
                    //callback
                    
                    sheet.open()
                }
            }
        ]
        
        Container {
            ListView {
                dataModel: _chats ? _chats : null
                stickToEdgePolicy: ListViewStickToEdgePolicy.Beginning
                
                listItemComponents: [
                    ListItemComponent {
                        ChatItem {
                            id: itemContainer
                            gestureHandlers: [
                                TapHandler {
                                    onTapped: {
                                        itemContainer.ListItem.view.openChat(ListItemData)
                                    }                
                                },
                                LongPressHandler {    
                                    onLongPressed: {
                                        confirmDialog.show()
                                    }
                                    attachedObjects: [
                                        SystemDialog {
                                            id: confirmDialog
                                            title: "Delete Chat"
                                            body: "Are you sure you want to delete this chat?"
                                            onFinished: {
                                                if (value == SystemUiResult.ConfirmButtonSelection)
                                                    itemContainer.ListItem.view.deleteChat(ListItemData)
                                            }
                                        }
                                    ]
                                }
                            
                            ]
                        }
                    }
                ]
                
                function deleteChat(chat) {
                    _owner.deleteChat(chat)
                }
                
                function openChat(chat) {
                    var page = chatPageDef.createObject()//this, {"dialog": chat})
                    page.chat = chat
                    navigationPane.push(page)
                }
                
                attachedObjects: [
                    ComponentDefinition {                      
                        id: chatPageDef                       
                        source: "Chat.qml"             
                    }
                ]
            }
        }
        attachedObjects: [
            ComponentDefinition {
                id: contactPickerSheetDef
                source: "contacts/ContactPicker.qml"
            }
        ]
    }
}