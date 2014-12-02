import bb.cascades 1.2
import bb.system 1.0
import bbgram.types.lib 0.1

import "chats"

Page {
    objectName: "Chat"
    property variant dialog: null
    
    function messageAdded(indexPath) {
        if (indexPath.length != 0 && indexPath[0] == 0)
            messages.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.None);
        if (!messages.dataModel.data(indexPath).our)
            _owner.markRead(dialog);
    }
    
    function onPop() {
        messages.dataModel.itemAdded.disconnect(messageAdded)
    }
    
    function onPush() {
        _owner.markRead(dialog);
        
        messages.dataModel.itemAdded.connect(messageAdded)
    }
    
    function sendMessage() {
        _owner.sendMessage(dialog, message.text)
        message.text = ""
    }
    
    function clearHistory(chat) {
        //_owner.clearHistory(chat)
    }
    
    function about() {
        var page = contactPageDef.createObject()
        page.user = dialog.user
        var navigationPane = Application.scene.activeTab.content 
        navigationPane.push(page)
    }
    
    titleBar: TitleBar {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties {
            Container {
                gestureHandlers: [
                    TapHandler {
                        onTapped: {
                            about()
                        }
                    }
                ]
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
            id: sendAction
            enabled: false
            title: "Send"
            imageSource: "asset:///images/bar_send.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            
            onTriggered: {
                sendMessage()
            }
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
            onTriggered: {
                about();
            }
        },
        ActionItem {
            title: "Shared Media"
            imageSource: "asset:///images/menu_sharedmedia.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            title: "Clear Chat"
            imageSource: "asset:///images/menu_bin.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
            onTriggered: confirmDialog.show()
            attachedObjects: [
                SystemDialog {
                    id: confirmDialog
                    title: "Clear History"
                    body: "Are you sure you want to clear history?"
                    onFinished: {
                        if (value == SystemUiResult.ConfirmButtonSelection)
                            clearHistory(ListItemData)
                    }
                }
            ]
        }
    ]
    
    Container {
        layout: StackLayout {
        }
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
                id: messages
                dataModel: dialog ? dialog.messages : null
                
                verticalAlignment: VerticalAlignment.Bottom
                stickToEdgePolicy: ListViewStickToEdgePolicy.Beginning
                
                multiSelectHandler {
                    actions: [
                        ActionItem {
                            title: "Remove message"
                            imageSource: "asset:///images/menu_bin.png"
                            onTriggered: {
                                console.log("Action remove message")
                            }
                        }
                    ]
                }
                
                listItemComponents: [
                    ListItemComponent {
                        Message {
                            id: chat_message
                            incoming: !ListItemData.our
                            text: ListItemData.text
                            date: ListItemData.date
                            
                            ListItem.onSelectionChanged: {
                                chat_message.selected = selected;
                            }
                        }
                    }
                ]
                
                onSelectionChanged: {
                    messages.multiSelectHandler.status = "Selected: " + selectionList().length
                }
                
                gestureHandlers: [
                    LongPressHandler {    
                        onLongPressed: {
                            messages.multiSelectHandler.active = true
                        }
                    }
                ]
            }
        }
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            leftPadding: 20
            rightPadding: 20
            topPadding: 10
            bottomPadding: 20
            horizontalAlignment: HorizontalAlignment.Fill
            background: Color.create("#252525")
            ImageButton {
                verticalAlignment: VerticalAlignment.Center
                defaultImageSource: "asset:///images/bar_voice1.png"
                pressedImageSource: "asset:///images/bar_voice2.png"
                preferredWidth: 90
                preferredHeight: 90
            }
            TextArea {
                id: message
                verticalAlignment: VerticalAlignment.Center
                focusPolicy: FocusPolicy.KeyAndTouch
                //text: "asdasjdh asdk jhasd asd jahsd kasd haskdjha asjdasdasdh"
                hintText: "Enter a message"
                onTextChanging: {
                    sendAction.enabled = text.length > 0;
                }
                input {
                    onSubmitted: {
                        sendMessage()
                    }
                }
            }
            ImageButton {
                verticalAlignment: VerticalAlignment.Center
                defaultImageSource: "asset:///images/bar_smile.png"
                pressedImageSource: "asset:///images/bar_smile.png"
                preferredWidth: 90
                preferredHeight: 90
            }
        }
    }
    
    attachedObjects: [
        ComponentDefinition {
            id: contactPageDef
            source: "ContactInfo.qml"
        }
    ]
}
