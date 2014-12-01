import bb.cascades 1.2
import bbgram.types.lib 0.1

import "chats"

Page {
    property Dialog dialog
    
    titleBar: TitleBar {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties {
            Container {

                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                ImageView {
                    image: dialog ? dialog.photo : null
                    //imageSource: "asset:///images/placeholders/user_placeholder_purple.png"
                    scalingMethod: ScalingMethod.AspectFit
                    maxWidth: 110
                }
                
                Container {
                    layout: StackLayout {
                    }
                    verticalAlignment: VerticalAlignment.Center
                    leftPadding: 20
                    Label {
                        text: dialog ? dialog.title : ""
                        //text: "Anastasiya Shy"
                        bottomMargin: 0
                        textStyle {
                            color: Color.White
                            fontSize: FontSize.Large
                        }
                        horizontalAlignment: HorizontalAlignment.Left
                    }
                    Label {
                        text: dialog ? (dialog.user.online ? "online" : "last seen " + dialog.user.lastSeenFormatted) : ""
                        topMargin: 0
                        textStyle {
                            color: Color.White
                            fontSize: FontSize.Small
                        }
                        horizontalAlignment: HorizontalAlignment.Left
                    }
                }
            }
        }
    }
    
    actions: [
        ActionItem {
            title: "Attach"
            imageSource: "asset:///images/bar_attach.png"
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
            title: "Send"
            imageSource: "asset:///images/bar_send.png"
            ActionBar.placement: ActionBarPlacement.OnBar
        },
        ActionItem {
            title: "Call"
            imageSource: "asset:///images/menu_phone.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            title: "About"
            imageSource: "asset:///images/bar_profile.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            title: "Shared Media"
            imageSource: "asset:///images/menu_sharedmedia.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            title: "Block Contact"
            imageSource: "asset:///images/menu_contactblock.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            title: "Delete Contact"
            imageSource: "asset:///images/menu_bin.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        }
    ]
    
    Container {
        layout: DockLayout {            
        }
        ImageView {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            imageSource: "asset:///images/background_hd.jpg"
            scalingMethod: ScalingMethod.AspectFill
        }
        ListView {
            layout: StackListLayout {
                orientation: LayoutOrientation.BottomToTop
            }
            id: listView
            verticalAlignment: VerticalAlignment.Bottom
            
            dataModel: dialog ? dialog.messages : null
            
            stickToEdgePolicy: ListViewStickToEdgePolicy.Beginning
            
            
            listItemComponents: [
                ListItemComponent {
                    Message {
                        incoming: !ListItemData.our
                        text: ListItemData.text
                        date: ListItemData.date
                    }
                }
            
            ]
        }
    }
}
