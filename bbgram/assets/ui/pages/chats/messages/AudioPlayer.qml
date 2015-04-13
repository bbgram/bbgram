import bb.cascades 1.2
import bb.multimedia 1.2

Container {
    property int duration: 0
    property string audioFileName: ""
    
    Button {
        text: "Play"
        onClicked: {
            console.log(audioFileName)
            audioPlayer.sourceUrl = audioFileName;
            audioPlayer.play()
        }
    }
    
    attachedObjects:[
        MediaPlayer {
            id: audioPlayer
            onError: {
                console.log(mediaError)
            }
        }
    ]
}
