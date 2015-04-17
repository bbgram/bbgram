import bb.cascades 1.2
import bbgram.bps.lib 0.1
import bb.multimedia 1.0
import bbgram.types.lib 0.1

Container {
    signal submitted();
    signal valueChanging(string value);    
    property alias value: message.text
    property string audioPath: ""
    property int audioDuration : 0
    
    
    function clear() {
        message.text = ""
        audioPath = ""
        audioDuration = 0
        voiceButton.setCancelMode(false);
    }
    /*layout: StackLayout {
     orientation: LayoutOrientation.TopToBottom
     }*/
    Container {
        id: inputPanel
        layoutProperties: StackLayoutProperties {
            spaceQuota: -1
        }
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        //minHeight: 400
        leftPadding: 10
        rightPadding: 10
        topPadding: 10
        bottomPadding: 10
        horizontalAlignment: HorizontalAlignment.Fill
        //verticalAlignment: VerticalAlignment.Fill
        background: Color.create("#252525")
        ImageButton {
            id: voiceButton
            verticalAlignment: VerticalAlignment.Bottom
            defaultImageSource: "asset:///images/bar_voice1.png"
            pressedImageSource: "asset:///images/bar_voice2.png"
            preferredWidth: 90
            preferredHeight: 90
            
            property bool isCancelMode : false
            
            function setCancelMode(value)
            {
                isCancelMode = value;
                
                if (value)
                {
                    voiceButton.defaultImageSource = "asset:///images/bar_voice_cancel_1.png"
                    voiceButton.pressedImageSource = "asset:///images/bar_voice_cancel_2.png"
                }
                else
                {
                    voiceButton.defaultImageSource = "asset:///images/bar_voice1.png"
                    voiceButton.pressedImageSource = "asset:///images/bar_voice2.png"
                }
            }
            
            onTouch: {
                return;
                if (isCancelMode)
                {
                    setCancelMode(false);
                    message.visible = true
                    recordInfo.visible = false
                    return;
                }
                
                if (event.touchType == TouchType.Down)
                {
                    audioRecorder.record()
                    message.visible = false
                    recordInfo.visible = true
                    recordTimer.start();
                }
                else if (event.touchType == TouchType.Up || event.touchType == TouchType.Cancel)
                {
                    if (audioRecorder.duration > 1000)
                    {
                        setCancelMode(true)
                        audioDuration = audioRecorder.duration / 1000
                    }
                    else
                    {
                        message.visible = true
                        recordInfo.visible = false
                    }
                    
                    audioPath = audioRecorder.outputUrl
                    audioRecorder.reset()
                    recordTimer.stop();
                }
            }
        }
        TextArea {
            id: message
            verticalAlignment: VerticalAlignment.Center
            focusPolicy: FocusPolicy.KeyAndTouch
            //text: "123456 asdasjdh asdk jhasd asd jahsd kasd haskdjha asjdasdasdh 987654321"
            hintText: "Enter a message"
            autoSize.maxLineCount: 5
            onTextChanging: {
                valueChanging(text);
            }
            input {                    
                submitKey: SubmitKey.Send
                flags: emojiPanel.delegateActive ? TextInputFlag.VirtualKeyboardOff : TextInputFlag.VirtualKeyboard
                onSubmitted: {
                    submitted();
                }
            }
        }
        Label {
            id: recordInfo
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Fill
            visible: false
            text: "Recording"
            textStyle {
                base: SystemDefaults.TextStyles.BodyText
                color: Color.Red
            }
        }
        ImageButton {
            verticalAlignment: VerticalAlignment.Bottom
            defaultImageSource: emojiPanel.delegateActive ? "asset:///images/bar_down.png" : "asset:///images/bar_smile.png"
            pressedImageSource: emojiPanel.delegateActive ? "asset:///images/bar_down.png" : "asset:///images/bar_smile.png"
            preferredWidth: 90
            preferredHeight: 90
            
            onClicked: {
                emojiPanel.delegateActive = !emojiPanel.delegateActive;
                if (emojiPanel.delegateActive)
                    virtualKeyboardService.hide()
                else
                    message.requestFocus()
                //emojiPanel.layoutProperties.spaceQuota = emojiPanel.delegateActive ? 1 : -1;
                //inputPanel.layoutProperties.spaceQuota = -emojiPanel.layoutProperties.spaceQuota;
            }
        }
    }
    ControlDelegate {
        id: emojiPanel
        
        sourceComponent: emojiPanelCompDef
        delegateActive: false 
        
        implicitLayoutAnimationsEnabled: false
        onControlChanged: {
            if (control)
                control.textArea = message
        }
    }
    attachedObjects: [
        VirtualKeyboardService {
            id: virtualKeyboardService
            
            onKeyboardVisible: {
                //console.log("Keyboard Visible");
                emojiPanel.delegateActive = false
            }
            onKeyboardHidden: {
                //console.log("Keyboard Hidden");
            }
        },
        AudioRecorder {
            id: audioRecorder
            outputUrl: "file://" + _owner.getDataPath() + "recording.m4a"
        },
        QTimer {
            id: recordTimer
            singleShot: false
            interval: 333
            onTimeout:{
                var sec = Math.ceil(audioRecorder.duration / 1000);
                var msec = (audioRecorder.duration % 1000)
                recordInfo.text = "Recoding " + sec + " : " + msec;
            }
        },
        ComponentDefinition {
            id: emojiPanelCompDef
            Container {
                property variant textArea: null
                property int height: virtualKeyboardService.height()
                maxHeight: height > 140 ? height - 140 : 275
                layout: StackLayout {                    
                }
                
                Container {
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    topPadding: 0
                    bottomPadding: 0
                    leftPadding: 10
                    rightPadding: 10
                    
                    
                    ImageButton {
                        defaultImageSource: "asset:///images/emojis_people.png"
                        scaleX: 0.8
                        scaleY: 0.8
                        onClicked: {
                            listView.dataModel.source = "asset:///dm_emojis_people.xml"
                        }
                    }
                    ImageButton {
                        defaultImageSource: "asset:///images/emojis_nature.png"
                        scaleX: 0.8
                        scaleY: 0.8
                        onClicked: {
                            listView.dataModel.source = "asset:///dm_emojis_nature.xml"
                        }
                    }
                    ImageButton {
                        defaultImageSource: "asset:///images/emojis_objects.png"
                        scaleX: 0.8
                        scaleY: 0.8
                        onClicked: {
                            listView.dataModel.source = "asset:///dm_emojis_objects.xml"
                        }
                    }
                    ImageButton {
                        defaultImageSource: "asset:///images/emojis_places.png"
                        scaleX: 0.8
                        scaleY: 0.8
                        onClicked: {
                            listView.dataModel.source = "asset:///dm_emojis_places.xml"
                        }
                    }
                    ImageButton {
                        defaultImageSource: "asset:///images/emojis_symbols.png"
                        scaleX: 0.8
                        scaleY: 0.8
                        onClicked: {
                            listView.dataModel.source = "asset:///dm_emojis_symbols.xml"
                        }
                    }
                    
                    /*ImageButton {
                     defaultImageSource: "asset:///images/emojis_backspace.png"
                     scaleX: 0.8
                     scaleY: 0.8
                     onClicked: {
                     //??
                     }
                     }*/
                }
                                
                                Container {
                                    background: Color.Black
                                    preferredHeight: 2
                                    horizontalAlignment: HorizontalAlignment.Fill                    
                                }
                                //preferredHeight: 100        
                                ListView {
                                    id: listView
                                    dataModel: XmlDataModel {
                                        source: "asset:///dm_emojis_people.xml"
                                    }
                                    layout: GridListLayout {
                                        columnCount: 8
                                    }
                                    
                                    onTriggered: {
                                        var code = dataModel.data(indexPath).code;
                                        var i1 = parseInt(code.slice(0, 4), 16);
                                        var i2 = parseInt(code.slice(4, 8), 16);
                                        var s = "";
                                        if (i1 != 0)
                                            s = String.fromCharCode(i1, i2);
                                        else
                                            s = String.fromCharCode(i2);
                                        textArea.editor.insertPlainText(s);
                                        textArea.requestFocus();
                                    }
                                    listItemComponents: ListItemComponent {
                                        type: "emoji"
                                        CustomListItem {
                                            dividerVisible: false
                                            Label {
                                                id: label
                                                horizontalAlignment: HorizontalAlignment.Center
                                                verticalAlignment: VerticalAlignment.Center
                                                textStyle {
                                                    fontSize: FontSize.XLarge
                                                }
                                                
                                                function parseChar(code) {
                                                    var i1 = parseInt(code.slice(0, 4), 16);
                                                    var i2 = parseInt(code.slice(4, 8), 16);
                                                    if (i1 != 0)
                                                        return String.fromCharCode(i1, i2)
                                                    else
                                                        return String.fromCharCode(i2)
                                                }
                                                
                                                //String.fromCharCode(0xD83D,0xDE04) + String.fromCharCode(0xf09f9884) + 
                                                text: parseChar(ListItemData.code);                                
                                            }
                                        }
                                    }
                                }
            }
        }
    ]
}