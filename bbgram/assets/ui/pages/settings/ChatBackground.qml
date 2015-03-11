import bb.cascades 1.2
import "../chats"

Page {
    titleBar: TitleBar {
        title: "Chat Background"
    }
    
    Container {
        layout: StackLayout {
        }
        
        Container {
            layout: DockLayout { 
            }
            
            Wallpaper {
                id: wallpaper
            }
            ActivityIndicator {
                id: indicator
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                preferredWidth: 150
            }
        }
        Container {
            background: Color.White
            minHeight: 200
            preferredHeight: 200
            ListView {
                dataModel: _owner.getWallpapers()
                layout: StackListLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                listItemComponents: [
                    ListItemComponent {
                        CustomListItem {                        
                            preferredWidth: 200
                            preferredHeight: 200
                            Container{                                
                                background: ListItemData.isSolid ? Color.create('#' + ListItemData.backgroundColor.toString(16)) : Color.Transparent
                                horizontalAlignment: HorizontalAlignment.Fill
                                verticalAlignment: VerticalAlignment.Fill
                                ImageView {
                                    preferredWidth: Infinity
                                    preferredHeight: Infinity
                                    visible: !ListItemData.isSolid
                                    imageSource: ListItemData.thumbnail
                                    scalingMethod: ScalingMethod.AspectFill
                                }
                            }
                        }
                    }
                ]
                function loaded(url) {
                    wallpaper.source = url;
                    _owner.wallpaper = wallpaper.source;
                    indicator.stop();
                }
                onTriggered: {
                    var w = dataModel.data(indexPath);
                    if (!w.isSolid) {
                        wallpaper.source = w.thumbnail;
                        indicator.start();
                        w.loaded.connect(loaded);
                        w.load();
                    }
                    else {
                        wallpaper.source = '#' + w.backgroundColor.toString(16);
                        _owner.wallpaper = wallpaper.source;
                    }
                }
            }
        }
    }
}