import bb.cascades 1.2
import bb.cascades.pickers 1.0

import "../settings"
import "../contacts"

Sheet {
    id: me
    
    property GroupDataModel selectionDataModel;
    property string groupName : "";
    property string chatPhoto : "";
    
    signal created()
    
    function groupCreatedSlot(groupChat)
    {
        _owner.groupCreated.disconnect(groupCreatedSlot)
        
        if (groupChat)
            Application.scene.openChat(groupChat)
            
        created();
        me.close();
    }
    
    Page {
        titleBar: TitleBar {
            id: titleBar
            kind: TitleBarKind.Default
            title: "New Group"
            acceptAction: ActionItem {
                id: acceptAction
                enabled: groupName.length > 0
                title: "Create"
                
                onTriggered: {
                    if (!selectionDataModel.isEmpty())
                    {
                        var idx = 0
                        var users = [];
                        var it = selectionDataModel.first();
                        while (idx < selectionDataModel.size())
                        {
                            users.push(selectionDataModel.data(it));
                            it = selectionDataModel.after(it);
                            idx++;
                        }
                        it = null

                        _owner.groupCreated.connect(groupCreatedSlot)
                        _owner.createGroup(users, groupName, chatPhoto)
                        
                        acceptAction.enabled = false
                        backAction.enabled = false
                    }
                    else
                    {
                        created();
                        me.close();
                    }
                }
            }
            dismissAction: ActionItem {
                id: backAction
                title: "Back"
                onTriggered:{
                    me.close()
                }
            }
        }
        
        Container {
            leftPadding: 20
            rightPadding: 20
            topPadding: 20
            bottomPadding: 20
            
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                ImageView {
                    id: groupImageView
                    verticalAlignment: VerticalAlignment.Center
                    
                    //image: chat ? chat.photo : null
                    imageSource: "asset:///images/menu_plus.png"
                    scalingMethod: ScalingMethod.AspectFit
                    preferredHeight: 180
                    preferredWidth: 180
                    
                    gestureHandlers: TapHandler {
                        onTapped: {
                            filePicker.open()
                        }
                    }
                    
                    attachedObjects: [
                        FilePicker {
                            id: filePicker
                            type : FileType.Picture
                            title : "Select Picture"
                            mode: FilePickerMode.Picker
                            directories : ["/accounts/1000/shared/"]
                            onFileSelected : {
                                
                                groupImageView.imageSource =  "file://" + selectedFiles[0];
                                chatPhoto = selectedFiles[0];
                            }
                        }
                    ]
                } 
                
                Container {
                    verticalAlignment: VerticalAlignment.Center
                    leftPadding: 20
                    TextField {
                        hintText: "Group Name"
                        onTextChanging: {
                            groupName = text
                        }
                    }
                }
            }
            
            SettingsHeader {
                text: "Participants"
            }
            
            ListView {
                id: membersList
                dataModel: selectionDataModel
                
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
                    //toggleSelection(indexPath)
                }
            }
        }
    }
}