import bb.cascades 1.2

Container {
    id: me
    property alias title: dropDown.title
    property alias options: dropDown.options
    property alias selectedIndex: dropDown.selectedIndex
    
    layout: DockLayout {}
    Container {
        verticalAlignment: VerticalAlignment.Center
        horizontalAlignment: HorizontalAlignment.Fill
        preferredHeight: 110
        layout: DockLayout { }
        topPadding: 20
        leftPadding: 20
        rightPadding: 20
        bottomPadding: 20
        
        DropDown {
            id: dropDown
            title: "Title"
            /*options: [
                Option {
                    text: "Defdault"
                }
            ]
            selectedIndex: 0*/
        }
    }
    Divider { verticalAlignment: VerticalAlignment.Bottom }
}