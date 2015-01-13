import bb.cascades 1.2

/*StandardListItem {
 property bool fromPhone: ListItemData.sortingKey != "!"
 image: if (!fromPhone) ListItemData.photo
 imageSource: if (fromPhone) "asset:///images/placeholders/user_placeholder_grayscale.png"
 title: ListItemData.firstName + " " + ListItemData.lastName
 //title: "firstName lastName"
 description: ListItemData.sortingKey != "!" ? "" : (ListItemData.online ? "online" : "last seen " + ListItemData.lastSeenFormatted)
 //description: "last seen "
 }*/

CustomListItem {
    property bool fromPhone: ListItemData.sortingKey != "!"
    
    highlightAppearance: HighlightAppearance.Full
    
    preferredHeight: 100
    
    content: Container {
        id:background
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        
        ImageView {
            verticalAlignment: VerticalAlignment.Center

            imageSource: fromPhone ? "asset:///images/placeholders/user_placeholder_grayscale.png" : ListItemData.photo
            
            scalingMethod: ScalingMethod.AspectFit
            preferredHeight: 100
            preferredWidth: 100
            minHeight: 100
            minWidth: 100
        }
        
        Container {
            layout: StackLayout {
            }
            verticalAlignment: VerticalAlignment.Center
            leftPadding: 10
            
            Container {
                rightPadding: 20
                Label {
                    text: ListItemData.firstName + " " + ListItemData.lastName
                    //text: "firstName lastName"
                    verticalAlignment: VerticalAlignment.Center
                    textStyle {
                        fontSize: FontSize.Large
                    }
                }
            }
            
            Container { 
                rightPadding: 20
                Label {
                    text: ListItemData.sortingKey != "!" ? "" : (ListItemData.online ? "online" : "last seen " + ListItemData.lastSeenFormatted)
                    verticalAlignment: VerticalAlignment.Center
                    textStyle {
                        color: Color.Gray
                        fontSize: FontSize.Medium
                    }
                }               
            }
        }
    
    }
}

