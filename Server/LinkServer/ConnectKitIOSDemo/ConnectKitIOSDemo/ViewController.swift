//
//  ViewController.swift
//  ConnectKitIOSDemo
//
//  Created by CaiLianfeng on 16/8/28.
//  Copyright © 2016年 WangQinghai. All rights reserved.
//

import UIKit
import Dispatch
import ConnectKit


public struct InteractionProtocol {
    public struct Package {
        public struct Header {
            public struct Error: Swift.Error {
                public enum Code {
                    case formatError
                    case checkError
                }
                
                public let code: Code
                public private(set) var description: String?
                
                public init(code: Code, description: String? = nil) {
                    self.code = code
                    self.description = description
                }
            }
            
            public let len: UInt32
            public var encodeBytes: [UInt8] {
                var number = len
                var numbers: [UInt32] = [number]
                for _ in 0 ..< 4 {
                    number = number >> 7
                    if number == 0 {
                        break
                    }else {
                        numbers.append(number)
                    }
                }
                var bytes: [UInt8] = []
                for (index, item) in numbers.reversed().enumerated() {
                    let b: UInt32 = 0x7f
                    var tmp = item & b
                    if index != numbers.count - 1 {
                        tmp = tmp | 0x80
                    }
                    bytes.append(UInt8(tmp))
                }
                bytes.append(Header.digest(len: len))
                return bytes
            }
            public let lenBytesLen: Int
            
            public init(len: UInt32) {
                self.len = len
                self.lenBytesLen = Header.encodeBytesLen(len: len)
            }
            
            public static func encodeBytesLen(len: UInt32) -> Int {
                var number = len
                var lenBytesLen = 1
                for _ in 0 ..< 3 {
                    number = number >> 7
                    if number == 0 {
                        break
                    }else {
                        lenBytesLen += 1
                    }
                }
                return lenBytesLen + 1
            }
            
            public static func decode(data: Data, usedLen: inout Int) throws -> Header? {
                let b: UInt8 = 0x7f
                var number: UInt32 = 0
                
                let maxLen = data.count > 5 ? 5 : data.count - 1
                for index in 0 ..< maxLen {
                    let item = data[index]
                    if item & 0x80 == 0x80 {//后边还有数据
                        number = number << 7
                        number += UInt32(item & b)
                    }else {
                        number = number << 7
                        number += UInt32(item & b)
                        let digestCode = data[index + 1]
                        if Header.checkDigest(len: number, digestCode: digestCode) {
                            usedLen = index + 2
                            return Header(len: number)
                        }else {
                            throw Header.Error(code: Header.Error.Code.checkError, description: "data decode check error, data is error")
                        }
                    }
                    if index == 4 {
                        throw Header.Error(code: Header.Error.Code.formatError, description: "data decode error, format is error")
                    }
                }
                usedLen = 0
                return nil
            }
            
            public static func decode(bytes: [UInt8], usedLen: inout Int) throws -> Header? {
                let b: UInt8 = 0x7f
                var number: UInt32 = 0
                
                let maxLen = bytes.count > 5 ? 5 : bytes.count - 1
                for index in 0 ..< maxLen {
                    let item = bytes[index]
                    if item & 0x80 == 0x80 {//后边还有数据
                        number = number << 7
                        number += UInt32(item & b)
                    }else {
                        number = number << 7
                        number += UInt32(item & b)
                        let digestCode = bytes[index + 1]
                        if Header.checkDigest(len: number, digestCode: digestCode) {
                            usedLen = index + 2
                            return Header(len: number)
                        }else {
                            throw Header.Error(code: Header.Error.Code.checkError, description: "data decode check error, data is error")
                        }
                    }
                    if index == 4 {
                        throw Header.Error(code: Header.Error.Code.formatError, description: "data decode error, format is error")
                    }
                }
                usedLen = 0
                return nil
            }
            
            public static func digest(len: UInt32) -> UInt8 {
                var tmp = len >> 8
                var lenValidCount: UInt8 = 1
                
                while tmp > 0 {
                    tmp = tmp >> 8
                    lenValidCount += 1
                }
                lenValidCount = lenValidCount << 6
                return (lenValidCount | UInt8(len & UInt32(0x3f)))
            }
            public static func checkDigest(len: UInt32, digestCode: UInt8) -> Bool {
                return digest(len: len) == digestCode
            }
        }
        public private(set) var header: Header
        public private(set) var body: Data?
        
        public private(set) var encodeBytesLen: Int
        public var encodeData: Data {
            var data = Data(bytes: header.encodeBytes)
            if let _body = body {
                data.append(_body)
            }
            return data
        }
        public init(body: Data?) {
            var _header: Header
            if let _body = body {
                _header = Header(len: UInt32(_body.count))
            }else {
                _header = Header(len: 0)
            }
            header = _header
            self.body = body
            
            var _encodeBytesLen: Int = _header.lenBytesLen
            if let _body = body {
                _encodeBytesLen += _body.count
            }
            self.encodeBytesLen = _encodeBytesLen
        }
    }
    
    public class PackageSeparater {
        public struct Error: Swift.Error {
            public enum Code {
                case deserializeHeaderError
                case canceled
            }
            
            public let code: Code
            public init(code: Code) {
                self.code = code
            }
        }
        
        
        struct TempBuffer {
            var header: InteractionProtocol.Package.Header
            var data: Data?
            var remainLen: Int {
                var _remainLen = Int(header.len)
                if let _data = data {
                    _remainLen -= _data.count
                }
                return _remainLen
            }
            var isComplete: Bool {
                return remainLen == 0
            }
            private mutating func append(from: inout Data) -> Int {
                if self.data != nil {
                    self.data!.append(from)
                }else {
                    self.data = from
                }
                return from.count
            }
            
            mutating func en(from: inout Data, beginIndex: Int) -> Int {
                if remainLen == 0 {
                    return 0
                }
                let inLen = from.count - beginIndex
                var needSub = true
                if inLen < 0 {
                    print("mutating func en(from: Data, beginIndex: Int) -> Int ,  beginIndex is Error")
                    return 0
                }else if inLen == 0 {
                    return 0
                }else {
                    if inLen <= remainLen && beginIndex == 0 {
                        needSub = false
                    }
                }
                if needSub {
                    var subData = from.subdata(in: Range<Data.Index>(uncheckedBounds: (lower: beginIndex, upper: beginIndex + remainLen)))
                    return append(from: &subData)
                }else {
                    return self.append(from: &from)
                }
            }
            
            init(header: InteractionProtocol.Package.Header) {
                self.header = header
            }
        }
        var readDataBuffer: [UInt8]?
        var tempBuffer: TempBuffer? = nil
        
        public init() {}
        
        public func clean() {
            self.readDataBuffer = nil
            self.tempBuffer = nil
        }
        
        public func handleData(from: Data, handleHeader: (Package.Header) -> Bool, handleDataItem: (inout DataItem) -> Void) throws {
            var data = from
            var usedDataCount = 0
            
            if readDataBuffer != nil {
                let readDataBufferCount = readDataBuffer!.count
                
                //                while data.count - usedDataCount > 0 {
                let headDataBufferLen = readDataBufferCount + (data.count - usedDataCount) >= 6 ? 6 : readDataBufferCount + (data.count - usedDataCount)
                
                var headDataBuffer: [UInt8] = [UInt8](repeating: 0, count: headDataBufferLen)
                
                var index = 0
                if readDataBufferCount != 0 {
                    for byte in readDataBuffer! {
                        headDataBuffer[index] = byte
                        index += 1
                    }
                }
                var useLen = 0
                while index < headDataBufferLen {
                    headDataBuffer[index] = data[usedDataCount + useLen]
                    index += 1
                    useLen += 1
                }
                
                do {
                    var usedLen: Int = 0
                    if let header = try InteractionProtocol.Package.Header.decode(bytes: headDataBuffer, usedLen: &usedLen) {
                        if !handleHeader(header) {
                            throw Error(code: InteractionProtocol.PackageSeparater.Error.Code.canceled)
                        }
                        tempBuffer = TempBuffer(header: header)
                        readDataBuffer = nil
                        usedDataCount = usedLen - readDataBufferCount
                        if usedDataCount < 0 {
                            print("func error : usedDataCount = usedLen - readDataBufferCount < 0 ")
                        }
                    }else {
                        readDataBuffer!.append(contentsOf: data.map({ (byte) -> UInt8 in
                            return byte
                        }))
                        return
                    }
                } catch _ {
                    throw Error(code: InteractionProtocol.PackageSeparater.Error.Code.deserializeHeaderError)
                }
            }
            
            while data.count - usedDataCount > 0 {
                if tempBuffer != nil {
                    let temUsedLen = tempBuffer!.en(from: &data, beginIndex: usedDataCount)
                    usedDataCount += temUsedLen
                    if tempBuffer!.isComplete {
                        if tempBuffer!.data == nil {
                            var dataItem = DataItem(data: nil, tag: "")
                            handleDataItem(&dataItem)
                        }else {
                            var dataItem = DataItem(data: tempBuffer!.data!, tag: "")
                            handleDataItem(&dataItem)
                        }
                        tempBuffer = nil
                    }
                }else {
                    let headDataBufferLen = (data.count - usedDataCount) >= 6 ? 6 : (data.count - usedDataCount)
                    var headDataBuffer: [UInt8] = [UInt8](repeating: 0, count: headDataBufferLen)
                    
                    var index = 0
                    
                    var useLen = 0
                    while index < headDataBufferLen {
                        headDataBuffer[index] = data[usedDataCount + useLen]
                        index += 1
                        useLen += 1
                    }
                    
                    do {
                        var usedLen: Int = 0
                        if let header = try InteractionProtocol.Package.Header.decode(bytes: headDataBuffer, usedLen: &usedLen) {
                            if !handleHeader(header) {
                                throw Error(code: InteractionProtocol.PackageSeparater.Error.Code.canceled)
                            }
                            tempBuffer = TempBuffer(header: header)
                            usedDataCount += usedLen
                        }else {
                            self.readDataBuffer = headDataBuffer
                            return
                        }
                        let temUsedLen = tempBuffer!.en(from: &data, beginIndex: usedDataCount)
                        usedDataCount += temUsedLen
                        if tempBuffer!.isComplete {
                            if tempBuffer!.data == nil {
                                var dataItem = DataItem(data: nil, tag: "")
                                handleDataItem(&dataItem)
                            }else {
                                var dataItem = DataItem(data: tempBuffer!.data!, tag: "")
                                handleDataItem(&dataItem)
                            }
                            tempBuffer = nil
                        }
                    }catch _ {
                        throw Error(code: InteractionProtocol.PackageSeparater.Error.Code.deserializeHeaderError)
                    }
                }
            }
        }

    }
    
}

public struct DataItem {
    public fileprivate(set) var tag: String
    public private(set) var data: Data?
    
    public init(data: Data?, tag: String) {
        self.data = data
        self.tag = tag
    }
}

//FIFO
open class PackageBuffer {
    
    public fileprivate(set) var buffer: [DataItem]
    
    public var itemCount: Int {
        return buffer.count
    }
    
    public init() {
        buffer = []
    }
    open func enBuffer(data: Data?, tag: String) {
        let item = DataItem(data: data, tag: tag)
        buffer.append(item)
    }
    open func enBuffer(item: DataItem) {
        buffer.append(item)
    }
    @discardableResult open func deFirst() -> DataItem? {
        if let item = buffer.first {
            buffer.removeFirst()
            return item
        }else {
            return nil
        }
    }
    
    open var first: DataItem? {
        return buffer.first
    }
}





let SocketQueue = DispatchQueue(label: "SocketQueue", attributes: [])
class UserDefaultsInfo {
    static let IPKey = "IP"
    static let PortKey = "PortKey"

    static func string(_ forKey: String) -> String? {
        return UserDefaults.standard.object(forKey: forKey) as? String
    }
    static func saveString(_ value: String, forKey: String) {
        UserDefaults.standard.set(value, forKey: forKey)
        UserDefaults.standard.synchronize()
    }
}


class ViewController: UIViewController {

    var connect: SocketConnect!

    
    @IBOutlet weak var titleLabel: UILabel!
    
    
    @IBOutlet weak var ip: UITextField!
    
    
    @IBOutlet weak var portTextField: UITextField!
    
    @IBOutlet weak var debugTextView: UITextView!
    
    
    @IBOutlet weak var messageTextField: UITextField!
    
    
    @IBOutlet weak var connectButton: UIButton!
    
    
    
    @IBOutlet weak var closeButton: UIButton!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.

        
        self.connect = SocketConnect(delegateQueue: SocketQueue)
        SocketQueue.async{
            self.connect.delegate = self
        }
        self.connectButton.isEnabled = true
        self.closeButton.isEnabled = false
        connect.listenConnectStatusChanged(nil, changedTo: nil) { (connect, oldState, newState) -> ListenSignal in
            DispatchQueue.main.async {
                if newState == .disconnect {
                    self.connectButton.isEnabled = true
                    self.closeButton.isEnabled = false
                }else {
                    self.connectButton.isEnabled = false
                    self.closeButton.isEnabled = true
                }
                self.titleLabel.text = "\(newState)"
            }
            return .continue
        }
        self.portTextField.text = UserDefaultsInfo.string(UserDefaultsInfo.PortKey)
        self.ip.text = UserDefaultsInfo.string(UserDefaultsInfo.IPKey)

    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


    @IBAction func connectButtonClicked(_ sender: UIButton) {
        
        UserDefaultsInfo.saveString(ip.text!, forKey: UserDefaultsInfo.IPKey)
        UserDefaultsInfo.saveString(portTextField.text!, forKey: UserDefaultsInfo.PortKey)
        let ip = self.ip.text ?? ""
        let port = self.portTextField.text ?? ""
        SocketQueue.async {
            self.connect.tryConnect(ip, port: Int(port)!, tlsSettings: nil)
        }
    }
    
    
    @IBAction func closeButtonClicked(_ sender: AnyObject) {
        SocketQueue.async {
            self.connect.disconnect()
        }
        
    }
    
    @IBOutlet weak var sendButtonClicked: UIButton!
    
    @IBAction func clearButtonClicked(_ sender: AnyObject) {
        
        self.debugTextView.text = ""
        
        self.connect.readOnce()
        
    }
    
    @IBAction func sendButtonClieked(_ sender: AnyObject) {
        var message: String = ""
        if let str = self.messageTextField.text {
            message = str
        }
        
        self.messageTextField.text = ""
        
        SocketQueue.async {
            self.send(text: message)
        }
        
    }
    
    let packageSeparater: InteractionProtocol.PackageSeparater = InteractionProtocol.PackageSeparater()
    
    func handleDataItem(data: Data) {
        DispatchQueue.main.async {
            if let str = String(data: data, encoding: String.Encoding.utf8) {
                let ocStr = str as NSString
                var text: String = ""
                if ocStr.length > 100 {
                    text = ocStr.substring(to: 100)
                    text += "  剩余长度:\((ocStr.length - 100))"
                }else {
                    text = str
                }
                
                self.debugTextView.text = self.debugTextView.text! + text + "\n"
            }else {
                self.debugTextView.text = self.debugTextView.text! + "error\n"
            }
        }
    }
    
    func handleEmptyPackage() {
        log()

        
    }
    
    func close(error: Swift.Error?) {
        print(error)
    }
    
    func send(text: String) {
        let package = InteractionProtocol.Package(body: text.data(using: String.Encoding.utf8))
        let _ =  self.connect.writeAfterConnect(package.encodeData, option: 0)
    }
    
    
}

func log(funcName: String = #function) {
    print(funcName)
}

extension ViewController: ConnectDelegate {
    func connectSuccess(_ connect: SocketConnect) {
        IMLog.info("connectSuccess")
        
        
        /*
         do login   then  interation = true
         
         */
        
        self.connect.readOnce()
        
    }
    func connectFailure(_ connect: SocketConnect, error: NSError) {
        IMLog.info("connectFailure")
    }
    func connectClosed(_ connect: SocketConnect, error: NSError?) {
        IMLog.info("connectClosed")
        
        connect.interacted = false
        
    }
    func connectDidReceiveData(_ connect: SocketConnect, data: Data) {
        IMLog.info("connectDidReceiveData")
        do {
            try packageSeparater.handleData(from: data, handleHeader: { (header) -> Bool in
                return true
            }) { (dataItem) in
                if dataItem.data == nil {
                    self.handleEmptyPackage()
                }else {
                    self.handleDataItem(data: dataItem.data!)
                }
            }
        }catch let error {
            print("error: \(error)")
        }
        self.connect.readOnce()
    }
}






