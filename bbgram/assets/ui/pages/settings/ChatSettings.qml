import bb.cascades 1.2

Page {
    titleBar: TitleBar {
        title: "Chat Settings"
    }
    
    ScrollView {
        Container {
            layout: StackLayout { }
            horizontalAlignment: HorizontalAlignment.Fill
            
            SettingsHeader {
                text: "Automatic Photo Download"
            }
            SettingsToggleButton {
                text: "Private Chats"
                checked: settings.value("Automatic Photo Download/Private Chats", false)
                onCheckedChanged: {
                    settings.setValue("Automatic Photo Download/Private Chats", checked)
                }
            }
            SettingsToggleButton {
                text: "Groups"
                checked: settings.value("Automatic Photo Download/Groups", false)
                onCheckedChanged: {
                    settings.setValue("Automatic Photo Download/Groups", checked)
                }
            }
            
            SettingsHeader {
                text: "Automatic Audio Download"
            }
            SettingsToggleButton {
                text: "Private Chats"
                checked: settings.value("Automatic Audio Download/Private Chats", false)
                onCheckedChanged: {
                    settings.setValue("Automatic Audio Download/Private Chats", checked)
                }
            }
            SettingsToggleButton {
                text: "Groups"
                checked: settings.value("Automatic Audio Download/Groups", false)
                onCheckedChanged: {
                    settings.setValue("Automatic Audio Download/Groups", checked)
                }
            }
            SettingsToggleButton {
                text: "Autoplay Audio"
                checked: settings.value("Automatic Audio Download/Autoplay Audio", false)
                onCheckedChanged: {
                    settings.setValue("Automatic Audio Download/Autoplay Audio", checked)
                }
            }
        }
    }
}