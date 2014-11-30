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
            },
            ActionItem {
                title: "Invite Friends"
                imageSource: "asset:///images/menu_bar_contact_plus.png"
                ActionBar.placement: ActionBarPlacement.InOverflow
            },
            ActionItem {
                title: "New Secret Chat"
                imageSource: "asset:///images/menu_secretchat.png"
                ActionBar.placement: ActionBarPlacement.InOverflow
            },
            ActionItem {
                title: "New Broadcast"
                imageSource: "asset:///images/menu_broadcast.png"
                ActionBar.placement: ActionBarPlacement.InOverflow
            }
        ]
        
        Container {
            ListView {
                dataModel: _contacts ? _contacts : null
                  
                listItemComponents: [
                    ListItemComponent {
                        ContactItem { }
                    }
                ]
                
                onTriggered: {
                    var page = contactPageDef.createObject();
                    page.user = dataModel.data(indexPath);
                    navigationPane.push(page);
                }
            }
        }
        
        attachedObjects: ComponentDefinition {
            id: contactPageDef
            source: "ContactInfo.qml"
        }
    }
}