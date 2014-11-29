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
            }
            SettingsToggleButton {
                text: "Message Preview"
            }
            SettingsDropDown {
                title: "Sound"
                options: [
                    Option {
                        text: "Default"
                    }
                ]
                selectedIndex: 0
            }
            
            SettingsHeader {
                text: "Group Notifications"
            }
            SettingsToggleButton {
                text: "Alert"
            }
            SettingsToggleButton {
                text: "Message Preview"
            }
            SettingsDropDown {
                title: "Sound"
                options: [
                    Option {
                        text: "Default"
                    }
                ]
                selectedIndex: 0
            }
            
            SettingsHeader {
                text: "In-App Notifications"
            }
            SettingsToggleButton {
                text: "In-App Sounds"
            }
            SettingsToggleButton {
                text: "In-App Vibrate"
            }
            SettingsToggleButton {
                text: "In-App Preview"
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