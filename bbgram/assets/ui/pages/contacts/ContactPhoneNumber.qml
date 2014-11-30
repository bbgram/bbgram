import bb.cascades 1.2

Container {
    property string phone: "+7 777 7777777"
    property string type: "Mobile"
    
    Container {
        leftPadding: 20
        
        Container {
            Label {
                text: phone
                textStyle {
                    fontSize: FontSize.Medium
                }
            }    
        }
        
        Container {
            Label {  
                text: type
                textStyle {
                    color: Color.Gray
                    fontSize: FontSize.Small
                }
            }
        }
    }
    Container {
        background: Color.White
        
        Divider {
        }
    }
}


