import bb.cascades 1.2
import bbgram.control.lib 0.1

Container {
    property int duration: 0
    property AudioPlayer player;
    
    Button {
        text: "Play"
        onClicked: {
            player.play();
        }
    }
}
