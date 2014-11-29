import bb.cascades 1.2
import bbgram.control.lib 0.1

Page {
    id: loginPage
    
    titleBar: TitleBar {
        title: "Your phone"
    }
    
    Container {
        leftPadding: 50
        rightPadding: 50
        topPadding: 50
        
        PhoneNumberInput {
            id: phoneNumber
        }
    
        Label {
            id: hint
            text: "<html><p style='text-align:center'>Please confirm your country code and enter your phone number.</p></html>"
            multiline: true
        }
        
        Button {
            horizontalAlignment: HorizontalAlignment.Center
            text: "Next"
            onClicked: {
                _owner.requestCode('+79084839697')
                var page = yourCodePageDef.createObject()
                var navigationPane = Application.scene
                navigationPane.backButtonsVisible = false
                navigationPane.push(page)
            }
        }
    }
    
    attachedObjects: [                  
        ComponentDefinition {                      
            id: yourCodePageDef                       
            source: "YourCode.qml"             
        }
    ]
}
