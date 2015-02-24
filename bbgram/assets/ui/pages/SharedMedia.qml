import bb.cascades 1.2
import bbgram.types.lib 0.1

Page {
    property Peer peer
    
    titleBar: TitleBar {
        kind: TitleBarKind.FreeForm
        kindProperties: FreeFormTitleBarKindProperties {
            Container {
                layout: DockLayout {}
                leftPadding: 20
                rightPadding: 20
                Label {
                    text: "Shared Media"
                    textStyle.base: titleTextStyle.style
                    horizontalAlignment: HorizontalAlignment.Left
                    verticalAlignment: VerticalAlignment.Center
                }
            }
        }
    }
    
    Container {
        topPadding: 20
        SegmentedControl {
            id: filter
            Option {
                id: mediaOption
                text: "Media"
            
            }
            Option {
                id: filesOption
                text: "Files"
                selected: true
            }
        }
        
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
                /*dataModel: XmlDataModel {
                 source: "asset:///dm_countries.xml"
                 }*/
                dataModel: peer ? peer.sharedMedia : null
                listItemComponents: [
                    ListItemComponent {
                        //type: "country"
                        Container {
                            layout: DockLayout {
                            }
                            ImageView {
                                imageSource: "asset:///images/background_hd.jpg"
                                scalingMethod: ScalingMethod.AspectFill
                            }
                            Label {
                                text: ListItemData.code
                                horizontalAlignment: HorizontalAlignment.Center
                                verticalAlignment: VerticalAlignment.Center
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