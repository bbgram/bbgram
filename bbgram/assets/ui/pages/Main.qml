import bb.cascades 1.2

TabbedPane {
    id: tabbedPane
    showTabsOnActionBar: true
    activeTab: chatsTab


    function openChat(chat) {
        activeTab = chatsTab
        var page = chatPageDef.createObject()
        page.dialog = chat
        activeTab.content.push(page)
    }
    
    Tab {
        id: contactsTab
        imageSource: "asset:///images/bar_profile.png"
        title: "Contacts"
    }
    Tab {
        id: chatsTab
        imageSource: "asset:///images/bar_chats.png"
        title: "Chats"
    }
    Tab {
        id: settingsTab
        imageSource: "asset:///images/bar_menu_settings.png"
        title: "Settings"
    }
    
    onActiveTabChanged: {
        if (activeTab.content === null) {
            var def = null
            switch (activeTab) {
                case contactsTab:
                    def = contactsPageDef
                    break
                case chatsTab:
                    def = chatsPageDef
                    break
                case settingsTab:
                    def = settingsPageDef
                    break
            }
            if (def !== null)
                activeTab.content = def.createObject()
        }
    }
    
    attachedObjects: [
        ComponentDefinition {                      
            id: contactsPageDef                       
            source: "Contacts.qml"             
        },  
        ComponentDefinition {                      
            id: chatsPageDef                       
            source: "Chats.qml"             
        },         
        ComponentDefinition {                      
            id: settingsPageDef                       
            source: "Settings.qml"             
        },
        ComponentDefinition {
            id: chatPageDef
            source: "Dialog.qml"
        }
    ]
}