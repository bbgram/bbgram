/*
 * Copyright (c) 2013 Macadamian Technologies Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 import bb.cascades 1.0

/**
 * CarouselView
 *
 * A ListView with items that take up the full size its viewport and 
 * snap to the edges. Defaults to LeftToRight layout but works well in
 * any orientation.
 *
 * You are expected to provide your own listItemComponents that follow
 * this pattern:
 * 
 *  ListItemComponent {
 *      type: "your item type"
 *
 *      Container {
 *          /* Bind this container's size to the ListView's layoutFrame
 *           * to make it take up the whole listview space. */
/*          preferredHeight: ListItem.view.layoutFrame.height
 *          preferredWidth: ListItem.view.layoutFrame.width
 *
 *          // Your content here
 *      }
 *  }
 *
 * If you do not specify your own listItemComponents, a default one is 
 * provided that shows dataModel items' first field in a centered Label.
 *
 * TODOs:
 *   - Currently will probably not work with GroupDataModel, using the default
 *     list item component for headers too. Maybe we could ignore headers and
 *     instead expose the group's key as a property?
 */
ListView {
    /**
     * A QRectF obtained from LayoutUpdateHandler layoutFrameChanged signal
     */
    property variant layoutFrame

    /**
     * ListItemData for the currently visible item
     *
     * N.B. Because of constraints with Cascades, this will actually be the
     * item that is currently visible at the leading edge of the the ListView
     * i.e. If you are scrolling to the left (moving items to the right), the
     * property will be updated the instant the hidden item on the left reveals
     * itself vs. scrolling to the right where it will be updated only after the
     * current item is completely hidden to the left and the new item's leading 
     * edge reaches the ListView's leading edge.
     */
    property variant visibleItemData 


    /*
     * This is where the magic happens, making this ListView a CarouselView.
     */
    id: me
    layout: StackListLayout { orientation: LayoutOrientation.LeftToRight } // Works fine with any orientation
    snapMode: SnapMode.LeadingEdge
    flickMode: FlickMode.SingleItem
    scrollIndicatorMode: ScrollIndicatorMode.None
    attachedObjects: [
        LayoutUpdateHandler { onLayoutFrameChanged: me.layoutFrame = layoutFrame }, 
        ListScrollStateHandler { onFirstVisibleItemChanged: me.visibleItemData = dataModel.data(firstVisibleItem) }
    ]
    listItemComponents: ListItemComponent {
        type: "__carouselitem" // Our hopefully non-conflicting item type

        Container {
            /* Bind this container's size to the ListView's layoutFrame
             * to make it take up the whole listview space. */
            preferredHeight: ListItem.view.layoutFrame.height
            preferredWidth: ListItem.view.layoutFrame.width
    
            layout: DockLayout { }
            Label {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                // text:
                property variant data: ListItemData
                onDataChanged: {
                    var typeofItemData = typeof ListItemData;

                    switch (typeofItemData) {
                        // When ListItemData is an object we show the first property we find
                        case "object": {
                            for (var i in ListItemData) {
                                text = ListItemData[i];
                                break;
                            }
                        }
                        break;

                        // TODO: Add cases for other types here
                        
                        // When we don't know how to handle a type we'll just show it's type name and log an error
                        default: {
                            console.log("CarouselView's default ListItemComponent can't handle ListItemData of type " + typeofItemData);
                            text = typeofItemData;
                        }
                        break;
                    }
                }
            } // end Label
        } // end Container
    } // end ListItemComponent

    function itemType(data, indexPath) {
        /* Check if any of the current listItemComponents handles the type of the 
         * item the ListView is asking for. If it doesn't, return our default item. */  
        var type = me.dataModel.itemType(indexPath);

        for (var i = 0; i < listItemComponents.length; i++) {
            if (listItemComponents[i].type == type) {
                return type; // The component user provided a ListItemComponent for this item's type, use it
            }
        }

        /* The type of the item is unknown. Instead of letting the ListView use the
         * StandardListItem we provide our own. */
        return "__carouselitem";
    }
}