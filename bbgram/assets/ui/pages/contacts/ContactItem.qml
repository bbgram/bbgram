import bb.cascades 1.2

StandardListItem {
    image: ListItemData.photo
    //imageSource: "asset:///images/placeholders/user_placeholder_purple.png"
    title: ListItemData.firstName + " " + ListItemData.lastName
    //title: "firstName lastName"
    description: ListItemData.online ? "online" : "last seen " + ListItemData.lastSeenFormatted
    //description: "last seen "
}