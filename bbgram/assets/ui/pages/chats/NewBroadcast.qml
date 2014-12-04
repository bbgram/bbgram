import bb.cascades 1.2

import "../settings"
import "../contacts"

Sheet {
    id: me
    property string caption : "New Broadcast"
    property int prev_filter : 3
    
    signal usersSelected(variant users, variant sheet)
    
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
                enabled: contactsList.selectedCount > 0
                title: "Create"
                
                onTriggered: {
                    var result = [];
                    for (var i = 0; i < contactsList.selectionList().length; i++)
                    {
                        result.push(contactsList.dataModel.data(indexPath))
                    }
                    usersSelected(result, me)
                    me.close()
                }
            }
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered:{
                    _contacts.filter = prev_filter
                    usersSelected([], me)
                    me.close()
                }
            }
        }
        
        Container {
            /*SettingsHeader {
                text: "Send message to.."
            }*/
            
            Divider {
            }
            ListView {
                id: contactsList
                dataModel: _contacts ? _contacts.model : null
                
                property int selectedCount : 0
                
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
                    toggleSelection(indexPath)
                }
                onSelectionChanged: {
                    selected ? selectedCount++ : selectedCount--
                }
            }
        }
    }
}