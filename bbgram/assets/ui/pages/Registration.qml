import bb.cascades 1.2
import bbgram.control.lib 0.1
import bbgram.types.lib 0.1
import bb.cascades.pickers 1.0

Page {
    property string photoPath;
    
    function checkValues()
    {
        nextButton.enabled = firstName.text.length > 0 && lastName.text.length > 0;
    }
    
    Container {
        leftPadding: 20
        rightPadding: 20
        topPadding: 20
        bottomPadding: 20
        
        Label {
            horizontalAlignment: HorizontalAlignment.Center
            text: "Your name"
            textStyle{
                fontSize: FontSize.XLarge
            }
        }
        
        Divider {
        
        }
        
        Label {
            horizontalAlignment: HorizontalAlignment.Center
            text: "Enter your name and add a profile picture"
            textStyle{
                fontSize: FontSize.Medium
            }
        }
        
        Container {
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            ImageView {
                id: photoImageView
                verticalAlignment: VerticalAlignment.Center
                
                imageSource: "asset:///images/menu_plus.png"
                scalingMethod: ScalingMethod.AspectFit
                preferredHeight: 180
                preferredWidth: 180
                
                gestureHandlers: TapHandler {
                    onTapped: {
                        filePicker.open()
                    }
                }
                
                attachedObjects: [
                    FilePicker {
                        id: filePicker
                        type : FileType.Picture
                        title : "Select Picture"
                        mode: FilePickerMode.Picker
                        directories : ["/accounts/1000/shared/"]
                        onFileSelected : {
                            photoImageView.imageSource =  "file://" + selectedFiles[0];
                            photoPath = selectedFiles[0];
                        }
                    }
                ]
            } 
            
            Container {
                Container {
                    
                    verticalAlignment: VerticalAlignment.Center
                    leftPadding: 20
                    TextField {
                        id: firstName
                        hintText: "First Name"
                        onTextChanging: {
                            checkValues()
                        }
                    }
                    TextField {
                        id: lastName
                        hintText: "Last Name"
                        onTextChanging: {
                            checkValues()
                        }
                    }
                }
            }
        }
        Container {
            preferredHeight: 50
        }
        Button {
            id: nextButton
            enabled: false
            horizontalAlignment: HorizontalAlignment.Center
            text: "Next"
            
            onClicked: {
                _owner.registerUser(firstName.text,lastName.text, photoPath);
            }
        }
    }
}
