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
            id: titleBar
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                Container {
                    layout: DockLayout { }
                    leftPadding: 20
                    rightPadding: 20
                    Label {
                        id: chatsLabel
                        text: "Chats"
                        textStyle.base: titleTextStyle.style
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Center
                    }
                    ImageButton {
                        id: searchButton
                        horizontalAlignment: HorizontalAlignment.Right
                        verticalAlignment: VerticalAlignment.Center
                        defaultImageSource: "asset:///images/header_search.png"
                        
                        onClicked: {
                            titleBar.flipVisibility();
                        }
                    }
                    
                    Container {
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        verticalAlignment: VerticalAlignment.Center
                        TextField {
                            topPadding: 10
                            id: searchField
                            visible: false
                            hintText: "Search"
                            //horizontalAlignment: HorizontalAlignment.Left
                            verticalAlignment: VerticalAlignment.Center
                            
                            onTextChanging: {
                                _chats.setFilter(1 | 2, text);
                            }
                        }
                        
                        Label {
                            id: cancelLabel
                            visible: false
                            text: "Cancel"
                            textStyle {
                                color: SystemDefaults.TextStyles.TitleText.color
                            }
                            verticalAlignment: VerticalAlignment.Center
                            
                            gestureHandlers: TapHandler {
                                onTapped: {
                                    titleBar.flipVisibility();
                                }
                            }
                        }
                    }
                }
            }
            
            function flipVisibility()
            {
                chatsLabel.visible = !chatsLabel.visible;
                searchField.visible = !searchField.visible;
                searchButton.visible = !searchButton.visible;
                cancelLabel.visible = !cancelLabel.visible;
            }
        }
        
        actions: [
            ActionItem {
                imageSource: "asset:///images/menu_chat.png"
                title: "New Chat"
                ActionBar.placement: ActionBarPlacement.InOverflow
                
                function newChatSlot(user, sheet)
                {
                    sheet.userSelected.disconnect(newChatSlot)
                    
                    if (user)
                        Application.scene.openChat(user)
                    
                }
                
                onTriggered: {
                    var sheet = contactPickerSheetDef.createObject()
                    sheet.userSelected.connect(newChatSlot);
                    sheet.open()
                }
            },
            
            ActionItem {
                imageSource: "asset:///images/menu_group.png"
                title: "New Group"
                ActionBar.placement: ActionBarPlacement.InOverflow
                
                onTriggered: {
                    var sheet = newGroupSheetDef.createObject()
                    sheet.open()
                }
            },
            
            ActionItem {
                imageSource: "asset:///images/menu_secretchat.png"
                title: "New Secret Chat"
                ActionBar.placement: ActionBarPlacement.InOverflow
                
                onTriggered: {
                }
            },
            
            ActionItem {
                imageSource: "asset:///images/menu_broadcast.png"
                title: "New Broadcast"
                ActionBar.placement: ActionBarPlacement.InOverflow
                
                function newBroadcastSlot(users, sheet)
                {
                    sheet.usersSelected.disconnect(newBroadcastSlot)
                    
                    if (users.length > 0)
                    {
                        Application.scene.openChat(_owner.createBroadcast(users));
                    }
                }
                
                onTriggered: {
                    var sheet = newBroadcastSheetDef.createObject()
                    sheet.usersSelected.connect(newBroadcastSlot)
                    sheet.open()
                }
            }
        ]
        
        Container {
            ListView {
                id: chatsList
                dataModel: _chats ? _chats.model : null
                stickToEdgePolicy: ListViewStickToEdgePolicy.Beginning
                
                multiSelectHandler{
                    
                    
                    actions: [
                        ActionItem {
                            title: "Clear History";
                            imageSource: "asset:///images/menu_deletehistory.png"
                            onTriggered: {
                                clearHistoryDialog.selectedChat = chatsList.dataModel.data(chatsList.selectionList()[0])
                                clearHistoryDialog.show();
                            }
                        },
                        ActionItem {
                            title: "Delete And Exit";
                            imageSource: "asset:///images/menu_bin.png"
                            onTriggered: {
                                deleteChatDialog.selectedChat = chatsList.dataModel.data(chatsList.selectionList()[0])
                                deleteChatDialog.show();
                            }
                        }
                    ]
                }
                
                listItemComponents: [
                    ListItemComponent {
                        type: "item"
                        ChatItem {
                            
                            id: itemContainer
                            
                            gestureHandlers: [
                                TapHandler {
                                    onTapped: {
                                        if (itemContainer.ListItem.view.multiSelectHandler.active != true)
                                            itemContainer.ListItem.view.openChat(ListItemData)
                                        else
                                        {
                                            for (var i = 0; i < itemContainer.ListItem.view.selectionList().length; i++)
                                                itemContainer.ListItem.view.toggleSelection(itemContainer.ListItem.view.selectionList()[i]);
                                            
                                            itemContainer.ListItem.view.toggleSelection(itemContainer.ListItem.view.indexPath);
                                        }
                                    }                
                                },
                                LongPressHandler {    
                                    onLongPressed: {
                                        itemContainer.ListItem.view.multiSelectHandler.active = true;
                                    }
                                }
                            ]
                        }
                    }
                ]
                
                function deleteChat(chat) {
                    _owner.deleteChat(chat)
                }
                
                function clearHistory(chat){
                    _owner.deleteHistory(chat)
                }
                
                function openChat(chat) {
                    var page = chatPageDef.createObject()//this, {"dialog": chat})
                    page.peer = chat
                    navigationPane.push(page)
                }
                
                attachedObjects: [
                    ComponentDefinition {                      
                        id: chatPageDef                       
                        source: "Chat.qml"             
                    },
                    SystemDialog {
                        property variant selectedChat : null
                        id: deleteChatDialog
                        title: "Delete Chat"
                        body: "Are you sure you want to delete this chat?"
                        onFinished: {
                            if (value == SystemUiResult.ConfirmButtonSelection)
                                chatsList.deleteChat(selectedChat)
                        }
                    },
                    SystemDialog {
                        property variant selectedChat : null
                        id: clearHistoryDialog
                        title: "Clear History"
                        body: "Are you sure you want to clear history?"
                        onFinished: {
                            if (value == SystemUiResult.ConfirmButtonSelection)
                                chatsList.clearHistory(selectedChat)
                        }
                    }
                ]
            }
        }
        attachedObjects: [
            TitleTextStyleDefinition {
                id: titleTextStyle
                fontSize: FontSize.Large
            },
            ComponentDefinition {
                id: contactPickerSheetDef
                source: "contacts/ContactPicker.qml"
            },
            ComponentDefinition {
                id: newBroadcastSheetDef
                source: "chats/NewBroadcast.qml"
            },
            ComponentDefinition {
                id: newGroupSheetDef
                source: "chats/NewGroup.qml"
            }
        ]
    }
}