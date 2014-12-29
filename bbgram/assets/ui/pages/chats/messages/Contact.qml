import bb.cascades 1.2

Container {
    property variant contact: me.ListItem.view.owner.getUser(ListItemData.media.user_id);
    layout: StackLayout {
    }
    Container {
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            Container {
                topPadding: 10
                ImageView {
                    imageSource: contact.photo
                    //imageSource: "asset:///images/placeholders/user_placeholder_purple.png"                            
                    minWidth: 80
                    minHeight: 80                
                    preferredWidth: 80
                    preferredHeight: 80
                }
            }
            Container {
                leftPadding: 10
                topPadding: 0
                bottomPadding: 0
                verticalAlignment: VerticalAlignment.Center
                layout: StackLayout {
                }
                Label {
                    text: ListItemData.media.first_name + " " + ListItemData.media.last_name
                    textStyle.color: Color.create(contact.color)
                    multiline: true
                    bottomMargin: 0
                
                }
                Label {
                    text: ListItemData.media.phone
                    textStyle.fontSize: FontSize.Small
                    multiline: true
                    topMargin: 0
                    bottomMargin: 0
                }
            }
        }
    }   
    Container {
        minHeight: 50
    }
}