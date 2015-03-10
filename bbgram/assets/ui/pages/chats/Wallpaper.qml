import bb.cascades 1.2

Container {
    property string source: ""
    
    Container {
        background: source.length > 0 && source.charAt(0) == '#' ? Color.create(source) : Color.Transparent
        preferredWidth: Infinity
        preferredHeight: Infinity
        ControlDelegate {
            sourceComponent: imageViewCompDef
            delegateActive: source.length == 0 || source.charAt(0) != '#'
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
                loadEffect: ImageViewLoadEffect.DefaultDeferred
                implicitLayoutAnimationsEnabled: false
                scalingMethod: ScalingMethod.AspectFill
            }
        }
        
    ]
}
