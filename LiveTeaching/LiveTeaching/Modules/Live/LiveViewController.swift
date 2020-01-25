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

    
//    public let drawingNavigationController: DrawingNavigationController = {
//       let bitmapLayout: BitmapLayout
//       let scale = UIScreen.main.scale
//       let screenSize: CGSize = UIScreen.main.size
//       let min = screenSize.width < screenSize.height ? screenSize.width : screenSize.height
//       if min * scale + 0.4 > 1080 {
//           bitmapLayout = BitmapLayout(boxSize: .preset960x720, colorSpace: DrawingContext.ColorSpace.little32Argb, backgroundColor: DrawingContext.Color.clear)
//       } else {
//           bitmapLayout = BitmapLayout(boxSize: .preset1440x1080, colorSpace: DrawingContext.ColorSpace.little32Argb, backgroundColor: DrawingContext.Color.clear)
//       }
//       let size: Size = Size(width: CCUInt32(bitmapLayout.countPerRow), height: CCUInt32(bitmapLayout.countPerRow) * 8)
//        let controller = DrawingNavigationController(frame: <#T##CGRect#>)
//        (identifier: "1", contentSize: size, offset: 0, bitmapLayout: bitmapLayout, zoomScale: CGFloat(bitmapLayout.countPerRow) / min, dataSource: DrawingBoardProvider(bitmapLayout: bitmapLayout))
//       return controller
//   } ()
//
//    public let drawingController: DrawingController = {
//        let bitmapLayout: BitmapLayout
//        let scale = UIScreen.main.scale
//        let screenSize: CGSize = UIScreen.main.size
//        let min = screenSize.width < screenSize.height ? screenSize.width : screenSize.height
//        if min * scale + 0.4 > 1080 {
//            bitmapLayout = BitmapLayout(boxSize: .preset960x720, colorSpace: DrawingContext.ColorSpace.little32Argb, backgroundColor: DrawingContext.Color.clear)
//        } else {
//            bitmapLayout = BitmapLayout(boxSize: .preset1440x1080, colorSpace: DrawingContext.ColorSpace.little32Argb, backgroundColor: DrawingContext.Color.clear)
//        }
//        let size: Size = Size(width: CCUInt32(bitmapLayout.countPerRow), height: CCUInt32(bitmapLayout.countPerRow) * 8)
//        let controller = DrawingController(identifier: "1", contentSize: size, offset: 0, bitmapLayout: bitmapLayout, zoomScale: CGFloat(bitmapLayout.countPerRow) / min, dataSource: DrawingBoardProvider(bitmapLayout: bitmapLayout))
//        return controller
//    } ()
    
    override func viewDidLoad() {
        super.viewDidLoad()
//        self.view.addSubview(self.drawingController.content)
        
        
        
        let scrollView = UIScrollView(frame: CGRect(x: 0, y: 0, width: 30, height: 400))
        scrollView.contentSize = CGSize(width: 0, height: 3200)
        self.view.addSubview(scrollView)
        
        scrollView.backgroundColor = UIColor.red
        
        scrollView.delegate = self
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
    
    
//    - (BOOL)shouldAutorotate {
//        //是否支持转屏
//        return true;
//    }
//
//    - (BOOL)prefersHomeIndicatorAutoHidden {
//        return NO;
//    }
//
//    - (BOOL)prefersStatusBarHidden {
//        return NO;
//    }
    override var supportedInterfaceOrientations: UIInterfaceOrientationMask {
        return .landscapeRight
    }
    override var preferredInterfaceOrientationForPresentation: UIInterfaceOrientation {
        return .landscapeRight
    }
}
