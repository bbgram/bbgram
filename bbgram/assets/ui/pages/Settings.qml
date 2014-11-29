import bb.cascades 1.2
import "settings"

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
                        text: "Settings"
                        textStyle {
                            color: Color.White
                            fontSize: FontSize.Large
                        }
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
        }
        
        actions: [
            ActionItem {
                title: "Edit"
                imageSource: "asset:///images/menu_bar_edit.png"
                ActionBar.placement: ActionBarPlacement.InOverflow
            },
            ActionItem {
                title: "Ask a Question"
                imageSource: "asset:///images/menu_FAQ.png"
                ActionBar.placement: ActionBarPlacement.InOverflow
            },
            ActionItem {
                title: "Telegram FAQ"
                imageSource: "asset:///images/menu_FAQ.png"
                ActionBar.placement: ActionBarPlacement.InOverflow
                onTriggered: {
                    _owner.openFAQ();
                }
            }
        ]
        
        ScrollView {    
            Container {
                layout: StackLayout {
                }
                topPadding: 40
                bottomPadding: 40
                Container {
                    background: Color.Gray
                    preferredWidth: Infinity
                    preferredHeight: 300
                }
    
                SettingsHeader {
                    text: "Settings"
                }
                Container {
                    layout: StackLayout {}
                    bottomPadding: 20
                    SettingsRow {
                        text: "Notifications and Sounds"
                        onClicked: {
                            var page = notificationsAndSoundsDef.createObject()
                            navigationPane.push(page)
                        }
                    }
                    SettingsRow {
                        text: "Privacy and Security"
                        onClicked: {
                            var page = privacyAndSecurityDef.createObject()
                            navigationPane.push(page);
                        }
                    }
                    SettingsRow {
                        text: "Chat Settings"
                        onClicked: {
                            var page = chatSettingsDef.createObject()
                            navigationPane.push(page);
                        }
                    }
                    SettingsRow {
                        text: "Chat Background"
                        onClicked: {
                            var page = chatBackgroundDef.createObject()
                            navigationPane.push(page);
                        }
                    }
                }
    
                Button {
                    text: "Log Out"
                    horizontalAlignment: HorizontalAlignment.Center
                }
                Label {
                    text: "Telegram for Blackberry v0.1"
                    textStyle {
                        fontSize: FontSize.Small
                        color: Color.Gray
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    topMargin: 40
                }
            }
        }
        attachedObjects: [
            ComponentDefinition {                      
                id: privacyAndSecurityDef                       
                source: "settings/PrivacyAndSecurity.qml"             
            },
            ComponentDefinition {                      
                id: notificationsAndSoundsDef                       
                source: "settings/NotificationsAndSounds.qml"             
            },            
            ComponentDefinition {                      
                id: chatSettingsDef                       
                source: "settings/ChatSettings.qml"             
            },            
            ComponentDefinition {                      
                id: chatBackgroundDef                       
                source: "settings/ChatBackground.qml"             
            }
        ]
    }
}