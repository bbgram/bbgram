import bb.cascades 1.2
import bb.system 1.0

import "chats"

NavigationPane {
    id: navigationPane

    onPushTransitionEnded: {
        if (page.onPush != undefined)
            page.onPush()
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
            },
            
            ActionItem {
                imageSource: "asset:///images/menu_group.png"
                title: "New Group"
                ActionBar.placement: ActionBarPlacement.InOverflow
            },
            
            ActionItem {
                imageSource: "asset:///images/menu_secretchat.png"
                title: "New Secret Chat"
                ActionBar.placement: ActionBarPlacement.InOverflow
            },
            
            ActionItem {
                imageSource: "asset:///images/menu_broadcast.png"
                title: "New Broadcast"
                ActionBar.placement: ActionBarPlacement.InOverflow
            }
        ]
        
        Container {
            ListView {
                dataModel: _chats
                
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
                    var page = dialogPageDef.createObject()//this, {"dialog": chat})
                    page.dialog = chat
                    navigationPane.push(page)
                }
                
                attachedObjects: [
                    ComponentDefinition {                      
                        id: dialogPageDef                       
                        source: "Dialog.qml"             
                    }
                ]
            }
        }
    }
}