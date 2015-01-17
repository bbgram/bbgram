import bb.cascades 1.2

Label {
    text: ListItemData.text + (ListItemData.our ? "<b>                    &nbsp;</b>" : "<b>              &nbsp;</b>")
    multiline: true
    textFormat: TextFormat.Html
    textStyle.color: Color.Black
}