import bb.cascades 1.2
import bbgram.types.lib 0.1

Page {
    property Peer peer
    
    titleBar: TitleBar {
        id: filter        
        kind: TitleBarKind.Segmented
        options: [
            Option {
                text: "Shared Media"
                selected: true
            },
            Option {
                text: "Files"
            }
        ]
    }
    
    Container {
        topPadding: 20
        ControlDelegate {
            sourceComponent: mediaListCompDef
            delegateActive: filter.selectedIndex == 0
            implicitLayoutAnimationsEnabled: false
        }
        
        ControlDelegate {
            sourceComponent: filesListCompDef
            delegateActive: filter.selectedIndex == 1
            implicitLayoutAnimationsEnabled: false
        }
    }
    
    attachedObjects: [
        TitleTextStyleDefinition {
            id: titleTextStyle
            fontSize: FontSize.Large
        },
        ComponentDefinition {
            id: mediaListCompDef
            ListView {
                leftPadding: 20
                rightPadding: 20
                layout: GridListLayout {
                    columnCount: 4
                }             
                function itemType(data, indexPath) {
                    if (data.flags == 1)
                        return "photo";
                    else
                        return "video";
                } 
                dataModel: peer ? peer.sharedMedia : null
                listItemComponents: [
                    ListItemComponent {
                        type: "photo"
                        ImageView {
                            //imageSource: "asset:///images/background_hd.jpg"
                            imageSource: ListItemData.thumbnail
                            scalingMethod: ScalingMethod.AspectFill
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Fill
                        }
                    },
                    ListItemComponent {
                        type: "video"
                        Container {
                            layout: DockLayout {                                
                            }
                            ImageView {
                                imageSource: ListItemData.thumbnail
                                scalingMethod: ScalingMethod.AspectFill
                                horizontalAlignment: HorizontalAlignment.Fill
                                verticalAlignment: VerticalAlignment.Fill
                            }
                            Container {
                                horizontalAlignment: HorizontalAlignment.Fill
                                verticalAlignment: VerticalAlignment.Bottom
                                background: Color.create("#66000000")
                                Container {
                                    leftPadding: 10
                                    bottomPadding: 5
                                    rightPadding: 5
                                    layout: StackLayout {
                                        orientation: LayoutOrientation.LeftToRight
                                    }
                                    ImageView {
                                        implicitLayoutAnimationsEnabled: false
                                        imageSource: "asset:///images/video.png"
                                        verticalAlignment: VerticalAlignment.Center
                                        rightMargin: 0
                                    }
                                    Label {           
                                        leftMargin: 10                         
                                        verticalAlignment: VerticalAlignment.Center
                                        text: ListItemData.durationFormatted
                                        textStyle {
                                            fontSize: FontSize.XSmall
                                            color: Color.White    
                                        }
                                    }
                                }
                            }
                        }
                    }
                ]
            }
        },
        ComponentDefinition {
            id: filesListCompDef
            ListView {
                leftPadding: 20
                rightPadding: 20
                layout: StackListLayout {                
                }
                horizontalAlignment: HorizontalAlignment.Fill
                dataModel: peer ? peer.sharedFiles : null
                listItemComponents: [
                    ListItemComponent {
                        CustomListItem {
                            Container {
                                layout: StackLayout {
                                    orientation: LayoutOrientation.LeftToRight
                                }
                                
                                Container {
                                    layout: DockLayout {                                        
                                    }
                                    
                                    preferredWidth: 100
                                    preferredHeight: 100
                                    verticalAlignment: VerticalAlignment.Center
                                    Label {
                                        horizontalAlignment: HorizontalAlignment.Center
                                        verticalAlignment: VerticalAlignment.Center
                                        text: ListItemData.extension
                                    }
                                    
                                    ImageView {
                                        preferredWidth: 100
                                        preferredHeight: 100
                                        horizontalAlignment: HorizontalAlignment.Center
                                        verticalAlignment: VerticalAlignment.Center
                                        imageSource: ListItemData.thumbnail
                                        scalingMethod: ScalingMethod.AspectFill
                                    }  
                                }
                                Container {
                                    layout: StackLayout {
                                        orientation: LayoutOrientation.LeftToRight
                                    }
                                    Container {
                                        layout: StackLayout {
                                        }
                                        leftPadding: 20
                                        topPadding: 10
                                        bottomPadding: 10
                                        Label {
                                            bottomMargin: 0
                                            text: ListItemData.caption
                                            textStyle.base: fileCaptionTextStyle.style
                                        }
                                        Label {
                                            topMargin: 0
                                            text: ListItemData.details
                                            textStyle.base: fileDetailsTextStyle.style
                                        }
                                        
                                        attachedObjects: [
                                            TextStyleDefinition {
                                                id: fileCaptionTextStyle
                                                fontSize: FontSize.Large
                                                fontStyle: FontStyle.Default
                                            },
                                            TextStyleDefinition {
                                                id: fileDetailsTextStyle
                                                color: Color.create("#555555")
                                                fontSize: FontSize.Small
                                            }
                                        ]
                                    }
                                }
                            }
                        }
                    }
                ]
            }            
        }
    ]
}