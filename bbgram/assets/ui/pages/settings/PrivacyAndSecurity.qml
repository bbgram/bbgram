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
                            text: "1 month"
                            value: 30
                        },
                        Option {
                            text: "3 months"
                            value: 90
                        },
                        Option {
                            text: "6 months"
                            value: 182
                        },
                        Option {
                            text: "1 year"
                            value: 365
                        }
                    ]
                    function daysToIndex(days) {
                        switch (days) {
                            case 30:
                                return 0;
                            case 90:
                                return 1;
                            case 182:
                                return 2;
                            case 365:
                                return 3;
                        }
                        return -1;
                    }
                    selectedIndex: daysToIndex(_owner.accountTTL)
                    onSelectedValueChanged: {
                        if (_owner.accountTTL != selectedValue)
                            _owner.accountTTL = selectedValue;
                    }
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