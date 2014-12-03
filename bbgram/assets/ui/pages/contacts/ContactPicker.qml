import bb.cascades 1.2

Sheet {
    id: me
    property string caption : "Caption"
    property string acceptText : "Add"
    property int prev_filter : 3
    property bool multiselect : false
    
    signal done(variant users, variant sheet)
    
    onCreationCompleted: {
        prev_filter = _contacts.filter
        _contacts.filter = 1
    }
    
    Page {
        titleBar: TitleBar {
            id: titleBar
            kind: TitleBarKind.Default
            title: caption
            acceptAction: ActionItem {
                enabled: multiselect
                id: acceptButton
                title: acceptText
                onTriggered:{
                    //_contacts.filter = prev_filter
                    
                    var result = []
                    for (var i = 0; i < contacts_list.selectionList().length; i++)
                        result.push(contacts_list.dataModel.data(contacts_list.selectionList()[i]))
                    
                    done(result, me)
                    
                    me.close()
                }
            }
            dismissAction: ActionItem {
                id: cancelButton
                title: "Cancel"
                onTriggered:{
                    //_contacts.filter = prev_filter
                    done([], me);
                    me.close()
                }
            }
        }
        Container {
            ListView {
                id: contacts_list
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
                    if (multiselect)
                        toggleSelection(indexPath)
                    else
                    {
                        if (selectionList().length == 0)
                            toggleSelection(indexPath)
                        else
                        {
                            if (selectionList()[0][0] == indexPath[0] && selectionList()[0][1] == indexPath[1])
                            {
                                toggleSelection(indexPath)
                            }
                            else
                            {
                                clearSelection()
                                toggleSelection(indexPath)
                            }
                        }  
                    }
                }

                onSelectionChanged:{
                    acceptButton.enabled = selectionList().length != 0
                    
                    if (selectionList().length > 1) {
                        titleBar.title = selectionList().length + " items selected";
                    } else if (selectionList().length == 1) {
                        titleBar.title = "1 item selected";
                    } else {
                        titleBar.title = "None selected";
                    }
                }
            }
        }
    }
}