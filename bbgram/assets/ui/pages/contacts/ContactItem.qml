import bb.cascades 1.2

StandardListItem {
    image: ListItemData.photo
    title: ListItemData.firstName + " " + ListItemData.lastName
    //title: "firstName lastName"
    description: ListItemData.sortingKey != "!" ? "" : (ListItemData.online ? "online" : "last seen " + ListItemData.lastSeenFormatted)
    //description: "last seen "
}