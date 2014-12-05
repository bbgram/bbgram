import bb.cascades 1.2

Sheet {
    id: me
    property string caption : "Select Contact"
    property int prev_filter : 3
    
    signal userSelected(variant user, variant sheet)
    
    onCreationCompleted: {
        prev_filter = _contacts.filter
        _contacts.filter = 1
    }
    
    Page {
        titleBar: TitleBar {
            id: titleBar
            kind: TitleBarKind.Default
            title: caption
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered:{
                    _contacts.filter = prev_filter
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