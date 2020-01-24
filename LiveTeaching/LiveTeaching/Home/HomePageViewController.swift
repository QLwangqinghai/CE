//
//  HomePageViewController.swift
//  LiveTeaching
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 vector. All rights reserved.
//

import UIFoundation
import DrawingBoard


//import UIKit.UIGestureRecognizerSubclass
//
//public final class GestureRecognizer: UIPanGestureRecognizer {
//
//
//    public func description(of touch: UITouch) -> String {
//        return "<UITouch: \(touch.addressDescription)> timestamp:\(touch.timestamp) phase:\(touch.phase) tapCount:\(touch.tapCount) majorRadius:\(touch.majorRadius) majorRadiusTolerance:\(touch.majorRadiusTolerance) locationInView:\(touch.location(in: self.view))"
//    }
//    public func description(of event: UIEvent) -> String {
//        return "<UIEvent: \(event.addressDescription)> timestamp:\(event.timestamp)"
//    }
//
//
//    public func log(tag: String, touches: Set<UITouch>, event: UIEvent) {
//        let touches = Array<UITouch>(touches).map{ (touch) -> String in
//            return self.description(of: touch)
//        }
//        print("\(tag): touches(\(touches.count):\(touches)) event:\(self.description(of: event))")
//    }
//
//    public override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent) {
//        super.touchesBegan(touches, with: event)
//        self.log(tag: "touchesBegan", touches: touches, event: event)
//    }
//
//    public override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent) {
//        super.touchesMoved(touches, with: event)
//        self.log(tag: "touchesMoved", touches: touches, event: event)
//    }
//
//    public override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent) {
//        super.touchesCancelled(touches, with: event)
//        self.log(tag: "touchesCancelled", touches: touches, event: event)
//    }
//
//    public override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent) {
//        super.touchesEnded(touches, with: event)
//        self.log(tag: "touchesEnded", touches: touches, event: event)
//    }
//
//}
//
//public class DView: UIView {
//
//    public func description(of touch: UITouch) -> String {
//        return "<UITouch: \(touch.addressDescription)> timestamp:\(touch.timestamp) phase:\(touch.phase) tapCount:\(touch.tapCount) majorRadius:\(touch.majorRadius) majorRadiusTolerance:\(touch.majorRadiusTolerance) locationInView:\(touch.location(in: self))"
//    }
//    public func description(of event: UIEvent?) -> String {
//        if let event = event {
//            return "<UIEvent: \(event.addressDescription)> timestamp:\(event.timestamp)"
//        } else {
//            return "<UIEvent: nil>"
//        }
//    }
//
//
//    public func log(_ function: String = #function, tag: String, touches: Set<UITouch>, event: UIEvent?) {
//        let touches = Array<UITouch>(touches).map{ (touch) -> String in
//            return self.description(of: touch)
//        }
//        print("\(function) - \(tag): touches(\(touches.count):\(touches)) event:\(self.description(of: event))")
//    }
//
//    public override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
//        super.touchesBegan(touches, with: event)
//        self.log(tag: "touchesBegan", touches: touches, event: event)
//    }
//
//    public override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
//        super.touchesMoved(touches, with: event)
//        self.log(tag: "touchesMoved", touches: touches, event: event)
//    }
//
//    public override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
//        super.touchesCancelled(touches, with: event)
//        self.log(tag: "touchesCancelled", touches: touches, event: event)
//    }
//
//    public override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
//        super.touchesEnded(touches, with: event)
//        self.log(tag: "touchesEnded", touches: touches, event: event)
//    }
//
//
//}



class HomePageViewController: BaseViewController {
    public let panGestureRecognizer: GestureRecognizer = GestureRecognizer()

//    public let drawingController: DrawingController = {
//        let bounds: Rect
//        
//        BitmapLayout.init(boxSize: <#T##DrawingContext.BoxSize#>, colorSpace: <#T##DrawingContext.ColorSpace#>, backgroundColor: <#T##DrawingContext.Color#>)
//        let scale = UIScreen.main.scale
//        let screenSize: CGSize = UIScreen.main.size
//        let min = screenSize.width < screenSize.height ? screenSize.width : screenSize.height
//        if min * scale + 0.4 > 1080 {
//            //good
//            case preset960x720
//            case preset1280x720
//            case preset1280x1080
//            
//            //good
//            case preset1440x1080
//            
//        } else {
//        
//        }
//        
//        
//        let controller = DrawingController.init(identifier: "1", bounds: Rect, offset: <#T##Int32#>, bitmapLayout: BitmapLayout, scale: <#T##CGFloat#>, dataSource: <#T##DrawingBoardDataSource#>)
//        
//        
//    } ()
//        DrawingController(identifier: "1", bounds: <#Rect#>)
//    
//    override func viewDidLoad() {
//        super.viewDidLoad()
//        self.view.addSubview(self.drawingController.view)
//
//    }


    @objc private func handlePan(recognizer: GestureRecognizer) {
        guard recognizer == self.panGestureRecognizer else {
            return
        }
        print("recognizer.state \(recognizer.state.rawValue)")
    }
}

