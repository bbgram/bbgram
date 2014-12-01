import bb.cascades 1.2

Sheet {
    id: me
    
    function checkFields(){
        addButton.enabled = firstName.text.length > 0 && lastName.text.length > 0 && phoneNumber.text.length > 0
    }
    
    Page {
        titleBar: TitleBar {
            title: "Add Contact"
            acceptAction: ActionItem {
                id: addButton
                title: "Add"
                enabled: firstName.text.length > 0 && lastName.text.length > 0 && phoneNumber.text.length > 0
                
                function contactAdded(error, message){
                    
                    if (error){
                        errorLabel.text = message
                        cancelButton.enabled = true
                    }    
                    else
                        me.close()
                }
                
                onTriggered: {
                    _contactManager.onContactAdded.connect(contactAdded)
                    _contactManager.addContact(firstName.text, lastName.text, phoneNumber.text)
                    
                    addButton.enabled = false
                    cancelButton.enabled = false
                }
            }
            dismissAction: ActionItem {
                id: cancelButton
                title: "Cancel"
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
                    text: ""
                    hintText: "First Name"
                    onTextChanging:{
                        me.checkFields()
                    }
                }
                TextField {
                    id: lastName
                    text: ""
                    hintText: "Last Name"
                    onTextChanging:{
                        me.checkFields()
                    }
                }
                TextField {
                    id: phoneNumber
                    text: ""
                    hintText: "Phone Number"
                    onTextChanging:{
                        me.checkFields()
                    }
                }
                
                Label {
                    id: errorLabel
                    text: ""
                }
            }
        }
    }
}