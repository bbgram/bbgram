import bb.cascades 1.2

NavigationPane {
    id: navigationPane
    
    function openChat(peer) {
        var page = chatPageDef.createObject()
        page.peer = peer
        
        getNavigationPane().push(page)
    }
    
    function getNavigationPane()
    {
        return navigationPane;
    }
    
    attachedObjects: [
        ComponentDefinition {                      
            id: chatsPageDef
            objectName: "chatsPageDef"           
            source: "Chats.qml"
        }
    ]
}
