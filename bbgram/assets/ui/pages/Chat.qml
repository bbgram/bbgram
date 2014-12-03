import bb.cascades 1.2
import bb.system 1.0
import bbgram.types.lib 0.1

import "chats"

Page {
    objectName: "Chat"
    property variant chat: null
    
    function messageAdded(indexPath) {
        if (indexPath.length != 0 && indexPath[0] == 0)
            messageList.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.None);
        if (!messageList.dataModel.data(indexPath).our)
            _owner.markRead(chat);
    }
    
    function onPop() {
        messageList.dataModel.itemAdded.disconnect(messageAdded)
    }
    
    function onPush() {
        _owner.markRead(chat);
        
        messageList.dataModel.itemAdded.connect(messageAdded)
    }
    
    function sendMessage() {
        _owner.sendMessage(chat, message.text)
        message.text = ""
    }
    
    function clearHistory(chat) {
        //_owner.clearHistory(chat)
    }
    
    function about() {
        if (chat.type == 1) {// user
            var page = contactPageDef.createObject()
            page.user = chat
            var navigationPane = Application.scene.activeTab.content 
            navigationPane.push(page)
        }
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
                    image: chat ? chat.photo : null
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
                        text: chat ? chat.title : ""
                        //text: "Anastasiya Shy"
                        bottomMargin: 0
                        textStyle {
                            color: Color.White
                            fontSize: FontSize.Large
                        }
                        horizontalAlignment: HorizontalAlignment.Left
                    }
                    Label {
                        text: chat && chat.type == 1 ? (chat.online ? "online" : "last seen " + chat.lastSeenFormatted) : ""
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
            id: attachAction
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
            id: callAction
            title: "Call"
            imageSource: "asset:///images/menu_phone.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
            onTriggered: {
                _owner.dialANumber("+" + chat.phone)
            }
        },
        ActionItem {
            id: aboutAction
            title: "About"
            imageSource: "asset:///images/bar_profile.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
            onTriggered: {
                about();
            }
        },
        ActionItem {
            id: addParticipantAction
            title: "Add Participant"
            imageSource: "asset:///images/menu_bar_contact_plus.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            id: sharedMediaAction
            title: "Shared Media"
            imageSource: "asset:///images/menu_sharedmedia.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
        },
        ActionItem {
            id: clearHistoryAction
            title: "Clear Chat"
            imageSource: "asset:///images/menu_bin.png"
            ActionBar.placement: ActionBarPlacement.InOverflow
            onTriggered: confirmDialog.show()
            attachedObjects: [
                SystemDialog {
                    id: confirmDialog
                    title: clearHistoryAction.title
                    body: "Are you sure you want to clear history?"
                    onFinished: {
                        if (value == SystemUiResult.ConfirmButtonSelection)
                            clearHistory(ListItemData)
                    }
                }
            ]
        }
    ]
    
    onChatChanged: {
        if (!chat)
            return
        if (chat.type != 1)    // User
            removeAction(callAction)
        if (chat.type != 2)  // Group
            removeAction(addParticipantAction)
        if (chat.type == 2) {
            clearHistoryAction.title = "Clear Conversation"
            aboutAction.imageSource = "asset:///images/menu_group.png"
        }
    }
    
    
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
            MessageList {
                id: messageList
                messages: chat && chat.messages != undefined ? chat.messages : null
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
                    submitKey: SubmitKey.Send
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
