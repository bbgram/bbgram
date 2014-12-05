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
                enabled: false
                
                function contactAdded(error, message){
                    firstName.enabled = true;
                    lastName.enabled = true;
                    phoneNumber.enabled = true;
                    
                    if (error)
                        errorLabel.text = message
                    else
                        me.close()
                    
                    _owner.contactAdded.disconect(contactAdded)
                }
                
                onTriggered: {
                    _owner.contactAdded.connect(contactAdded)
                    _owner.addContact(firstName.text, lastName.text, phoneNumber.text)
                    
                    firstName.enabled = false
                    lastName.enabled = false
                    phoneNumber.enabled = false
                }
            }
            dismissAction: ActionItem {
                id: cancelButton
                title: "Cancel"
                onTriggered:{
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
                
                Container
                {
                    Label {
                        id: errorLabel
                        text: ""
                    }
                }
            }
        }
    }
}