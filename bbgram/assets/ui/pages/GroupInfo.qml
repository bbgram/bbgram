import bb.cascades 1.2
import bbgram.types.lib 0.1

import "settings"
import "contacts"
import "chats"

Page {
    property GroupChat chat
    
    actions: [
        ActionItem {
            title: "Edit"
            imageSource: "asset:///images/menu_bar_edit.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            
            onTriggered: {
                var sheet = editGroupSheetDef.createObject();
                sheet.chat = chat;
                sheet.open();
            }
        },
        ActionItem {
            id: addParticipantAction
            title: "Add Participant"
            imageSource: "asset:///images/menu_bar_contact_plus.png"
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
            id: sharedMediaAction
            title: "Shared Media"
            imageSource: "asset:///images/menu_sharedmedia.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        DeleteActionItem {
            title: "Clear and Exit"
            imageSource: "asset:///images/menu_bin.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        }
    ]
    
    ScrollView {
        Container {
            Container {
                leftPadding: 20
                topPadding: 20
                rightPadding: 20
                bottomPadding: 20
                
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                ImageView {
                    verticalAlignment: VerticalAlignment.Center
                    
                    image: chat ? chat.photo : null
                    //imageSource: "asset:///images/placeholders/user_placeholder_purple.png"
                    scalingMethod: ScalingMethod.Fill
                    preferredHeight: 200
                    preferredWidth: 200
                }
                
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.TopToBottom
                    }
                    verticalAlignment: VerticalAlignment.Center
                    
                    Label {
                        text: chat ? chat.title : ""
                        //text: "firstName lastName"
                        textStyle {
                            fontSize: FontSize.Large
                        }
                    }
                    
                    Label {
                        text: "Members Online"
                        //text: "last seen ..."
                        textStyle {
                            color: Color.Gray
                            fontSize: FontSize.Large
                        }
                    }
                }
            }
            
            Container {
                layout: StackLayout {
                }
                horizontalAlignment: HorizontalAlignment.Fill
                
                SettingsHeader {
                    text: "Settings"
                }
                SettingsToggleButton {
                    text: "Notifications"
                }
                SettingsRow {
                    text: "Shared Media"
                }
                
                SettingsHeader {
                    text: "Participants"
                }
                
                ListView {
                    id: groupList
                    dataModel: _contacts ? _contacts.model : null
                    
                    multiSelectHandler {
                        actions: [
                            ActionItem {
                                title: "Delete from Group"
                                imageSource: "asset:///images/menu_bin.png"
                                onTriggered: {
                                    console.log("Delete from froup")                                
                                }
                            }
                        ]
                    }
                    
                    gestureHandlers: [
                        LongPressHandler {
                            onLongPressed: {
                                groupList.multiSelectHandler.active = true;
                            }
                        }
                    ]
                    
                    listItemComponents: [
                        ListItemComponent {
                            type: "item"
                            ContactItem { }
                        }
                    ]
                    
                    onSelectionChanged: {
                        groupList.multiSelectHandler.status = "Selected: " + selectionList().length
                    }
                }
            }
        }
    }
    attachedObjects: ComponentDefinition {
        id: editGroupSheetDef
        source: "chats/EditGroup.qml"
    }
}
