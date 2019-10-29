//
//  AppDelegate.swift
//  MyCombine
//
//  Created by vector on 2019/10/29.
//  Copyright © 2019 vector. All rights reserved.
//

import Cocoa




@propertyWrapper public struct Observed<Value> {

    public init(wrappedValue: Value) {
        self._value = wrappedValue
    }

    /// A publisher for properties marked with the `@Published` attribute.
    public struct Publisher: OpenCombine.Publisher {

        /// The kind of values published by this publisher.
        public typealias Output = Value

        /// The kind of errors this publisher might publish.
        ///
        /// Use `Never` if this `Publisher` does not publish errors.
        public typealias Failure = Never

        /// This function is called to attach the specified
        /// `Subscriber` to this `Publisher` by `subscribe(_:)`
        ///
        /// - SeeAlso: `subscribe(_:)`
        /// - Parameters:
        ///     - subscriber: The subscriber to attach to this `Publisher`.
        ///                   once attached it can begin to receive values.
        public func receive<Downstream: Subscriber>(subscriber: Downstream)
            where Downstream.Input == Value, Downstream.Failure == Never
        {
            subject.subscribe(subscriber)
        }

        fileprivate let subject: OpenCombine.CurrentValueSubject<Value, Never>

        fileprivate init(_ output: Output) {
            subject = .init(output)
        }
    }

    private var _value: Value
    let aa: ReferenceWritableKeyPath
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
        get { self._value }
        set {
            self._value = newValue
//            $.
//            publisher?.subject.value = newValue
        }
    }

//    private var publisher: Publisher?
//
//    @available(*, unavailable, message:
//        "This subscript is unavailable in OpenCombine yet")
//    public static subscript<EnclosingSelf: AnyObject>(
//        _enclosingInstance object: EnclosingSelf,
//        wrapped wrappedKeyPath: ReferenceWritableKeyPath<EnclosingSelf, Value>,
//        storage storageKeyPath: ReferenceWritableKeyPath<EnclosingSelf, Published<Value>>
//    ) -> Value {
//        get { fatalError() }
//        set { fatalError() }
//    }
}




@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    @IBOutlet weak var window: NSWindow!

    @Observed public var subTitle: String?

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        
        
        self.subTitle
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

