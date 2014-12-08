import bb.cascades 1.2

Sheet {
    id: me
    property string caption : "Select Contact"
    property int prev_filter : 3
    
    signal userSelected(variant user, variant sheet)
    
    onCreationCompleted: {
        prev_filter = _contacts.filter
        _contacts.setFilter(1)
    }
    
    Page {
        titleBar: TitleBar {
            id: titleBar
            kind: TitleBarKind.Default
            title: caption
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered:{
                    _contacts.setFilter(prev_filter)
                    userSelected(null, me)
                    me.close()
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
                    text.length > 0 ? _contacts.setFilter(1 | 4, inputText.text) : _contacts.setFilter(1);
                }
            }
            Divider {
            }
            ListView {
                id: contactsList
                dataModel: _contacts ? _contacts.model : null
                
                listItemComponents: [
                    ListItemComponent {
                        type: "header"
                        Header {
                            visible: ListItemData != "!"
                            title: ListItemData
                        }
                    },
                    ListItemComponent {
                        type: "item"
                        ContactItem { }
                        
                    }
                ]
                onTriggered: {
                    var user = dataModel.data(indexPath);
                    userSelected(user, me)
                    me.close()
                }
            }
        }
    }
}