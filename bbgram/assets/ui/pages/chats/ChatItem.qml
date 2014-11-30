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
            minHeight: 140
            minWidth: 140
        }
        
        Container {
            layout: StackLayout {
            }
            verticalAlignment: VerticalAlignment.Center
            leftPadding: 10
            
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                /*ImageView {
                     imageSource: "asset:///images/secretchat_green.png"
                     preferredWidth: 40
                     preferredHeight: 40
                     verticalAlignment: VerticalAlignment.Center
                 }*/
                Label {
                    leftMargin: 0
                    text: ListItemData.title
                    //text: "firstName lastName"
                    verticalAlignment: VerticalAlignment.Center
                    textStyle {
                        fontSize: FontSize.Large
                    }
                }    
            }
            
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                rightPadding: 20
                Container {
                    layoutProperties: StackLayoutProperties  {
                        spaceQuota: 1
                    }
                    Label {
                        text: ListItemData.status
                        //text: "As they rounded a bend in the path that ran beside the river, Lara recognized the silhouette of a fig tree atop a nearby hill."
                        textStyle {
                            color: Color.Gray
                            fontSize: FontSize.Medium
                        }                    
                        horizontalAlignment: HorizontalAlignment.Left
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
                Container {
                    visible: false
                    layoutProperties: StackLayoutProperties  {
                        spaceQuota:0 
                    }
                    leftPadding: 20

                    verticalAlignment: VerticalAlignment.Center
                    
                    Container {
                        layout: DockLayout {}
                        background: Color.create("#74B266")
                        minWidth: 45
                        leftPadding: 4
                        rightPadding: 4
                        bottomPadding: 4
                        horizontalAlignment: HorizontalAlignment.Right
                        verticalAlignment: VerticalAlignment.Bottom
                        
                        Label {
                            text: "3"
                            horizontalAlignment: HorizontalAlignment.Center
                            verticalAlignment: VerticalAlignment.Center
                            textStyle {
                                color: Color.White
                                textAlign: TextAlign.Center
                                fontSize: FontSize.Small
                                fontWeight: FontWeight.Bold
                            }
                        
                        }
                    }
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

