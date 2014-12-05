import bb.cascades 1.2

Label {
    text: ListItemData.text + (incoming ? "<b>            &nbsp;</b>" : "<b>                 &nbsp;</b>")
    multiline: true
    textFormat: TextFormat.Html
}