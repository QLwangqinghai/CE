//
//  CollectionView.swift
//  CollectionView
//
//  Created by vector on 2019/11/5.
//  Copyright © 2019 angfung. All rights reserved.
//

import UIKit


//public class Layout {
//    public var isEnabled: Bool = true
//    public internal(set) var frame: CGRect
//    init(frame: CGRect) {
//        self.frame = frame
//    }
//}
//public final class Section: Layout {
//    internal var cells: [CellLayout] = []
//    internal var view: UIStackView
//    override init(frame: CGRect) {
//        self.view = UIStackView(frame: frame)
//        super.init(frame: frame)
//    }
//}
//public final class HeaderLayout: Layout {
//
//}
//public final class FooterLayout: Layout {
//
//}
//public final class CellLayout: Layout {
//    override public var frame: CGRect {
//        didSet {
//            print(frame)
//        }
//    }
//}
//
//
//open class CollectionView: UIScrollView {
//
//    public private(set) var sections: [Section] = []
//
//
//}


public class Layout: UICollectionViewFlowLayout {
    public var isEnabled: Bool = true
    public internal(set) var frame: CGRect
    init(frame: CGRect) {
        self.frame = frame
    }
}
