

import UIKit

public final class Section {
    public var isEnabled: Bool = true
    public private(set) var frame: CGRect
    
    internal var cells: [Cell] = []
    init(frame: CGRect) {
        self.frame = frame
    }
    
}
public final class Header {
    
}
public final class Footer {
    
}
public final class Cell {
    
}


open class CollectionView: UIScrollView {

}

