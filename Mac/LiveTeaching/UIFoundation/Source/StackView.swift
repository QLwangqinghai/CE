//
//  StackView.swift
//  Basic
//
//  Created by vector on 2020/2/16.
//

import UIKit

open class StackView: UIStackView {
    public class Observer {
        public typealias ObserveClosure = (_ observer: Observer, _ view: StackView) -> Void
        public var willLayoutSubviews: ObserveClosure
        public var didLayoutSubviews: ObserveClosure
        public init() {
            self.willLayoutSubviews = {(_, _) in
            }
            self.didLayoutSubviews = {(_, _) in
            }
        }
    }
    
    private var observers: [Observer] = []

    public func addObserver(_ observer: Observer) {
        self.observers.append(observer)
    }
    public func removeObserver(_ observer: Observer) {
        self.observers.removeAll(where: { (item) -> Bool in
            return item === observer
        })
    }
    
    public override init(frame: CGRect) {
        super.init(frame: frame)
    }
    public required init(coder: NSCoder) {
        super.init(coder: coder)
    }
    
    public override func layoutSubviews() {
        let observers = self.observers
        for observer in observers {
            observer.willLayoutSubviews(observer, self)
        }
        super.layoutSubviews()
        for observer in observers.reversed() {
            observer.didLayoutSubviews(observer, self)
        }
    }

}
