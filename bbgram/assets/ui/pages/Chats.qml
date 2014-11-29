import bb.cascades 1.2

import "chats"

Page {
    titleBar: TitleBar {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties {
            Container {
                layout: DockLayout { }
                leftPadding: 20
                rightPadding: 20
                Label {
                    text: "Chats"
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
            imageSource: "asset:///images/menu_chat.png"
            title: "New Chat"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        
        ActionItem {
            imageSource: "asset:///images/menu_group.png"
            title: "New Group"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        
        ActionItem {
            imageSource: "asset:///images/menu_secretchat.png"
            title: "New Secret Chat"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        
        ActionItem {
            imageSource: "asset:///images/menu_broadcast.png"
            title: "New Broadcast"
            ActionBar.placement: ActionBarPlacement.InOverflow
        }
    ]
    
    Container {
        ListView {
            dataModel: _chats
            
            listItemComponents: [
                ListItemComponent {
                    ChatItem {   
                    }
                }
            ]
        }
    }
}