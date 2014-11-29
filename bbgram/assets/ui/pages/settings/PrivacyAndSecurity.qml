import bb.cascades 1.2

Page {
    titleBar: TitleBar {
        title: "Privacy and Security"
    }
    
    ScrollView {
        Container {
            layout: StackLayout { }
            horizontalAlignment: HorizontalAlignment.Fill
            
            SettingsHeader {
                text: "Privacy"
            }
            SettingsRow {
                text: "Blocked Users"
            }
            SettingsRow {
                text: "Last Seen"
            }
            
            SettingsHeader {
                text: "Security"
            }
            SettingsRow {
                text: "Terminate all other sessions"
                arrowVisible: false
            }
            
            SettingsHeader {
                text: "Delete my account"
            }
            Container {
                leftPadding: 30
                rightPadding: 30
                topPadding: 20
                DropDown {
                    title: "If Away For"
                    options: [
                        Option {
                            text: "6 months"
                        }
                    ]
                    selectedIndex: 0
                }
                Label {
                    text: "If you do not log in at least once within this period, your account will be deleted along with all groups, messages and contacts."
                    multiline: true
                    textStyle {
                        color: Color.Gray
                        fontSize: FontSize.Small
                    }
                }
            }
        }
    }
}