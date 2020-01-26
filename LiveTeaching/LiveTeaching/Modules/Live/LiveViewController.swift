//
//  LiveViewController.swift
//  LiveTeaching
//
//  Created by vector on 2020/1/25.
//  Copyright © 2020 vector. All rights reserved.
//

import DrawingBoard

class LiveViewController: ViewController, UIScrollViewDelegate {
    
    
//    public let drawingBoard: DrawingBoard

    public let sectionController: DrawingSectionController = {
        let scale = UIScreen.main.scale
        let screenSize: CGSize = UIScreen.main.size
        let min = screenSize.width < screenSize.height ? screenSize.width : screenSize.height
        let boxSize: DrawingContext.BoxSize
        if min * scale + 0.4 > 1080 {
            boxSize = .preset960x720
        } else {
            boxSize = .preset1440x1080
        }
        
        let bitmapLayout = BitmapLayout(boxSize: boxSize, colorSpace: DrawingContext.ColorSpace.little32Argb, backgroundColor: DrawingContext.Color.clear)

        let contentWidth: CCUInt32 = boxSize.size.width
        let contentSize: Size = Size(width: contentWidth, height: contentWidth * 8)
        
        let frame: CGRect = CGRect(x: 0, y: 0, width: min * CGFloat(contentWidth) / CGFloat(boxSize.size.height), height: min)
        let zoomScale: CGFloat = CGFloat(boxSize.size.height) / min
        let sectionIdentifier = "12345"
        let child = DrawingController(ownerIdentifier: sectionIdentifier, domain: "0", contentSize: contentSize, offset: 0, bitmapLayout: bitmapLayout, zoomScale: zoomScale, backgroundDataSource: DrawingBoardProvider(bitmapLayout: bitmapLayout), contentDataSource: DrawingBoardProvider(bitmapLayout: bitmapLayout))
        
        let controller = DrawingSectionController(identifier: "12345", childs: [child], currentIndex: 0, frame: frame)
        return controller
    } ()
    
    override func viewDidLoad() {
        super.viewDidLoad()
//        self.view.addSubview(self.drawingController.content)
        
        self.sectionController.frame.origin.x = self.view.bounds.size.width - self.sectionController.frame.size.width
        self.view.addSubview(self.sectionController.content)
        
        let scrollView = UIScrollView(frame: CGRect(x: 0, y: 0, width: 30, height: 400))
        scrollView.contentSize = CGSize(width: 0, height: 3200)
        self.view.addSubview(scrollView)
        
        scrollView.backgroundColor = UIColor.red
        
        scrollView.delegate = self
        self.contentView.isHidden = true
    }
    
    override func viewWillLayoutSubviews() {
        super.viewWillLayoutSubviews()
        self.sectionController.frame.origin.x = self.view.bounds.size.width - self.sectionController.frame.size.width
    }
    
        
    func scrollViewDidScroll(_ scrollView: UIScrollView) {
        print(scrollView.contentOffset)
        
        print(self.view.layer)
        
//        self.drawingController.pageContext.updateOffset(Int32(scrollView.contentOffset.y * self.drawingController.pageContext.zoomScale))
    }

    
    
    
    
    
    override var shouldAutorotate: Bool {
        return false
    }
    override var prefersStatusBarHidden: Bool {
        return true
    }
    override var prefersHomeIndicatorAutoHidden: Bool {
        return true
    }
    
    override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
        return .landscapeRight
    }
    override var preferredInterfaceOrientationForPresentation: UIInterfaceOrientation {
        return .landscapeRight
    }
}
