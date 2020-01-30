//
//  Connect.swift
//  ConnectKitDemo
//
//  Created by CaiLianfeng on 16/8/28.
//  Copyright © 2016年 WangQinghai. All rights reserved.
//

import Foundation



public protocol OptionsValueListenAble {
    static func options(_ from: [Self]) -> Self
    func contentOption(_ item: Self) -> Bool
}

extension UInt32: OptionsValueListenAble {
    public static func options(_ from: [UInt32]) -> UInt32 {
        var result: UInt32 = 0
        for item in from {
            result |= item
        }
        return result
    }
    public func contentOption(_ item: UInt32) -> Bool {
        if self & item == item {
            return true
        }else {
            return false
        }
    }
}
extension UInt64: OptionsValueListenAble {
    public static func options(_ from: [UInt64]) -> UInt64 {
        var result: UInt64 = 0
        for item in from {
            result |= item
        }
        return result
    }
    public func contentOption(_ item: UInt64) -> Bool {
        if self & item == item {
            return true
        }else {
            return false
        }
    }
}

open class CommonListenItem<ListenedObjectType, OptionsType: OptionsValueListenAble, StatusType>: NSObject {
    let oldOptions: OptionsType
    let newOptions: OptionsType
    
    public typealias ListenClosure = (_ obj: ListenedObjectType, _ old: StatusType, _ new: StatusType) -> ListenSignal
    var closure: ListenClosure
    
    init(oldOptions: [OptionsType], newOptions: [OptionsType], closure: @escaping ListenClosure) {
        self.oldOptions = OptionsType.options(oldOptions)
        self.newOptions = OptionsType.options(newOptions)
        self.closure = closure
    }
    
    fileprivate func canHandleOptionChanged(_ oldOption: OptionsType, newOption: OptionsType) -> Bool {
        if oldOptions.contentOption(oldOption) && newOptions.contentOption(newOption) {
            return true
        }else {
            return false
        }
    }
}


public protocol ConnectDelegate: class {
    func connectSuccess(_ connect: SocketConnect)
    func connectFailure(_ connect: SocketConnect, error: NSError)
    func connectClosed(_ connect: SocketConnect, error: NSError?)
    func connectDidReceiveData(_ connect: SocketConnect, data: Data)
}

public typealias SocketConnectClosure = (SocketConnect) -> Void
open class SocketConnect : NSObject {
    public typealias DataItemClosure = (_ connect: SocketConnect, _ item: SocketConnect.DataItem) -> Void
    
    public typealias ListenStatusChangeClosure = (_ connect: SocketConnect, _ oldStatus: Status, _ newStatus: Status) -> ListenSignal
    open class ListenItem: CommonListenItem<SocketConnect, UInt32, Status> {
        
        fileprivate static func UInt32Value(_ from: [Status]) -> UInt32 {
            var value: UInt32 = 0
            for statusItem in from {
                value |= statusItem.rawValue
            }
            return value
        }
        fileprivate static func UInt32Values(_ from: [Status]) -> [UInt32] {
            var values: [UInt32] = []
            for statusItem in from {
                values.append(statusItem.rawValue)
            }
            return values
        }
        
        init(oldStatuss: [Status], newStatuss: [Status], closure: @escaping ListenClosure) {
            super.init(oldOptions: ListenItem.UInt32Values(oldStatuss), newOptions: ListenItem.UInt32Values(newStatuss), closure: closure)
        }
        
        fileprivate func handleStatusChanged(_ connect: SocketConnect, oldStatus: Status, newStatus: Status) -> ListenSignal {
            if super.canHandleOptionChanged(oldStatus.rawValue, newOption: newStatus.rawValue) {
//                if let clos = self.closure as ListenStatusChangeClosure {
//                    clos(connect, oldStatus, newStatus)
//                }
                
//                print(self.closure)
                let result = self.closure(connect, oldStatus, newStatus)
                return result
            }else {
                return ListenSignal.continue
            }
        }
    }
    public enum HighPermission: UInt64 {
        case afterConnected = 0x8000000000000000
        case afterInteracted = 0x4000000000000000
    }
    open class DataItem: NSObject {
        public enum Status: UInt32 {
            case waiting = 0
            case sending = 1
            case waitTimeouted = 2
            case sendFailure = 3
            case sendSuccess = 4
        }
        
        internal enum TryType: UInt32 {
            case noTry = 0
            case tryUntilTimeout = 1
        }
        
        public let data: Data
        fileprivate let tryType: TryType
        fileprivate var timeouted: Bool = false
        fileprivate var permissionOption: UInt64
        fileprivate var tag: Int = 0
        
        
        open dynamic fileprivate(set) var statusRawValue: UInt32 = 0
        open fileprivate(set) var status: Status = .waiting {
            didSet(oldValue) {
                if oldValue != status {
                    statusRawValue = status.rawValue
                }
            }
        }
        
        fileprivate var successClosure: DataItemClosure?
        fileprivate var timeoutClosure: DataItemClosure?
        fileprivate var failureClosure: DataItemClosure?
        
        init(data: Data, tryType: TryType, permissionOption: UInt64) {
            self.data = data
            self.tryType = tryType
            self.permissionOption = permissionOption
        }
    }
    
    public enum Status : UInt32 {
        case disconnect = 0x1
        case connecting = 0x2
        case connected = 0x4
        
        public static var AllStatuss: [SocketConnect.Status] = [Status.disconnect, Status.connecting, Status.connected]
    }
    
    
    //Permissions
    fileprivate var connectPermission: UInt64 = 0 {
        didSet(oldValue) {
            if oldValue != connectPermission {
                remakeTotalPermissions()
            }
        }
    }
    fileprivate var interactedPermission: UInt64 = 0{
        didSet(oldValue) {
            if oldValue != interactedPermission {
                remakeTotalPermissions()
            }
        }
    }
    open var customPermission: UInt32 = 0{
        didSet(oldValue) {
            if oldValue != customPermission {
                remakeTotalPermissions()
            }
        }
    }
    
    fileprivate func remakeTotalPermissions() {
        var _totalPermissions: UInt64 = 0
        _totalPermissions |= connectPermission
        _totalPermissions |= interactedPermission
        _totalPermissions |= UInt64(customPermission)
        totalPermissions = _totalPermissions
    }

    var totalPermissions: UInt64 = 0 {
        didSet(oldValue) {
            if oldValue != totalPermissions {
                _setNeedWrite()
            }
        }
    }
    
    
    //连接状态
    open dynamic fileprivate(set) var statusRawValue: UInt32 = 0
    open var status: Status = .disconnect {
        didSet(oldValue) {
            if oldValue != status {
                if status == .connected {
                    connectPermission = HighPermission.afterConnected.rawValue
                }else {
                    connectPermission = 0
                }
                
                statusRawValue = status.rawValue
                let items = self.listenItems
                for item in items {
                    if item.handleStatusChanged(self, oldStatus: oldValue, newStatus: status) == .cancel {
                        if let index = listenItems.index(of: item) {
                            listenItems.remove(at: index)
                        }
                    }
                }
            }
        }
    }
    
    open var interacted: Bool = false {
        didSet(oldValue) {
            if oldValue != interacted {
                if interacted {
                    interactedPermission = HighPermission.afterInteracted.rawValue
                }else {
                    interactedPermission = 0
                }
            }
        }
    }
    
    fileprivate var sendItems: [Int: DataItem] = [:]
    fileprivate var itemQueue: [DataItem] = []
    
    
    fileprivate var tagSequence: SequenceGenerator<Int> = SequenceGenerator(sequence: 0)
    //socket
    internal var socket: GCDAsyncSocket?
    
    //代理队列
    public let delegateQueue: DispatchQueue
    
    //socket ip
    internal var ipAdr: String = ""
    
    //socket 端口
    internal var port: Int = 0
    
    
    open weak var delegate: ConnectDelegate?
    internal var tlsSettings: [AnyHashable: Any]?
    internal var listenItems: [ListenItem] = []
    
    public init(delegateQueue: DispatchQueue = DispatchQueue.main) {
        self.delegateQueue = delegateQueue
    }
    
    //default is all status
    open func listenConnectStatusChanged(_ oldStatuss: [Status]?, changedTo newStatuss: [Status]?, closure: @escaping ListenStatusChangeClosure) -> ListenItem? {
        var old: [Status]
        if let _oldStatuss = oldStatuss {
            if _oldStatuss.count == 0 {
                return nil
            }
            old = _oldStatuss
        }else {
            old = Status.AllStatuss
        }
        
        var new: [Status]
        if let _newStatuss = newStatuss {
            if _newStatuss.count == 0 {
                return nil
            }
            new = _newStatuss
        }else {
            new = Status.AllStatuss
        }
        
        let item = ListenItem(oldStatuss: old, newStatuss: new, closure: closure)
        listenItems.append(item)
        return item
    }
    open func cancelListenConnectStatusChanged(_ item: ListenItem) {
        if let index = listenItems.index(of: item) {
            listenItems.remove(at: index)
        }
    }
    
    //断开之后回调
    fileprivate func checkOnceDataItems() {
        let olditemQueue = self.itemQueue
        for item in olditemQueue {
            if item.tryType == .noTry {
                item.status = .sendFailure
                item.failureClosure?(self, item)
                item.timeoutClosure = nil
                item.successClosure = nil
                item.failureClosure = nil
                if let index = itemQueue.index(of: item) {
                    itemQueue.remove(at: index)
                }
                sendItems.removeValue(forKey: item.tag)
            }else {
                if item.timeouted == true {
                    item.status = .waitTimeouted
                    item.timeoutClosure?(self, item)
                    item.timeoutClosure = nil
                    item.successClosure = nil
                    item.failureClosure = nil
                    if let index = itemQueue.index(of: item) {
                        itemQueue.remove(at: index)
                    }
                    sendItems.removeValue(forKey: item.tag)
                }else {
                    item.status = .waiting
                }
            }
        }
    }
    
    //must call in delegateQueue, default queue is mainQueue, when call this will disconnect old stream, then all items will send faild
    open func tryConnect(_ ipAdr: String, port: Int, tlsSettings: [AnyHashable: Any]?) {
        
        if self.ipAdr == ipAdr && self.port == port && (self.tlsSettings == nil) == (tlsSettings == nil) {
            if self.status == .connected {
                return
            }
        }

        self.ipAdr = ipAdr
        self.port = port
        self.tlsSettings = tlsSettings
        if let _socket = socket {
            _socket.delegate = nil
            _socket.disconnect()
        }
        self.status = .disconnect
        
        checkOnceDataItems()
        
        socket = GCDAsyncSocket(delegate: self, delegateQueue: self.delegateQueue)
        do {
            try socket!.connect(toHost: self.ipAdr, onPort: UInt16(self.port))
        }catch let error {
            self.delegateQueue.async(execute: {
                self.status = .disconnect
                self.delegate?.connectFailure(self, error: error as NSError)
            })
        }
        self.status = .connecting
    }
    open func disconnect() {
        if let csocket = socket {
            csocket.disconnect()
            self.status = .disconnect
        }
    }
    internal func removeAllSendItems() {
        sendItems.removeAll()
    }
    
    fileprivate func makeItemToConnect(_ tryType: DataItem.TryType, data: Data, option: UInt32, waitingTimeout: TimeInterval, successClosure: DataItemClosure?, timeoutClosure: DataItemClosure?, failureClosure: DataItemClosure?) -> DataItem {
        let itemOption = UInt64(option) | HighPermission.afterConnected.rawValue
        let item = DataItem(data: data, tryType: tryType, permissionOption: itemOption)
        item.status = .waiting
        var tag = tagSequence.nextSequence()
        
        while sendItems[tag] != nil {
            tag = tagSequence.nextSequence()
        }
        
        item.tag = tag
        item.successClosure = successClosure
        item.timeoutClosure = timeoutClosure
        item.failureClosure = failureClosure
        
        itemQueue.append(item)
        sendItems[tag] = item
        var timeout: TimeInterval
        if waitingTimeout < 3 {
            timeout = 3
        }else if waitingTimeout > 3600 {
            timeout = 3600
        }else {
            timeout = waitingTimeout
        }
        let nanoSeconds = Int64(timeout * Double(NSEC_PER_SEC))
        let time = DispatchTime.now() + Double(nanoSeconds) / Double(NSEC_PER_SEC)
        self.delegateQueue.asyncAfter(deadline: time, execute: {[weak self] in
            guard let strongSelf = self else {
                return
            }
            
            if strongSelf.sendItems[item.tag] == item {
                if item.status == .waiting {
                    item.status = .waitTimeouted
                    item.timeoutClosure?(strongSelf, item)
                    item.timeoutClosure = nil
                    item.successClosure = nil
                    item.failureClosure = nil
                    
                    if let index = strongSelf.itemQueue.index(of: item) {
                        strongSelf.itemQueue.remove(at: index)
                    }
                    strongSelf.sendItems.removeValue(forKey: item.tag)
                }else {
                    item.timeouted = true
                }
            }
        })
        _setNeedWrite()
        return item
    }
    
    //default timeout time = 300, min time = 3, max = 3600
    open func writeAfterConnect(_ data: Data, option: UInt32, waitingTimeout: TimeInterval = 300, successClosure: DataItemClosure? = nil, timeoutClosure: DataItemClosure? = nil, failureClosure: DataItemClosure? = nil) -> DataItem {
        var timeout: TimeInterval
        if waitingTimeout < 3 {
            timeout = 3
        }else if waitingTimeout > 3600 {
            timeout = 3600
        }else {
            timeout = waitingTimeout
        }
        return makeItemToConnect(.noTry, data: data, option: option, waitingTimeout: timeout, successClosure: successClosure, timeoutClosure: timeoutClosure, failureClosure: failureClosure)
    }
    
    //default timeout time = 300, min time = 3, max = 3600
    open func tryWriteConnectUntilTimeout(_ data: Data, option: UInt32, waitingTimeout: TimeInterval = 300, successClosure: DataItemClosure? = nil, timeoutClosure: DataItemClosure? = nil) -> DataItem {
        var timeout: TimeInterval
        if waitingTimeout < 3 {
            timeout = 3
        }else if waitingTimeout > 3600 {
            timeout = 3600
        }else {
            timeout = waitingTimeout
        }
        return makeItemToConnect(.tryUntilTimeout, data: data, option: option, waitingTimeout: timeout, successClosure: successClosure, timeoutClosure: timeoutClosure, failureClosure: nil)
    }
    
    fileprivate func makeItemToBeInteracted(_ tryType: DataItem.TryType, data: Data, option: UInt32, waitingTimeout: TimeInterval, successClosure: DataItemClosure?, timeoutClosure: DataItemClosure?, failureClosure: DataItemClosure?) -> DataItem {
        let itemOption = UInt64(option) | HighPermission.afterConnected.rawValue | HighPermission.afterInteracted.rawValue

        let item = DataItem(data: data, tryType: tryType, permissionOption: itemOption)
        item.status = .waiting
        var tag = tagSequence.nextSequence()
        
        while sendItems[tag] != nil {
            tag = tagSequence.nextSequence()
        }
        
        item.tag = tag
        item.successClosure = successClosure
        item.timeoutClosure = timeoutClosure
        item.failureClosure = failureClosure
        
        itemQueue.append(item)
        
        var timeout: TimeInterval
        if waitingTimeout < 3 {
            timeout = 3
        }else if waitingTimeout > 3600 {
            timeout = 3600
        }else {
            timeout = waitingTimeout
        }
        let nanoSeconds = Int64(timeout * Double(NSEC_PER_SEC))
        let time = DispatchTime.now() + Double(nanoSeconds) / Double(NSEC_PER_SEC)
        self.delegateQueue.asyncAfter(deadline: time, execute: {[weak self] in
            guard let strongSelf = self else {
                return
            }
            
            if strongSelf.sendItems[item.tag] == item {
                if item.status == .waiting {
                    item.status = .waitTimeouted
                    item.timeoutClosure?(strongSelf, item)
                    item.timeoutClosure = nil
                    item.successClosure = nil
                    item.failureClosure = nil
                    
                    if let index = strongSelf.itemQueue.index(of: item) {
                        strongSelf.itemQueue.remove(at: index)
                    }
                    strongSelf.sendItems.removeValue(forKey: item.tag)
                }else {
                    item.timeouted = true
                }
            }
            })
        _setNeedWrite()
        return item
    }
    
    //default timeout time = 300, min time = 3, max = 3600
    open func writeAfterBeInteracted(_ data: Data, option: UInt32,  waitingTimeout: TimeInterval = 300, successClosure: DataItemClosure? = nil, timeoutClosure: DataItemClosure? = nil, failureClosure: DataItemClosure? = nil) -> DataItem {
        var timeout: TimeInterval
        if waitingTimeout < 3 {
            timeout = 3
        }else if waitingTimeout > 3600 {
            timeout = 3600
        }else {
            timeout = waitingTimeout
        }
        return makeItemToBeInteracted(.noTry, data: data, option: option, waitingTimeout: timeout, successClosure: successClosure, timeoutClosure: timeoutClosure, failureClosure: failureClosure)
    }
    
    //default timeout time = 300, min time = 3, max = 3600
    open func tryWriteAfterBeInteractedUntilTimeout(_ data: Data, option: UInt32, waitingTimeout: TimeInterval = 300, successClosure: DataItemClosure? = nil, timeoutClosure: DataItemClosure? = nil) -> DataItem {
        var timeout: TimeInterval
        if waitingTimeout < 3 {
            timeout = 3
        }else if waitingTimeout > 3600 {
            timeout = 3600
        }else {
            timeout = waitingTimeout
        }
        return makeItemToBeInteracted(.tryUntilTimeout, data: data, option: option, waitingTimeout: timeout, successClosure: successClosure, timeoutClosure: timeoutClosure, failureClosure: nil)
    }
    
    fileprivate func _setNeedWrite() {
        if status == .connected {
            _writeItem()
        }
    }
    
    fileprivate func _suitableItem() -> DataItem? {
        if self.status == .connected {
            for item in itemQueue {
                if item.status == DataItem.Status.waiting && totalPermissions.contentOption(item.permissionOption) {
                    return item
                }
            }
        }
        return nil
    }
    
    fileprivate func _writeItem() {
        while true {
            if let item = _suitableItem() {
                if let csocket = self.socket {
                    item.status = .sending
                    csocket.write(item.data, withTimeout: -1, tag: item.tag)
                    IMLog.info("socket: writedata:\(item.data.count) tag: \(item.tag) ")
                }
            }else {
                break
            }
        }
    }
    
    fileprivate func _socketDidConnect(_ sock: GCDAsyncSocket) {
        IMLog.info("connect success:\(self.ipAdr) port:\(self.port) 本地port:\(sock.localPort)")
        self.status = .connected
        self.delegate?.connectSuccess(self)
        self._setNeedWrite()
        sock.readData(withTimeout: -1, tag: 0)
    }
}

// MARK: -  Socket Delegate
extension SocketConnect: GCDAsyncSocketDelegate {
    
    public func socket(_ sock: GCDAsyncSocket!, didConnectToHost host: String!, port: UInt16) {
        if let _tlsSettings = tlsSettings {
            IMLog.info("socket:\(sock) host:\(host) port:\(port) 建联成功，开始ssl握手")
            sock.startTLS(_tlsSettings)
        }else {
            _socketDidConnect(sock)
        }
    }
    
    public func socketDidSecure(_ sock: GCDAsyncSocket!) {
        IMLog.info("socket:\(sock) ssl握手完成)")
        _socketDidConnect(sock)
    }
    
    public func socket(_ sock: GCDAsyncSocket!, didWriteDataWithTag tag: Int) {
        if let item = sendItems[tag] {
            item.status = .sendSuccess
            if let index = self.itemQueue.index(of: item) {
                self.itemQueue.remove(at: index)
            }
            if let index = self.itemQueue.index(of: item) {
                self.itemQueue.remove(at: index)
            }
            item.successClosure?(self, item)
            item.successClosure = nil
            item.timeoutClosure = nil
            item.failureClosure = nil
        }
        IMLog.info("socket:\(sock) didWriteDataWithTag) tag:\(tag)")
    }
    
    public func socket(_ sock: GCDAsyncSocket!, didRead data: Data!, withTag tag: Int) {
        IMLog.info("socket:\(sock) didReadData:  dataLen\(data.count) tag:\(tag))")
        self.delegate?.connectDidReceiveData(self, data: data)
        sock.readData(withTimeout: -1, tag: 0)
    }
    public func socketDidDisconnect(_ sock: GCDAsyncSocket!, withError err: Error?) {
        if self.status == .connected {
            IMLog.info("socket:\(sock) socketDidDisconnect err:\(err))")
            self.status = .disconnect
            self.delegate?.connectClosed(self, error: err as NSError?)
        }else {
            IMLog.info("socket:\(sock) connect failure err:\(err))")
            var error: NSError
            if let _err = err as? NSError {
                error = _err
            }else {
                error = NSError(domain: "未知错误", code: 0, userInfo: nil)
            }
            self.status = .disconnect
            self.checkOnceDataItems()
            self.delegate?.connectFailure(self, error: error)
        }
    }
    
    public func readOnce() {
        socket?.readData(withTimeout: -1, tag: 0)
    }
    
}















