import bb.cascades 1.2
import bb.data 1.0

Page {
    titleBar: TitleBar {
        id: titleBar
        title: "Choose a country"
        kind: TitleBarKind.FreeForm

        kindProperties: FreeFormTitleBarKindProperties {
            Container {
                layout: DockLayout {
                }
                leftPadding: 20
                rightPadding: 20
                Label {
                    id: label
                    text: "Choose a country"
                    textStyle.base: titleTextStyle.style
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
            }
            expandableArea {
                content:  Container {
                    leftPadding: 20
                    rightPadding: 20
                    topPadding: 20
                    bottomPadding: 20
                    TextField {
                        id: filter
                        hintText: ""
                        textStyle {
                            fontSize: FontSize.Medium
                        }
                        onTextChanging: {
                            _phoneNumberInput.setFilter(text);
                        }
                    }
                }
                onExpandedChanged: {
                    if (expanded) {
                        filter.text = ""
                        filter.requestFocus()
                    }
                    else
                        _phoneNumberInput.setFilter("");
                }
            }
        }
    }
    
    Container {
        leftPadding: 20
        rightPadding: 20
        topPadding:  20
        bottomPadding: 20

        ListView {
            id: countries
            preferredWidth: Infinity
            preferredHeight: Infinity
            dataModel: _phoneNumberInput.dataModel
            
            function itemType(data, indexPath) {
                if (indexPath.length == 1)
                    return "header"
                else
                    return "country";
            }
            
            listItemComponents: [
                ListItemComponent {
                    type: "header"
                    
                    Header {
                        title: ListItemData
                    }
                },
                ListItemComponent {
                    type: "country"
                    
                    StandardListItem {
                        title: ListItemData.name
                        status: "+" + ListItemData.code
                    }
                }
            ]
            
            onTriggered: {
                var selectedItem = _phoneNumberInput.dataModel.data(indexPath);
                _phoneNumberInput.setCountry(selectedItem.name, selectedItem.code)
                _phoneNumberInput.setFilter("");
            }
        }
    }
    
    attachedObjects: [
        TitleTextStyleDefinition {
            id: titleTextStyle
            fontSize: FontSize.Large
        }
    ]
}
