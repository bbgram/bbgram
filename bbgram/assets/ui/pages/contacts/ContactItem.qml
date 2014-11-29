import bb.cascades 1.2

Container {
    Container {
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }

        
        ImageView {
            verticalAlignment: VerticalAlignment.Center
            
            image: ListItemData.photo
            //imageSource: "asset:///images/placeholders/user_placeholder_purple.png"
            scalingMethod: ScalingMethod.AspectFit
            preferredHeight: 140
            preferredWidth: 140
        }
        
        Container {
            layout: StackLayout {
            }
            verticalAlignment: VerticalAlignment.Center
            leftPadding: 10
            
            Container {
                Label {
                    text: ListItemData.firstName + " " + ListItemData.lastName
                    //text: "firstName lastName"
                    verticalAlignment: VerticalAlignment.Top
                    textStyle {
                        fontSize: FontSize.Large
                    }
                }    
            }
            
            Container {
                Label {
                    text: ListItemData.status
                    //text: "status"
                    textStyle {
                        color: Color.Gray
                        fontSize: FontSize.Medium
                    }
                    verticalAlignment: VerticalAlignment.Top
                }
            }
        }
    }
    Container {
        background: Color.White
        
        Divider {
        }
    }
}
    
