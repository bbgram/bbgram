import bb.cascades 1.2
import "contacts"

Sheet {
    id: me
    property string caption : "Caption"
    property string actionText : "Add"
    property string actionImageSource : "asset:///images/menu_bar_contact_plus.png"
    property int prev_filter : 3
    property bool multiselect : false
    
    onCreationCompleted: {
        prev_filter = _contacts.filter
        _contacts.filter = 1
    }
    
    signal done(variant users, variant sheet)
    
    Page {
        titleBar: TitleBar {
            title: caption
            dismissAction: ActionItem {
                id: cancelButton
                title: "Cancel"
                onTriggered:{
                    _contacts.filter = prev_filter
                    me.close()
                }
            }
        }
        
        actions: [
            MultiSelectActionItem{
                id: multiSelectAction
                enabled: multiselect
                onTriggered: {
                    contacts_list.multiSelectHandler.active = true;
                }
            }
        ]
        
        onCreationCompleted: {
            if (!multiselect)
                removeAction(multiSelectAction);
        }
        
        Container {
            ListView {
                id: contacts_list
                dataModel: _contacts ? _contacts.model : null
                
                multiSelectHandler
                {
                    active: multiselect
                    actions: [
                        ActionItem {
                            title: actionText
                            imageSource: actionImageSource
                            onTriggered: {
                                var result = []
                                for (var i = 0; i < contacts_list.selectionList().length; i++)
                                result.push(contacts_list.dataModel.data(contacts_list.selectionList()[i]))
                                
                                done(result, me)
                                me.close()
                            }
                        }
                    ]
                }
                
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
                        ContactItem {
                        }
                    }
                ]
                
                onTriggered: {
                    var result = []
                    result.push(dataModel.data(indexPath))
                    done(result, me)
                    me.close()
                }
                
                onSelectionChanged: {
                    multiSelectHandler.status = "Selected " + selectionList().length;
                }
                gestureHandlers: [
                    LongPressHandler {    
                        onLongPressed: {
                            if (multiselect)
                                contacts_list.multiSelectHandler.active = true
                        }
                    }
                ]
            }
        }
    }
}