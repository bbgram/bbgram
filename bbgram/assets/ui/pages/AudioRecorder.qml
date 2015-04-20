import bb.cascades 1.2
import bbgram.types.lib 0.1


Container {
    id: me
    property bool active: false
    property alias duration : recordTimer.duration
    
    signal startRecord();
    signal cancelRecord();
    signal stopRecord();
    
    onActiveChanged: {
        container.rightPadding = 90
        cancelHint.opacity = 1
    }
    
    layout: DockLayout {
    }
    
    
    Container {
        id: container
        visible: me.active
        
        layout: DockLayout {
        }
        
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Fill
        leftPadding: 10
        rightPadding: 90
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageView {
                verticalAlignment: VerticalAlignment.Center   
                imageSource: "asset:///images/rec.png"
                scaleX: 0.75
                scaleY: 0.75
            }
            Label {
                function formatTime(t) {
                    var h = Math.floor(t / 3600);
                    var m = Math.floor(t / 60) % 60;
                    m = m >= 10 ? m : "0" + m;
                    var s = t % 60;
                    s = s >= 10 ? s : "0" + s;
                    if (h != 0)
                        return "%1:%2:%3".arg(h).arg(m).arg(s);
                    else
                        return "%1:%2".arg(m).arg(s);
                }
                verticalAlignment: VerticalAlignment.Center
                text: formatTime(recordTimer.duration)
                textStyle {
                    fontSize: FontSize.Large
                    color: Color.create('#da564d')
                }
                leftMargin: 5
                topMargin: 0
                bottomMargin: 0
            }    
        }
        
        Label {
            id : cancelHint
            horizontalAlignment: HorizontalAlignment.Right
            text: "< Slide to cancel"
            textStyle {
                fontSize: FontSize.Large
                color: Color.White
            }
        }
    }
    ImageButton {
        id: voiceButton
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Right        
        defaultImageSource: me.active ? "asset:///images/bar_voice2.png" : "asset:///images/bar_voice1.png"
        pressedImageSource: "asset:///images/bar_voice2.png"
        preferredWidth: 90
        preferredHeight: 90
        
        property int touchX: 0
        
        function cancel() {
            cancelRecord()
            recordTimer.stop();
            me.active = false;
        }
        
        onTouch: {
            if (event.touchType == TouchType.Down) {
                me.active = true;
                startRecord();
                recordTimer.duration = 0;
                recordTimer.start();
                voiceButton.touchX = event.localX
            }
            else if (event.touchType == TouchType.Up || event.touchType == TouchType.Cancel) {
                if (me.active) {
                    stopRecord();
                    recordTimer.stop();
                    me.active = false;
                }
            }
            else if (event.touchType == TouchType.Move && me.active) {
                var dx = voiceButton.touchX - event.localX;
                if (dx > 0) {
                    container.rightPadding = 90 + dx;
                    cancelHint.opacity = (100 - dx) / 100;
                    if (dx > 100)
                        cancel();
                }
                else 
                    container.rightPadding = 90;
            }
        }
    }
    
    attachedObjects: [
        QTimer {
            id: recordTimer
            
            property int duration : 0
            
            singleShot: false
            interval: 1000
            onTimeout:{
                duration += 1;
            }
        }
    ]
}
