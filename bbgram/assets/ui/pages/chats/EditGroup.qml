import bb.cascades 1.2
import bbgram.types.lib 0.1

import "../settings"

Sheet {
    id: me
    
    property GroupChat chat
    
    Page {
        titleBar: TitleBar {
            title: "Edit Group"
            acceptAction: ActionItem {
                title: "Done"
                onTriggered: {
                    
                    if (groupTitle.text != chat.title)
                        _owner.setGroupName(chat, groupTitle.text);
                    
                    me.close()
                }
            }
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered: {
                    me.close()
                }
            }
        }
        Container {
            leftPadding: 20
            rightPadding: 20
            topPadding: 20
            bottomPadding: 20
            
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                ImageView {
                    verticalAlignment: VerticalAlignment.Center
                    
                    image: chat ? chat.photo : null
                    //imageSource: "asset:///images/placeholders/user_placeholder_purple.png"
                    scalingMethod: ScalingMethod.AspectFit
                    preferredHeight: 180
                    preferredWidth: 180
                } 
                
                Container {
                    verticalAlignment: VerticalAlignment.Center
                    leftPadding: 20
                    TextField {
                        id: groupTitle
                        text: chat ? chat.title : ""
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
                Divider {
                }
                SettingsDropDown {
                    title: "Sounds"
                    options: [
                        Option {
                            text: "Default"
                        },
                        Option {
                            text: "Spirit Remix"
                        }
                    ]
                    selectedIndex: 0
                }
                Divider {
                }
                
                Button {
                    horizontalAlignment: HorizontalAlignment.Center
                    
                    text: "Delete and Exit"
                }
            }   
        }        
    }
}