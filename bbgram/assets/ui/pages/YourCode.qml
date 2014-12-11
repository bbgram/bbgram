import bb.cascades 1.2
import bbgram.control.lib 0.1
import bbgram.types.lib 0.1

Page {
    property variant pageStack: []
    
    property int time: 120
    
    attachedObjects: [
        QTimer {
            id: timer
            interval: 1000
            onTimeout: {
                time -= 1
                if (time == 0) {
                    _owner.requestPhoneCall();
                    timer.stop()
                }   
            }
        }
    ]
    onCreationCompleted: {
        timer.start();
    }
    
    titleBar: TitleBar {
        title: "Your code"
    }
    
    
    Container {
        layout: StackLayout {}
        leftPadding: 50
        rightPadding: 50
        topPadding: 50
        bottomPadding: 50
        
        Label {
            id: hint
            text: "<html><p style='text-align:center'>We have sent an SMS with an activation code to your phone <b>" + _owner.phone + "</b></p></html>"
            multiline: true
            horizontalAlignment: HorizontalAlignment.Center
        }
        
        TextField {
            id: code
            topMargin: 60
            hintText: "Code"            
            inputMode: TextFieldInputMode.NumbersAndPunctuation
            preferredWidth: 400
            horizontalAlignment: HorizontalAlignment.Center
        }
        
        Label {
            topMargin: 30
            text: {
                var s = ""
                if (time == 0)
                    s = "Calling you..."
                else {
                    var m = Math.floor(time / 60)
                    var s = time % 60
                    s = "We will call you in %1:%2".arg(m).arg(s < 10 ? "0" + s : s)
                }
                "<html><p style='text-align:center'>" + s + "</p></html>"   
            }
            multiline: true
            horizontalAlignment: HorizontalAlignment.Center
        }
        
        
        Button {
            horizontalAlignment: HorizontalAlignment.Center
            text: "Next"
            onClicked: {
                _owner.submitCode(code.text)
            }
        }
    }


}
