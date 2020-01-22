//
//  ViewController.swift
//  LiveTeaching
//
//  Created by vector on 2020/1/20.
//  Copyright © 2020 vector. All rights reserved.
//

import UIFoundation

class ViewController: BaseViewController {
    public let panGestureRecognizer: UIPanGestureRecognizer = UIPanGestureRecognizer()

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        self.panGestureRecognizer.addTarget(self, action: #selector(ViewController.handlePan))

    }


    @objc private func handlePan(recognizer: UIPanGestureRecognizer) {
        guard recognizer == self.panGestureRecognizer else {
            return
        }
        print(recognizer.state)
    }

}

