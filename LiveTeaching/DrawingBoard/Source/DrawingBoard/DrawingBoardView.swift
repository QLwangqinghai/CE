//
//  DrawingBoardView.swift
//  DrawingBoard
//
//  Created by vector on 2020/1/23.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

extension Log {
    public static func description(of touch: UITouch, view: UIView?) -> String {
        return "<UITouch: \(touch.addressDescription)> timestamp:\(touch.timestamp) phase:\(touch.phase) tapCount:\(touch.tapCount) majorRadius:\(touch.majorRadius) majorRadiusTolerance:\(touch.majorRadiusTolerance) locationInView:\(touch.location(in: view))"
    }
    public static func description(of event: UIEvent?) -> String {
        if let event = event {
            return "<UIEvent: \(event.addressDescription)> timestamp:\(event.timestamp)"
        } else {
            return "<UIEvent: nil>"
        }
    }
    public static func logEvent(_ function: String = #function, view: UIView?, touches: Set<UITouch>, event: UIEvent?) {
        let touches = Array<UITouch>(touches).map{ (touch) -> String in
            return self.description(of: touch, view: view)
        }
        print("\(function) - touches(\(touches.count):\(touches)) event:\(self.description(of: event))")
    }
}


public class TapEventHandler: NSObject {
    open var isEnabled: Bool = false

    override init() {
        super.init()
    }
}
public class PanEventHandler: NSObject {
    open var isEnabled: Bool = false

    override init() {
        super.init()
    }
}
public final class DrawingEventRecognizer: NSObject {
//长按  点击 move
    public override init() {
        super.init()
    }
    public var isEnabled: Bool = false

    public enum State : Int {
        case possible
        case pan
        case pan2
        case recognized
        case ended
        case cancelled
        case failed
//        public static var recognized: UIGestureRecognizer.State { get }
    }
    
//    public var recogning: Bool
    
    
    public var tapHandler: TapEventHandler?
    public var panHandler: PanEventHandler?

    public func touchesBegan(view: UIView, touches: Set<UITouch>, with event: UIEvent?) {
        Log.logEvent(view: view, touches: touches, event: event)
//        guard let context = self.context else {
//            self.drawingHandler = nil
////            super.touchesBegan(touches, with: event)
//            print("touchesBegan context is nil")
//            return
//        }
//        guard let handler = context.shouldBegin(event: e) else {
//            self.drawingHandler = nil
////            super.touchesBegan(touches, with: event)
//            print("touchesBegan shouldBegin return nil")
//            return
//        }
//        self.drawingHandler = handler
//        if !handler.handleTouchesBegan(self, touches: touches, with: event) {
//            self.logTouch(touches, with: e)
//        }
    }
    public func touchesMoved(view: UIView, touches: Set<UITouch>, with event: UIEvent?) {
        Log.logEvent(view: view, touches: touches, event: event)

//        guard let e = event else {
////            super.touchesMoved(touches, with: event)
//            print("touchesMoved error: \(touches)")
//            return
//        }
//        guard let handler = self.drawingHandler else {
////            super.touchesMoved(touches, with: event)
//            print("touchesMoved drawingItem is nil")
//            return
//        }
//        if !handler.handleTouchesMoved(self, touches: touches, with: event) {
//            self.logTouch(touches, with: e)
//        }
    }
    public func touchesEnded(view: UIView, touches: Set<UITouch>, with event: UIEvent?) {
        Log.logEvent(view: view, touches: touches, event: event)

//        guard let e = event else {
////            super.touchesEnded(touches, with: event)
//            print("touchesEnded error: \(touches)")
//            return
//        }
//        guard let handler = self.drawingHandler else {
////            super.touchesEnded(touches, with: event)
//            print("touchesEnded drawingItem is nil")
//            return
//        }
//        if !handler.handleTouchesEnded(self, touches: touches, with: event) {
//            self.logTouch(touches, with: e)
//        }
    }
    public func touchesCancelled(view: UIView, touches: Set<UITouch>, with event: UIEvent?) {
        Log.logEvent(view: view, touches: touches, event: event)

//        guard let e = event else {
////            super.touchesCancelled(touches, with: event)
//            print("touchesCancelled error: \(touches)")
//            return
//        }
//        guard let handler = self.drawingHandler else {
////            super.touchesCancelled(touches, with: event)
//            print("touchesCancelled drawingItem is nil")
//            return
//        }
//        if !handler.handleTouchesCancelled(self, touches: touches, with: event) {
//            self.logTouch(touches, with: e)
//        }
    }

}




public class DrawingBoardView: UIView {
    public let eventRecognizer: DrawingEventRecognizer = DrawingEventRecognizer()
    
    private var isDispatchEventToRecognizer: Bool = false
    
    open override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?) {
        if self.eventRecognizer.isEnabled {
            self.isDispatchEventToRecognizer = true
        } else {
            self.isDispatchEventToRecognizer = false
        }
        guard self.isDispatchEventToRecognizer else {
            super.touchesBegan(touches, with: event)
            return
        }
        self.eventRecognizer.touchesBegan(view: self, touches: touches, with: event)
    }
    open override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard self.isDispatchEventToRecognizer else {
            super.touchesMoved(touches, with: event)
            return
        }
        self.eventRecognizer.touchesMoved(view: self, touches: touches, with: event)
    }
    open override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard self.isDispatchEventToRecognizer else {
            super.touchesEnded(touches, with: event)
            return
        }
        guard self.eventRecognizer.isEnabled else {
            return
        }
        self.eventRecognizer.touchesEnded(view: self, touches: touches, with: event)
    }
    open override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard self.isDispatchEventToRecognizer else {
            super.touchesCancelled(touches, with: event)
            return
        }
        guard self.eventRecognizer.isEnabled else {
            return
        }
        self.eventRecognizer.touchesCancelled(view: self, touches: touches, with: event)
    }
}

