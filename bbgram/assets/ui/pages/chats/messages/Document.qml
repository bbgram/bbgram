import bb.cascades 1.2

Container {
    property bool incoming: true
    property string caption: "Document"
    property string fileSize: "0 KB"
    
    layout: StackLayout {
        orientation: LayoutOrientation.LeftToRight
    }
    maxHeight: 125
    Container {
        verticalAlignment: VerticalAlignment.Center
        layout: DockLayout {        
        }
        background: incoming ? Color.create("#e8f2f9") : Color.create("#d4eac0") 
        preferredWidth: 125
        preferredHeight: 125
        ImageView {
            imageSource: incoming ? "asset:///images/doc_blue.png" : "asset:///images/doc_green.png" 
            scaleX: 1.25
            scaleY: 1.25
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
        }    
    }
    
    Container {
        layout: StackLayout {
        }
        leftPadding: 20
        Label {
            text: caption
            bottomMargin: 0  
        }
        Label {            
            text: fileSize
            textStyle.base: incoming ? inSizeTextStyle.style : outSizeTextStyle.style
            topMargin: 0
        }    
    }
    attachedObjects: [
        TextStyleDefinition {
            id: sizeTextStyle
            fontStyle: FontStyle.Italic
            fontSize: FontSize.XSmall
        },
        TextStyleDefinition {
            id: inSizeTextStyle
            base: sizeTextStyle.style
            color: Color.create('#97a1a8')
        },
        TextStyleDefinition {
            id: outSizeTextStyle
            base: sizeTextStyle.style
            color: Color.create('#75b166')
        }
    ]
}