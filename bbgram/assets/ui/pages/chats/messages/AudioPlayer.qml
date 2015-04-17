import bb.cascades 1.2
import bbgram.control.lib 0.1

Container {
    property bool incoming: true
    property string caption: "Audio Message"
    property int duration: 0
    property AudioPlayer player;
    
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
        ImageButton {
            defaultImageSource: incoming ? "asset:///images/play_blue_1.png" : "asset:///images/play_green_1.png"
            pressedImageSource: incoming ? "asset:///images/play_blue_2.png" : "asset:///images/play_green_2.png"
            
            scaleX: 1.25
            scaleY: 1.25
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            
            onClicked: {
                player.play();
            }
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
            text: "0:0 / " + duration + ":0"
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

