import bb.cascades 1.2

StandardListItem {
    property bool fromPhone: ListItemData.sortingKey != "!"
    image: if (!fromPhone) ListItemData.photo
    imageSource: if (fromPhone) "asset:///images/placeholders/user_placeholder_grayscale.png"
    title: ListItemData.firstName + " " + ListItemData.lastName
    //title: "firstName lastName"
    description: ListItemData.sortingKey != "!" ? "" : (ListItemData.online ? "online" : "last seen " + ListItemData.lastSeenFormatted)
    //description: "last seen "
}