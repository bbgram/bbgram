import bb.cascades 1.2

Container {
    id: me
    property string source: _owner.wallpaper
    
    Container {
        background: source.length > 0 && source.charAt(0) == '#' ? Color.create(source) : Color.Transparent
        preferredWidth: Infinity
        preferredHeight: Infinity
        ControlDelegate {
            sourceComponent: imageViewCompDef
            delegateActive: me.source.length == 0 || me.source.charAt(0) != '#'
            implicitLayoutAnimationsEnabled: false
        }
    }
    attachedObjects: [
        ComponentDefinition {
            id: imageViewCompDef
            ImageView {
                id: imageView
                preferredWidth: Infinity
                preferredHeight: Infinity
                imageSource: source.length == 0 ? "asset:///images/background_hd.jpg" : source
                loadEffect: ImageViewLoadEffect.None
                implicitLayoutAnimationsEnabled: false
                scalingMethod: ScalingMethod.AspectFill
            }
        }
        
    ]
}
