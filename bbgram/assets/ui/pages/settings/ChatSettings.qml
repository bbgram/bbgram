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
            }
            SettingsToggleButton {
                text: "Groups"
            }
            
            SettingsHeader {
                text: "Automatic Audio Download"
            }
            SettingsToggleButton {
                text: "Private Chats"
            }
            SettingsToggleButton {
                text: "Groups"
            }
            SettingsToggleButton {
                text: "Autoplay Audio"
            }
        }
    }
}