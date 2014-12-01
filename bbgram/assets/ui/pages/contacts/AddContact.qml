import bb.cascades 1.2

Sheet {
    id: me
    
    property bool isProcessing : false
    
    Page {
        titleBar: TitleBar {
            title: "Add Contact"
            acceptAction: ActionItem {
                title: "Add"
                enabled: firstName.text.length > 0 && lastName.text.length > 0 && phoneNumber.text.length > 0 && !isProcessing
                
                function contactAdded(error, message){
                    isProcessing = false
                    
                    if (error)
                    {
                        errorLabel.text = message
                    }    
                    else
                        me.close()
                }
                
                onTriggered: {
                    //me.done(user, firstName.text, lastName.text)
                    //me.close()
                    _contactManager.onContactAdded.connect(contactAdded)
                    _contactManager.addContact(firstName.text, lastName.text, phoneNumber.text)
                    isProcessing = true
                }
            }
            dismissAction: ActionItem {
                title: "Cancel"
                enabled: !isProcessing
                onTriggered: {
                    me.close()
                }
            }
        }
        Container{
            leftPadding: 20
            rightPadding: 20
            topPadding: 20
            bottomPadding: 20
            
            Container {
                TextField {
                    id: firstName
                    //text: user ? user.firstName : ""
                    text: ""
                    hintText: "First Name"
                }
                TextField {
                    id: lastName
                    text: ""
                    //text: user ? user.lastName : ""
                    hintText: "Last Name"
                }
                TextField {
                    id: phoneNumber
                    text: ""
                    //text: user ? user.lastName : ""
                    hintText: "Phone Number"
                }
                
                Label {
                    id: errorLabel
                    text: ""
                }
            }
        }
    }
}