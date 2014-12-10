import bb.cascades 1.2

Container {
    topPadding: 15
    bottomMargin: 15
    
    layout: DockLayout {            
    }
    ImageView {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        imageSource: "asset:///images/date.amd"
    }
    Container {            
        leftPadding: 20
        rightPadding: 20
        topPadding: 4
        bottomPadding: 8
        Label {
            text: ListItemData.formatDateTime(ListItemData.dateTime, "MMMM d")
            textStyle.base: TextStyleDefinition.style
        }
    }
    attachedObjects: [
        TextStyleDefinition {
            id: dateHeaderTextStyle
            color: Color.White
        }
    ]
}