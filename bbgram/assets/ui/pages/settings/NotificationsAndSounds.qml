import bb.cascades 1.2

Page {
    titleBar: TitleBar {
        title: "Notifications and Sounds"
    }
    
    ScrollView {
        Container {
            layout: StackLayout { }
            horizontalAlignment: HorizontalAlignment.Fill
            
            SettingsHeader {
                text: "Message Notifications"
            }
            SettingsToggleButton {
                text: "Alert"
                checked: settings.value("Message Notifications/Alert", false)
                onCheckedChanged: {
                    settings.setValue("Message Notifications/Alert", checked)
                }
            }
            SettingsToggleButton {
                text: "Message Preview"
                checked: settings.value("Message Notifications/Message Preview", false)
                onCheckedChanged: {
                    settings.setValue("Message Notifications/Message Preview", checked)
                }
            }
            SettingsDropDown {
                title: "Sound"
                options: [
                    Option {
                        text: "Default"
                    }
                ]
                selectedIndex: settings.value("Message Notifications/Sound", 0)
                
                onSelectedIndexChanged: {
                    settings.setValue("Message Notifications/Sound", selectedIndex)
                }
            }
            
            SettingsHeader {
                text: "Group Notifications"
            }
            SettingsToggleButton {
                text: "Alert"
                checked: settings.value("Group Notifications/Alert", false)
                onCheckedChanged: {
                    settings.setValue("Group Notifications/Alert", checked)
                }
            }
            SettingsToggleButton {
                text: "Message Preview"
                checked: settings.value("Group Notifications/Message Preview", false)
                onCheckedChanged: {
                    settings.setValue("Group Notifications/Message Preview", checked)
                }
            }
            SettingsDropDown {
                title: "Sound"
                options: [
                    Option {
                        text: "Default"
                    }
                ]
                selectedIndex: settings.value("Group Notifications/Sound", 0)
                
                onSelectedIndexChanged: {
                    settings.setValue("Group Notifications/Sound", selectedIndex)
                }
            }
            
            SettingsHeader {
                text: "In-App Notifications"
            }
            SettingsToggleButton {
                text: "In-App Sounds"
                checked: settings.value("In-App Notifications/In-App Sounds", false)
                onCheckedChanged: {
                    settings.setValue("In-App Notifications/In-App Sounds", checked)
                }
            }
            SettingsToggleButton {
                text: "In-App Vibrate"
                checked: settings.value("In-App Notifications/In-App Vibrate", false)
                onCheckedChanged: {
                    settings.setValue("In-App Notifications/In-App Vibrate", checked)
                }
            }
            SettingsToggleButton {
                text: "In-App Preview"
                checked: settings.value("In-App Notifications/In-App Preview", false)
                onCheckedChanged: {
                    settings.setValue("In-App Notifications/In-App Preview", checked)
                }
            }
            
            SettingsHeader {
                text: "Reset"
            }
            SettingsRow {
                text: "Reset All Notifications"
                arrowVisible: false
            }
        }
    }
}