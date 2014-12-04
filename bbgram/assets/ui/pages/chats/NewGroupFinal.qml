import bb.cascades 1.2

import "../settings"
import "../contacts"

Sheet {
    id: me
    
    property GroupDataModel selectionDataModel;
    property string groupName : "";
    
    signal created()
    
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
                    created();
                    me.close();
                }
            }
            dismissAction: ActionItem {
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
                    verticalAlignment: VerticalAlignment.Center
                    
                    //image: chat ? chat.photo : null
                    imageSource: "asset:///images/menu_plus.png"
                    scalingMethod: ScalingMethod.AspectFit
                    preferredHeight: 180
                    preferredWidth: 180
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