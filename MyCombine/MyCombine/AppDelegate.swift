//
//  AppDelegate.swift
//  MyCombine
//
//  Created by vector on 2019/10/29.
//  Copyright © 2019 vector. All rights reserved.
//

import Cocoa


public struct ObserveKey : Hashable {
    static var sequence: UInt64 = 0
    static var lock: NSLock = NSLock()

    static func nextSequence() -> UInt64 {
        lock.lock()
        ObserveKey.sequence += 1
        let sequence = ObserveKey.sequence
        lock.unlock()
        return sequence
    }
    
    private let key: UInt64
    
    private init(key: UInt64) {
        self.key = key
    }
    public init() {
        self.key = ObserveKey.nextSequence()
    }
    
    
}

@propertyWrapper public struct Observed<Value> where Value: Equatable {
    public init(wrappedValue: Value) {
        self._value = wrappedValue
    }

    public struct Change {
        public let old: Value
        public let new: Value
        public init(old: Value, new: Value) {
            self.old = old
            self.new = new
        }
    }
    public class Observer {
        public let closure: ObserveClosure
        public let key: ObserveKey
        fileprivate init(closure: @escaping ObserveClosure) {
            self.key = ObserveKey()
            self.closure = closure
        }
        fileprivate init(key: ObserveKey, closure: @escaping ObserveClosure) {
            self.key = key
            self.closure = closure
        }
    }
    fileprivate class Publisher {
        private var observers: [ObserveKey : Observer] = [:]
        
        fileprivate func insert(_ observer: Observer) {
            self.observers[observer.key] = observer
        }
        fileprivate func remove(_ observer: Observer) {
            self.observers[observer.key] = nil
        }
        fileprivate func remove(_ key: ObserveKey) {
            self.observers[key] = nil
        }
        fileprivate func publish(_ change: Change) {
            let observers = self.observers
            observers.forEach { (_, value) in
                value.closure(value, change)
            }
        }
    }
    
    public typealias ObserveClosure = (_ observer: Observer, _ change: Change) -> Void
    
    private var _publisher: Publisher?
    private var publisher: Publisher {
        mutating get {
            if let v = _publisher {
                return v
            } else {
                let v = Publisher()
                _publisher = v
                return v
            }
        }
    }

//
//    /// A publisher for properties marked with the `@Published` attribute.
//    public struct Publisher: OpenCombine.Publisher {
//
//        /// The kind of values published by this publisher.
//        public typealias Output = Value
//
//        /// The kind of errors this publisher might publish.
//        ///
//        /// Use `Never` if this `Publisher` does not publish errors.
//        public typealias Failure = Never
//
//        /// This function is called to attach the specified
//        /// `Subscriber` to this `Publisher` by `subscribe(_:)`
//        ///
//        /// - SeeAlso: `subscribe(_:)`
//        /// - Parameters:
//        ///     - subscriber: The subscriber to attach to this `Publisher`.
//        ///                   once attached it can begin to receive values.
//        public func receive<Downstream: Subscriber>(subscriber: Downstream)
//            where Downstream.Input == Value, Downstream.Failure == Never
//        {
//            subject.subscribe(subscriber)
//        }
//
//        fileprivate let subject: OpenCombine.CurrentValueSubject<Value, Never>
//
//        fileprivate init(_ output: Output) {
//            subject = .init(output)
//        }
//    }

    private var _value: Value
    /// The property that can be accessed with the
    /// `$` syntax and allows access to the `Publisher`
//    public var projectedValue: Publisher {
//        mutating get {
//            if let publisher = publisher {
//                return publisher
//            }
//            let publisher = Publisher(value)
//            self.publisher = publisher
//            return publisher
//        }
//    }
//
//    @available(*, unavailable, message:
//        "@Published is only available on properties of classes")
//
    public var wrappedValue: Value {
        get { _value }
        set {
            let old = _value
            _value = newValue
            
            if old != newValue {
                let change = Change(old: old, new: newValue)
                self._publisher?.publish(change)
            }
        }
    }

    public mutating func observe(_ closure: @escaping ObserveClosure) -> Observer {
        let observer: Observer = Observer(closure: closure)
        self.publisher.insert(observer)
        return observer
    }
    public mutating func addObserver(key: ObserveKey, closure: @escaping ObserveClosure) -> Observer {
        let observer: Observer = Observer(closure: closure)
        self.publisher.insert(observer)
        return observer
    }
    public mutating func removeObserver(_ observer: Observer) {
        self.publisher.remove(observer)
    }
    public mutating func removeObserver(forKey key: ObserveKey) {
        self.publisher.remove(key)
    }

}




@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    @IBOutlet weak var window: NSWindow!

    @Observed public var subTitle: String?

    
    let ob: NSObject = NSObject()
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        
        
//        self.subTitle
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }

    func ob<Value>(_ keyPath: KeyPath<AppDelegate, Observed<Value>>, options: NSKeyValueObservingOptions = [], changeHandler: @escaping (AppDelegate, NSKeyValueObservedChange<Value>) -> Void) -> Void {
        let a = self[keyPath: keyPath]
        let k: NSKeyValueChange?
//        a.
        print("a")
        
 
        self._subTitle.observe { (observer, change) in
            
        }
        
    }

    
}



//public extension
//func ob<Value>(_ keyPath: KeyPath<AppDelegate, Value>, options: NSKeyValueObservingOptions = [], changeHandler: @escaping (AppDelegate, NSKeyValueObservedChange<Value>) -> Void) -> NSKeyValueObservation {
//
//}
//
//public func invalidate()
