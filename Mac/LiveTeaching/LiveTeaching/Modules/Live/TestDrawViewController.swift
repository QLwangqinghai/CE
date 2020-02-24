//
//  TestDrawViewController.swift
//  LiveTeaching
//
//  Created by vector on 2020/2/24.
//  Copyright © 2020 vector. All rights reserved.
//

import UIKit

public class DView: UIView {
    let bitmapContext: CGContext
    
    let bitmapLayout: Drawing.BitmapLayout
    let bitmap: UnsafeMutableRawPointer
    let bitmapFrame: CGRect
    
    public override init(frame: CGRect) {
        let width = UInt32(200 * UIScreen.main.scale + 0.4)
        let height = width
        let bitmapLayout: Drawing.BitmapLayout = Drawing.BitmapLayout(width: width, colorSpace: Drawing.ColorSpace.little32Argb, backgroundColor: Drawing.Color(little32Argb: 0))
        let ptr = UnsafeMutableRawPointer.allocate(byteCount: bitmapLayout.bytesPerRow * Int(height), alignment: 4)
        self.bitmapLayout = bitmapLayout
        self.bitmap = ptr

        let colorSpace = bitmapLayout.colorSpace
        let bitmapContext = CGContext(data: ptr, width: Int(width), height: Int(height), bitsPerComponent: Int(colorSpace.bitsPerComponent), bytesPerRow: bitmapLayout.bytesPerRow, space: colorSpace.space, bitmapInfo: colorSpace.bitmapInfo, releaseCallback: { (context, ptr) in
        }, releaseInfo: nil)!
        self.bitmapContext = bitmapContext
        self.bitmapFrame = CGRect(x: 0, y: 0, width: Double(width), height: Double(height))
        super.init(frame: frame)
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    func displayContent() {
        self.layer.contents = self.bitmapContext.makeImage()
        self.layer.superlayer?.setNeedsDisplay()
    }
    
    func test0() {
        self.bitmapContext.setFillColor(UIColor.clear.cgColor)
        self.bitmapContext.fill(self.bitmapFrame)
        
        self.bitmapContext.setLineWidth(30)
        self.bitmapContext.setStrokeColor(UIColor.red.cgColor)

        self.bitmapContext.addEllipse(in: self.bitmapFrame)
        self.bitmapContext.strokePath()
        self.displayContent()
    }
    
}


class TestDrawViewController: ViewController {

    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        
        let view = DView(frame: CGRect.init(x: 40, y: 100, width: 200, height: 200))
        view.backgroundColor = .black
        self.view.addSubview(view)
        
        DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 3) {
            view.test0()
        }
        
    }
    

    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepare(for segue: UIStoryboardSegue, sender: Any?) {
        // Get the new view controller using segue.destination.
        // Pass the selected object to the new view controller.
    }
    */

}
