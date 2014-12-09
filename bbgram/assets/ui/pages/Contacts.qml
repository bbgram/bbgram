import bb.cascades 1.2
import "contacts"

NavigationPane {
    id: navigationPane
    Page {
        titleBar: TitleBar {
            id: titleBar
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                Container {
                    layout: DockLayout {}
                    leftPadding: 20
                    rightPadding: 20
                    topPadding: 10
                    Label {
                        id: contactsLabel
                        text: "Contacts"
                        textStyle {
                            color: Color.White
                            fontSize: FontSize.Large
                        }
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
                        
                        TextField {
                            topPadding: 10
                            id: searchField
                            visible: false
                            hintText: "Search"
                            //horizontalAlignment: HorizontalAlignment.Left
                            verticalAlignment: VerticalAlignment.Center
                            
                            onTextChanging: {
                                _contacts.setFilter(_contacts.filter | 4, text);
                            }
                        }
                        
                        Label {
                            id: cancelLabel
                            visible: false
                            text: "Cancel"
                            textStyle {
                                color: Color.White
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
                contactsLabel.visible = !contactsLabel.visible;
                searchField.visible = !searchField.visible;
                searchButton.visible = !searchButton.visible;
                cancelLabel.visible = !cancelLabel.visible;
            }
        }
        
        actions: [
            ActionItem {
                title: "Add New Contact"
                imageSource: "asset:///images/menu_plus.png"
                ActionBar.placement: ActionBarPlacement.InOverflow
                
                onTriggered: {
                    var sheet = addContactSheetDef.createObject();
                    sheet.open();
                }
            },
            MultiSelectActionItem {
                title: "Invite Friends"
                imageSource: "asset:///images/menu_bar_contact_plus.png"
                multiSelectHandler: inviteFriendsMultiSelectHandler
            },
            MultiSelectActionItem {
                title: "New Secret Chat"
                imageSource: "asset:///images/menu_secretchat.png"
                multiSelectHandler: newSecretChatMultiSelectHandler
            },
            MultiSelectActionItem {
                title: "New Broadcast"
                imageSource: "asset:///images/menu_broadcast.png"
                multiSelectHandler: newBroadcastMultiSelectHandler
            }
        ]
        
        Container {
            ListView {
                leadingVisual: Container {
                    topPadding: 20
                    bottomPadding: 20
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1
                    }
                     Label {
                         preferredWidth: Infinity
                         verticalAlignment: VerticalAlignment.Center                         
                         text: "%1 Contacts".arg(_contacts ? _contacts.telegramContactsCount : 0)
                         textStyle.fontSize: FontSize.XLarge
                         textStyle.textAlign: TextAlign.Center
                     }
                }
                id: contacts_list
                dataModel: _contacts ? _contacts.model : null
                
                listItemComponents: [
                    ListItemComponent {
                        type: "header"
                        Header {
                            visible: ListItemData != "!"
                            title: ListItemData
                        }
                    },
                    ListItemComponent {
                        type: "item"
                        ContactItem { }
                    }
                ]
                
                onSelectionChanged: {
                    //multiSelectHandler.status = "Selected " + selectionList().length;
                }
                
                onTriggered: {
                    var user = dataModel.data(indexPath);
                    if (user.sortingKey == "!") {
                        Application.scene.openChat(user);
                    }
                    else {
                        if (user.phone != undefined && user.phone != "")
                            _owner.sendInviteText(user.phone, "Hey, let's switch to Telegram: http://telegram.org/dl2")
                    }
                }
                
                attachedObjects: [
                    MultiSelectHandler {
                        id: inviteFriendsMultiSelectHandler
                        actions: [
                            ActionItem {
                                title: "Invite Friends"
                                imageSource: "asset:///images/menu_bar_contact_plus.png"
                                onTriggered: {
                                    console.log("Action invite friends")
                                }
                            }
                        ]
                        onActiveChanged: {
                            active ? _contacts.setFilter(2) : _contacts.setFilter(3)
                        }
                    },
                    MultiSelectHandler {
                        id: newSecretChatMultiSelectHandler
                        actions: [
                            ActionItem {
                                title: "New Secret Chat"
                                imageSource: "asset:///images/menu_secretchat.png"
                                onTriggered: {
                                    console.log("SecretChat invite")
                                }
                            }
                        ]
                        onActiveChanged: {
                            active ? _contacts.setFilter(1) : _contacts.setFilter = (3)
                        }
                    },
                    MultiSelectHandler {
                        id: newBroadcastMultiSelectHandler
                        actions: [
                            ActionItem {
                                title: "New Broadcast"
                                imageSource: "asset:///images/menu_broadcast.png"
                                onTriggered: {
                                    console.log("Briadcast invite")
                                }
                            }
                        ]
                        onActiveChanged: {
                            active ? _contacts.setFilter(1) : _contacts.setFilter(3)
                        }
                    }
                    
                ]
            }
        }
        
        attachedObjects: [
            ComponentDefinition {
                id: addContactSheetDef
                source: "contacts/AddContact.qml"
            },
            ComponentDefinition {
                id: newBroadcastSheetDef
                source: "chats/NewBroadcast.qml"
            }
        ]
    }
}