import bb.cascades 1.2

Sheet {
    id: me
    property variant userData: null
    property string caption : "Select Chat"
    property int prevFilter : 0
    
    signal chatSelected(variant chat, variant data)
    
    onCreationCompleted: {
        prevFilter = _chats.filter
        _chats.setFilter(1)
    }
    
    onClosed: {
        _chats.setFilter(prevFilter)
        chatSelected.disconnect()
    }
    
    Page {
        titleBar: TitleBar {
            id: titleBar
            kind: TitleBarKind.Default
            title: caption
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered:{
                    me.close();
                }
            }
        }
        
        Container {
            topPadding: 20
            leftPadding: 20
            rightPadding: 20
            bottomPadding: 20
            TextField {
                id: inputText
                hintText: "Search"
                
                onTextChanging: {
                    text.length > 0 ? _chats.setFilter(3, inputText.text) : _chats.setFilter(1);
                }
            }
            Divider {
            }
            ListView {
                id: chatsList
                dataModel: _chats ? _chats.model : null
                
                listItemComponents: [
                    ListItemComponent {
                        type: "item"
                        ChatItem { }
                        
                    }
                ]
                onTriggered: {
                    var chat = dataModel.data(indexPath);
                    chatSelected(chat, userData);
                    me.close();
                }
            }
        }
    }
}