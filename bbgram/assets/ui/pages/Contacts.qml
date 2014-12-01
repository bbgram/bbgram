import bb.cascades 1.2
import "contacts"

NavigationPane {
    id: navigationPane
    Page {
        titleBar: TitleBar {
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                Container {
                    layout: DockLayout {}
                    leftPadding: 20
                    rightPadding: 20
                    Label {
                        text: "Contacts"
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
                id: contacts_list
                dataModel: _contacts ? _contacts.model : null
                
                property MultiSelectHandler currentMultiSelectHandler : multiSelectHandler
                
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
                    //multiSelectHandler.status = "Selected" + selectionList().length;
                    selectionList().valueOf()
                }
                
                onTriggered: {
                    var page = contactPageDef.createObject();
                    page.user = dataModel.data(indexPath);
                    navigationPane.push(page);
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
                            contacts_list.currentMultiSelectHandler = inviteFriendsMultiSelectHandler;
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
                            contacts_list.currentMultiSelectHandler = newSecretChatMultiSelectHandler;
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
                            contacts_list.currentMultiSelectHandler = newBroadcastMultiSelectHandler;
                        }
                    }
                    
                ]
            }
        }
        
        attachedObjects: [
            ComponentDefinition {
                id: contactPageDef
                source: "ContactInfo.qml"
            },
            ComponentDefinition {
                id: addContactSheetDef
                source: "contacts/AddContact.qml"
            }
        ]
    }
}