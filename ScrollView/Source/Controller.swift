//
//  Controller.swift
//  ScrollView
//
//  Created by vector on 2020/1/15.
//  Copyright © 2020 angfung. All rights reserved.
//

import Foundation

public class DrawingBoardController {
    public struct BoxConfig {
        public let colorSpace: DrawingContext.ColorSpace
        public let backgroundColor: DrawingContext.Color
        public init(backgroundColor: DrawingContext.Color = DrawingContext.Color(little32Argb: 0x00_ff_ff_ff) , colorSpace: DrawingContext.ColorSpace = .little32Argb) {
            self.colorSpace = colorSpace
            self.backgroundColor = backgroundColor
        }
    }

    
    public let config: DrawingContext.BoxConfig
    public let frame: Rect

    public init(frame: Rect, config: DrawingContext.BoxConfig) {
        let v = C2DRectStandardize(frame);
        self.frame = v
        self.config = config
    }
    
}
