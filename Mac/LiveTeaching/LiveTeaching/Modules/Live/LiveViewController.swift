//
//  LiveViewController.swift
//  LiveTeaching
//
//  Created by vector on 2020/1/25.
//  Copyright © 2020 vector. All rights reserved.
//


class LiveViewController: ViewController, UIScrollViewDelegate {
    
    
//    public let drawingBoard: DrawingBoard

    

    public let containerController: DrawingContainerController = {
        let scale = UIScreen.main.scale
        let screenSize: CGSize = UIScreen.main.size
        let min = screenSize.width < screenSize.height ? screenSize.width : screenSize.height
        let drawingSize: DrawingSize
        if min * scale + 0.4 > 1080 {
            drawingSize = .preset1440x1080
        } else {
            drawingSize = .preset960x720
        }
        
        let controller = DrawingContainerController(frame: CGRect(), drawingSize:drawingSize, contentHeightLimit: drawingSize.rawValue.width * 16, bitmapLayout: Drawing.BitmapLayout(width: drawingSize.rawValue.width, colorSpace: Drawing.ColorSpace.little16Xrgb, backgroundColor: Drawing.Color(little32Argb: 0)))
        var status = DrawingContext.Status()
        status.offset = 0
        status.contentHeight = drawingSize.rawValue.width * 16
        controller.context.updateStatus(status)
        return controller
    } ()

    override func viewDidLoad() {
        super.viewDidLoad()
//        self.view.addSubview(self.drawingController.content)
        self.view.addSubview(self.containerController.content)
        self.resetContainerControllerLayout()
        
        let scrollView = UIScrollView(frame: CGRect(x: 0, y: 0, width: 30, height: 400))
        scrollView.contentSize = CGSize(width: 0, height: 3200)
        self.view.addSubview(scrollView)
        
        scrollView.backgroundColor = UIColor.red
        
        scrollView.delegate = self
        self.contentView.isHidden = true
    }
    
    override func viewDidLayoutSubviews() {
        super.viewDidLayoutSubviews()
        self.resetContainerControllerLayout()
    }
    
    private func resetContainerControllerLayout() {
        let bounds = self.view.bounds
        var frame = bounds
        frame.size.width = bounds.size.height * 4 / 3
        frame.origin.x = bounds.size.width - frame.size.width
        frame.origin.y = 0
        self.containerController.frame = frame
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
