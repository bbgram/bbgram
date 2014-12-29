import bb.cascades 1.2
import bbgram.types.lib 0.1

Sheet {
    id: me
    
    property User user
    property string caption
    
    signal done(variant user, string firstName, string lastName)
    
    Page {
        titleBar: TitleBar {
            title: caption
            acceptAction: ActionItem {
                title: "Done"
                enabled: firstName.text.length > 0 && lastName.text.length > 0
                onTriggered: {
                    me.done(user, firstName.text, lastName.text)
                    me.close()
                }
            }
            dismissAction: ActionItem {
                title: "Cancel"
                onTriggered: {
                    me.close()
                }
            }
        }
        Container {
            leftPadding: 20
            rightPadding: 20
            topPadding: 20
            bottomPadding: 20
            
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                ImageView {
                    verticalAlignment: VerticalAlignment.Center
                    
                    imageSource: user ? user.photo : ""
                    //imageSource: "asset:///images/placeholders/user_placeholder_purple.png"
                    scalingMethod: ScalingMethod.AspectFit
                    preferredHeight: 180
                    preferredWidth: 180
                } 
                
                Container {
                    leftPadding: 20
                    TextField {
                        id: firstName
                        text: user ? user.firstName : ""
                        hintText: ""
                    }
                    TextField {
                        id: lastName
                        text: user ? user.lastName : ""
                        hintText: ""
                    }
                }
            }   
        }        
    }
}