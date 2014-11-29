import bb.cascades 1.2
import bbgram.control.lib 0.1

Page {
    property variant pageStack: []
    
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
            text: "<html><p style='text-align:center'>We will call you in 0:23</p></html>"
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
