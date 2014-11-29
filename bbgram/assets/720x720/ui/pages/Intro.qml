import bb.cascades 1.2

import "intro"

NavigationPane {
    id: navigationPane
    
    Page {
        Container {
            id: intro
            property variant indicators: []
            
            layout: DockLayout {}
            topPadding: 100 
            bottomPadding: 40
            
            ImageView {
                id: icon_image
                preferredWidth: 200
                preferredHeight: 200
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Top  
            }
            
            CarouselView {
                id: carousel
                preferredWidth: Infinity
                preferredHeight: Infinity
                dataModel: XmlDataModel { source: "../../dm_intro_page.xml" }
                
                onCreationCompleted: {
                    var component = Qt.createComponent("intro/Indicator.qml");
                    var n = carousel.dataModel.childCount(0)
                    var arr = []
                    for (var i = 0; i < n; i++) {
                        var indicator = component.createObject(parent);
                        indicator_container.add(indicator)
                        arr.push(indicator)
                    }
                    intro.indicators = arr
                }
                
                onVisibleItemDataChanged: {
                    var n = carousel.dataModel.childCount(0)
                    for (var i = 0; i < n; i++) {
                        var indicator = intro.indicators[i] 
                        if (visibleItemData && carousel.dataModel.data([i]).icon == visibleItemData.icon)
                            indicator.background = Color.create(visibleItemData.color)
                        else
                            indicator.background = Color.Gray
                    }
                    icon_image.imageSource = visibleItemData ? visibleItemData.icon : ""
                }
                
                listItemComponents: ListItemComponent {
                    type: "page"
        
                    Container {                
                        minHeight: ListItem.view.layoutFrame.height
                        preferredWidth: ListItem.view.layoutFrame.width
                        topPadding: 240
                        
                        Label {
                            text: ListItemData.title
                            horizontalAlignment: HorizontalAlignment.Center
                            
                            textStyle {
                                color: Color.Black
                                fontSize: FontSize.PointValue
                                fontSizeValue: 10  
                            }
                        }
                            
                        Label {
                            text: "<html><p style='text-align:center'>" + ListItemData.message + "</p></html>"
                            horizontalAlignment: HorizontalAlignment.Center
                            multiline: true
                            textFormat: TextFormat.Html
        
                            textStyle {
                                color: Color.create("#808080")
                                fontSize: FontSize.PointValue
                                fontSizeValue: 6
                            }
                        }
                    }
                }
            }
            
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                topPadding: 425
                Button {
                    text: "Start messaging"
                    onClicked: {
                        var page = loginPageDef.createObject()
                        navigationPane.push(page)
                    }
                }
            }
            
            Container {
                id: indicator_container
                layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Bottom
            }
        }
        attachedObjects: [                  
            ComponentDefinition {                      
                id: loginPageDef                       
                source: "Login.qml"             
            }
        ]
    }
}