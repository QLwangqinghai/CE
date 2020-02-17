//
//  DataBase.swift
//  Sqlite
//
//  Created by wangqinghai on 2017/12/15.
//  Copyright © 2017年 wangqinghai. All rights reserved.
//

import Foundation
import SQLite3

//http://blog.csdn.net/sukhoi27smk/article/details/41895849 字段对应关系

//thread unsafe
typealias SQLite3BusyCallBack_f = @convention(c) (UnsafeMutableRawPointer?, Int32) -> Int32

let databaseBusyHandler: SQLite3BusyCallBack_f = { (contentPtr, count) -> Int32 in
    guard let content = contentPtr else {
        return 0
    }
    let uContainer = Unmanaged<DatabaseContainer>.fromOpaque(content)
    let container = uContainer.takeRetainedValue()
    guard let database = container.database else {
        uContainer.release()
        return 0
    }
    if count == 0 {
        container.startBusyRetryTime = ProcessInfo.processInfo.systemUptime
        uContainer.release()
        return 1
    }
    let delta = ProcessInfo.processInfo.systemUptime - container.startBusyRetryTime
    if delta < database.maxBusyRetryTimeInterval {
        let requestedSleepInMillseconds: Int32 = 50
        let actualSleepInMilliseconds = sqlite3_sleep(requestedSleepInMillseconds)//int requestedSleepInMillseconds = (int) arc4random_uniform(50) + 50;
        if requestedSleepInMillseconds != actualSleepInMilliseconds {
            print("WARNING: Requested sleep of \(requestedSleepInMillseconds) milliseconds, but SQLite returned actualSleepInMilliseconds. Maybe SQLite wasn't built with HAVE_USLEEP=1?");
        }
        uContainer.release()
        return 1
    } else {
        uContainer.release()
        return 0
    }
}

public protocol DatabaseLogProtocol: class {
    var logLevel: Database.LogLevel { get }
    func log(database: Database, time: TimeInterval, level: Database.LogLevel, _ message: Any)
}

public protocol SqliteDataProtocol {
    var sqliteDataPtr: UnsafeRawPointer { get }
    var sqliteDataLength: Int32 { get }
}
public protocol SqliteTextProtocol {
    var sqliteStringPtr: UnsafePointer<Int8> { get }
}

extension NSData: SqliteDataProtocol {
    public var sqliteDataPtr: UnsafeRawPointer {
        return self.bytes
    }
    public var sqliteDataLength: Int32 {
        return Int32(truncating: NSNumber(value: self.length))
    }
}

extension NSString: SqliteTextProtocol {
    private static let sqlNullString: NSString = ""
    
    public var sqliteStringPtr: UnsafePointer<Int8> {
        if let ptr = self.cString(using: String.Encoding.utf8.rawValue) {
            return ptr
        } else {
            return NSString.sqlNullString.cString(using: String.Encoding.utf8.rawValue)!
        }
    }
}

public enum SqliteValue {
    case null
    case double(Double)
    case int32(Int32)
    case int64(Int64)
    case data(SqliteDataProtocol)
    case text(SqliteTextProtocol)
    
    public init(_ value: Double?) {
        if let v = value {
            self = .double(v)
        } else {
            self = .null
        }
    }
    public init(_ value: Int32?) {
        if let v = value {
            self = .int32(v)
        } else {
            self = .null
        }
    }
    public init(_ value: Int64?) {
        if let v = value {
            self = .int64(v)
        } else {
            self = .null
        }
    }
    public init(_ value: String?) {
        if let v = value {
            self = .text(v as NSString)
        } else {
            self = .null
        }
    }
    public init(_ value: Data?) {
        if let v = value {
            self = .data(v as NSData)
        } else {
            self = .null
        }
    }
    public init(_ value: NSString?) {
        if let v = value {
            self = .text(v)
        } else {
            self = .null
        }
    }
    public init(_ value: NSData?) {
        if let v = value {
            self = .data(v)
        } else {
            self = .null
        }
    }
}

public extension SqliteValue {
    public init(_ value: UInt32?) {
        if let v = value {
            self = .int32(Int32(bitPattern: v))
        } else {
            self = .null
        }
    }
    public init(_ value: UInt64?) {
        if let v = value {
            self = .int64(Int64(bitPattern:v))
        } else {
            self = .null
        }
    }
}


fileprivate final class DatabaseQueueManager {
    private static let share: DatabaseQueueManager = DatabaseQueueManager()

    private static let specificKey: DispatchSpecificKey<DispatchQueue> = DispatchSpecificKey<DispatchQueue>()
    let lock: NSLock
    var queueMap: [String: DispatchQueue] = [:]
    init() {
        self.lock = NSLock()
    }
    func queue(_ key: String) -> DispatchQueue {
        self.lock.lock()
        var queue: DispatchQueue
        if let value = self.queueMap[key] {
            queue = value
        } else {
            queue = DispatchQueue(label: key, qos: DispatchQoS.userInteractive)
            queue.setSpecific(key: DatabaseQueueManager.specificKey, value: queue)
            self.queueMap[key] = queue
        }
        self.lock.unlock()
        return queue
    }
    static func queue(_ key: String) -> DispatchQueue {
        return DatabaseQueueManager.share.queue(key)
    }
    static func isInQueue(_ queue: DispatchQueue) -> Bool {
        return DispatchQueue.getSpecific(key: DatabaseQueueManager.specificKey) == queue
    }
}


public final class Database {
    private static let memoryPath = ":memory:"
    
    fileprivate class Context {
        fileprivate var statement: Statement
        fileprivate var database: Database
        fileprivate init(statement: Statement, database: Database) {
            self.statement = statement
            self.database = database
        }
    }
    
    public class QueryResultSet {
        fileprivate var context: Context?
        public lazy var columnMap: [String: Int32] = {
            guard let context = self.context else {
                return [:]
            }
            
            let columnCount: Int32 = sqlite3_column_count(context.statement.cStatement)
            var map: [String: Int32] = [String: Int32](minimumCapacity: Int(columnCount))
            
            var columnIdx: Int32 = 0
            while columnIdx < columnCount {
                map[String(cString: sqlite3_column_name(context.statement.cStatement, columnIdx)).lowercased()] = columnIdx
                columnIdx += 1
            }
            return map
        }()

        fileprivate init(context: Context) {
            self.context = context
        }
        
        func closeItem() {
            guard let context = self.context else {
                return
            }
            sqlite3_reset(context.statement.cStatement)
        }

        
        public func next() throws -> Bool {
            guard let context = self.context else {
                throw Database.invalidSetError
            }
            let stepResultCode = sqlite3_step(context.statement.cStatement)
            if SQLITE_BUSY == stepResultCode {
                let message = "Database busy (\(context.database.sqlitePath)): SQLITE_BUSY "
                context.database.log(Database.LogLevel.error, message)
            } else if SQLITE_LOCKED == stepResultCode {
                let message = "Database busy (\(context.database.sqlitePath)): SQLITE_LOCKED "

                context.database.log(Database.LogLevel.error, message)
            } else if (SQLITE_DONE == stepResultCode || SQLITE_ROW == stepResultCode) {
            } else if (SQLITE_ERROR == stepResultCode) {
                let message = "Error calling sqlite3_step (\(context.database.sqlitePath)): SQLITE_ERROR \(context.database.lastErrorMessage())"

                context.database.log(Database.LogLevel.error, message)
            } else if (SQLITE_MISUSE == stepResultCode) {
                let message = "Error calling sqlite3_step (\(context.database.sqlitePath)): SQLITE_MISUSE \(context.database.lastErrorMessage())"

                context.database.log(Database.LogLevel.error, message)
            } else {
                let message = "Unknown error calling sqlite3_step (code(\(stepResultCode): \(context.database.lastErrorMessage())"
                context.database.log(Database.LogLevel.error, message)
            }
        
            if SQLITE_ROW != stepResultCode {
                closeItem()
            }
            return (stepResultCode == SQLITE_ROW);
        }
        
        public func columnCount() throws -> Int32 {
            guard let context = self.context else {
                throw Database.invalidSetError
            }
            return sqlite3_column_count(context.statement.cStatement)
        }
        
        func hasAnotherRow() throws -> Bool {
            guard let context = self.context else {
                throw Database.invalidSetError
            }

            guard let handle = context.database.sqliteHandle else {
                return false
            }
            
            return sqlite3_errcode(handle) == SQLITE_ROW
        }
        
        func columnIndex(_ name: String) -> Int32? {
            return self.columnMap[name.lowercased()]
        }

        public func int64Value(for column: String) throws -> Int64? {
            guard let context = self.context else {
                throw Database.invalidSetError
            }
            guard let index = self.columnIndex(column) else {
                return nil
            }
            return sqlite3_column_int64(context.statement.cStatement, index)
        }
        
        public func int32Value(for column: String) throws -> Int32? {
            guard let context = self.context else {
                throw Database.invalidSetError
            }
            guard let index = self.columnIndex(column) else {
                return nil
            }
            return sqlite3_column_int(context.statement.cStatement, index)
        }
        
        public func uint64Value(for column: String) throws -> UInt64? {
            guard let context = self.context else {
                throw Database.invalidSetError
            }
            guard let index = self.columnIndex(column) else {
                return nil
            }
            return UInt64(bitPattern: sqlite3_column_int64(context.statement.cStatement, index))
        }
        
        public func uint32Value(for column: String) throws -> UInt32? {
            guard let context = self.context else {
                throw Database.invalidSetError
            }
            guard let index = self.columnIndex(column) else {
                return nil
            }
            return UInt32(bitPattern: sqlite3_column_int(context.statement.cStatement, index))
        }
        
        public func doubleValue(for column: String) throws -> Double? {
            guard let context = self.context else {
                throw Database.invalidSetError
            }
            guard let index = self.columnIndex(column) else {
                return nil
            }
            return sqlite3_column_double(context.statement.cStatement, index)
        }
        public func stringValue(for column: String) throws -> String? {
            guard let context = self.context else {
                throw Database.invalidSetError
            }

            guard let index = self.columnIndex(column) else {
                return nil
            }
            if sqlite3_column_type(context.statement.cStatement, index) == SQLITE_NULL {
                return nil
            }
            guard let cStr = sqlite3_column_text(context.statement.cStatement, index) else {
                return nil
            }
            return String(cString: cStr)
        }
        public func dataValue(for column: String) throws -> Data? {
            guard let context = self.context else {
                throw Database.invalidSetError
            }

            guard let index = self.columnIndex(column) else {
                return nil
            }
            if sqlite3_column_type(context.statement.cStatement, index) == SQLITE_NULL {
                return nil
            }
            guard let dataBuffer = sqlite3_column_blob(context.statement.cStatement, index) else {
                return nil
            }
            let dataSize = sqlite3_column_bytes(context.statement.cStatement, index)
            return Data(bytes: dataBuffer, count: Int(dataSize))
        }
        
        public struct SqliteData {
            public let dataBuffer: UnsafeRawPointer
            public let size: Int
            
        }
        public func dataValue<T>(for column: String, handler: (SqliteData?) throws -> T) throws -> T  {
            guard let context = self.context else {
                throw Database.invalidSetError
            }
            guard let index = self.columnIndex(column) else {
                return try handler(nil)
            }
            if sqlite3_column_type(context.statement.cStatement, index) == SQLITE_NULL {
                return try handler(nil)
            }
            guard let dataBuffer = sqlite3_column_blob(context.statement.cStatement, index) else {
                return try handler(nil)
            }
            let dataSize = sqlite3_column_bytes(context.statement.cStatement, index)
            let data = SqliteData(dataBuffer: dataBuffer, size: Int(dataSize))
            return try handler(data)
        }
        
        public struct SqliteCString {
            public let dataBuffer: UnsafeRawPointer
        }
        public func cStringValue<T>(for column: String, handler: (SqliteCString?) throws -> T) throws -> T  {
            guard let context = self.context else {
                throw Database.invalidSetError
            }

            guard let index = self.columnIndex(column) else {
                return try handler(nil)
            }
            if sqlite3_column_type(context.statement.cStatement, index) == SQLITE_NULL {
                return try handler(nil)
            }
            guard let dataBuffer = sqlite3_column_text(context.statement.cStatement, index) else {
                return try handler(nil)
            }
            let cString = SqliteCString(dataBuffer: dataBuffer)
            return try handler(cString)
        }
        fileprivate func columnIndexIsNull(columnIdx: Int32, context: Context) -> Bool {
            return sqlite3_column_type(context.statement.cStatement, columnIdx) == SQLITE_NULL
        }
        
        public func columnIsNull(columnName: String) throws -> Bool {
            guard let context = self.context else {
                throw Database.invalidSetError
            }
            guard let index = self.columnMap[columnName] else {
                return false
            }
            return self.columnIndexIsNull(columnIdx: index, context: context)
        }
    }
    
    public class ManipulateResultSet {
        fileprivate var context: Context?

//        public lazy var columnMap: [String: Int32] = {
//            let columnCount: Int32 = sqlite3_column_count(statement.cStatement)
//            var map: [String: Int32] = [String: Int32](minimumCapacity: Int(columnCount))
//
//            var columnIdx: Int32 = 0
//            while columnIdx < columnCount {
//                map[String(cString: sqlite3_column_name(statement.cStatement, columnIdx)).lowercased()] = columnIdx
//                columnIdx += 1
//            }
//            return map
//        }()
        
        fileprivate init(context: Context) {
            self.context = context
        }

        
//        func closeItem() {
//            sqlite3_reset(self.statement.cStatement)
//        }
//
//        public func next() throws -> Bool {
//            let stepResultCode = sqlite3_step(self.statement.cStatement)
//            if SQLITE_BUSY == stepResultCode {
//                let message = "Database busy (\(self.database.sqlitePath)): SQLITE_BUSY "
//                database.log(Database.LogLevel.error, message)
//            } else if SQLITE_LOCKED == stepResultCode {
//                let message = "Database busy (\(self.database.sqlitePath)): SQLITE_LOCKED "
//
//                database.log(Database.LogLevel.error, message)
//            } else if (SQLITE_DONE == stepResultCode || SQLITE_ROW == stepResultCode) {
//            } else if (SQLITE_ERROR == stepResultCode) {
//                let message = "Error calling sqlite3_step (\(self.database.sqlitePath)): SQLITE_ERROR \(database.lastErrorMessage())"
//
//                database.log(Database.LogLevel.error, message)
//            } else if (SQLITE_MISUSE == stepResultCode) {
//                let message = "Error calling sqlite3_step (\(self.database.sqlitePath)): SQLITE_MISUSE \(database.lastErrorMessage())"
//
//                database.log(Database.LogLevel.error, message)
//            } else {
//                let message = "Unknown error calling sqlite3_step (code(\(stepResultCode): \(database.lastErrorMessage())"
//                database.log(Database.LogLevel.error, message)
//            }
//
//            if SQLITE_ROW != stepResultCode {
//                closeItem()
//            }
//            return (stepResultCode == SQLITE_ROW);
//        }
//
//        public func columnCount() -> Int32 {
//            return sqlite3_column_count(self.statement.cStatement)
//        }
//
//        func hasAnotherRow() -> Bool {
//            guard let handle = database.sqliteHandle else {
//                return false
//            }
//
//            return sqlite3_errcode(handle) == SQLITE_ROW
//        }
//
//        func columnIndex(_ name: String) -> Int32? {
//            return self.columnMap[name.lowercased()]
//        }
//
//
//        public func int64Value(for column: String) -> Int64? {
//            guard let index = self.columnIndex(column) else {
//                return nil
//            }
//            return sqlite3_column_int64(self.statement.cStatement, index)
//        }
//
//        public func int32Value(for column: String) -> Int32? {
//            guard let index = self.columnIndex(column) else {
//                return nil
//            }
//            return sqlite3_column_int(self.statement.cStatement, index)
//        }
//        public func doubleValue(for column: String) -> Double? {
//            guard let index = self.columnIndex(column) else {
//                return nil
//            }
//            return sqlite3_column_double(self.statement.cStatement, index)
//        }
//        public func stringValue(for column: String) -> String? {
//            guard let index = self.columnIndex(column) else {
//                return nil
//            }
//            if sqlite3_column_type(self.statement.cStatement, index) == SQLITE_NULL {
//                return nil
//            }
//            guard let cStr = sqlite3_column_text(self.statement.cStatement, index) else {
//                return nil
//            }
//            return String(cString: cStr)
//        }
//        public func dataValue(for column: String) -> Data? {
//            guard let index = self.columnIndex(column) else {
//                return nil
//            }
//            if sqlite3_column_type(self.statement.cStatement, index) == SQLITE_NULL {
//                return nil
//            }
//            guard let dataBuffer = sqlite3_column_blob(self.statement.cStatement, index) else {
//                return nil
//            }
//            let dataSize = sqlite3_column_bytes(self.statement.cStatement, index)
//            return Data(bytes: dataBuffer, count: Int(dataSize))
//        }
//
//        public struct SqliteData {
//            public let dataBuffer: UnsafeRawPointer
//            public let size: Int
//
//        }
//        public func dataValue<T>(for column: String, handler: (SqliteData?) -> T) -> T  {
//            guard let index = self.columnIndex(column) else {
//                return handler(nil)
//            }
//            if sqlite3_column_type(self.statement.cStatement, index) == SQLITE_NULL {
//                return handler(nil)
//            }
//            guard let dataBuffer = sqlite3_column_blob(self.statement.cStatement, index) else {
//                return handler(nil)
//            }
//            let dataSize = sqlite3_column_bytes(self.statement.cStatement, index)
//            let data = SqliteData(dataBuffer: dataBuffer, size: Int(dataSize))
//            return handler(data)
//        }
//
//        public struct SqliteCString {
//            public let dataBuffer: UnsafeRawPointer
//
//        }
//        public func cStringValue<T>(for column: String, handler: (SqliteCString?) -> T) -> T  {
//            guard let index = self.columnIndex(column) else {
//                return handler(nil)
//            }
//            if sqlite3_column_type(self.statement.cStatement, index) == SQLITE_NULL {
//                return handler(nil)
//            }
//            guard let dataBuffer = sqlite3_column_text(self.statement.cStatement, index) else {
//                return handler(nil)
//            }
//            let cString = SqliteCString(dataBuffer: dataBuffer)
//            return handler(cString)
//        }
//
//        public func columnIndexIsNull(columnIdx: Int32) -> Bool {
//            return sqlite3_column_type(self.statement.cStatement, columnIdx) == SQLITE_NULL
//        }
//
//        public func columnIsNull(columnName: String) -> Bool {
//            guard let index = self.columnMap[columnName] else {
//                return false
//            }
//            return self.columnIndexIsNull(columnIdx: index)
//        }
    }

    
    fileprivate class Statement {
        fileprivate var sql: String
        fileprivate private(set) var cStatement: OpaquePointer!
        fileprivate private(set) var isClosed: Bool = false

        fileprivate init(sql: String, statement: OpaquePointer) {
            self.sql = sql
            self.cStatement = statement
        }
        fileprivate func step() -> Int32 {
            return sqlite3_step(cStatement)
        }
        @discardableResult fileprivate func reset() -> Int32 {
            return sqlite3_reset(cStatement)
        }
        fileprivate func close(_ database: Database) {
            if !self.isClosed {
                let closeErrorCode = sqlite3_finalize(cStatement)
                if SQLITE_OK != closeErrorCode {
                    database.log(Database.LogLevel.error, "Unknown error finalizing or resetting statement (\(closeErrorCode): \(database.lastError())")
                    database.log(Database.LogLevel.error, "DB Query: \(sql)")
                }
                
                cStatement = nil
                self.isClosed = true
            }
        }
        
        private func close() {
            if !self.isClosed {
                sqlite3_finalize(cStatement)
                cStatement = nil
                self.isClosed = true
            }
        }
        
        func bind(_ value: SqliteValue, at column: Int32, database: Database) throws {
            guard let statement = self.cStatement else {
                throw NSError()
            }
            
            func checkBindResult(_ code: Int32) throws {
                if code != SQLITE_OK {
                    print("code: \(code)  error: \(database.lastError())")
                    
                }
            }
            
            var bindResult: Int32 = 0
            
            switch value {
            case .null:
                bindResult = sqlite3_bind_null(statement, column)
                break
            case .int32(let content):
                bindResult = sqlite3_bind_int(statement, column, content)
                break
            case .int64(let content):
                bindResult = sqlite3_bind_int64(statement, column, content)
                break
            case .double(let content):
                bindResult = sqlite3_bind_double(statement, column, content)
                break
            case .text(let content):
                bindResult = sqlite3_bind_text(statement, column, content.sqliteStringPtr, -1, nil)
                break
            case .data(let content):
                if content.sqliteDataLength > 0 {
                    bindResult = sqlite3_bind_blob(statement, column, content.sqliteDataPtr, content.sqliteDataLength, nil)
                } else {
                    bindResult = sqlite3_bind_null(statement, column)
                }
                break
            }
            
            try checkBindResult(bindResult)
        }
        
        deinit {
            self.close()
        }
    }

    public final class Logger: DatabaseLogProtocol, Equatable {
        public private(set) var logLevel: LogLevel = .error
        
        public func log(database: Database, time: TimeInterval, level: LogLevel, _ message: Any) {
            if level <= self.logLevel {
                print(message)
            }
        }
        
        public init(logLevel: LogLevel) {
            self.logLevel = logLevel
        }
        
        public static func == (lhs: Logger, rhs: Logger) -> Bool {
            return lhs === rhs
        }
    }
    
    public struct Error: Swift.Error {
        public enum Code {
            case notOpen
            case invalidSet
            case sqliteError(Int32)
        }
        
        public let code: Code
        public let message: String

        public init(code: Code, message: String) {
            self.code = code
            self.message = message
        }
    }
    
    public enum LogLevel: UInt {
        case error = 1
        case info = 2
        case debug = 3
        case verbose = 4
        
        public static func <= (lhs: LogLevel, rhs: LogLevel) -> Bool {
            return lhs.rawValue <= rhs.rawValue
        }
        public static func < (lhs: LogLevel, rhs: LogLevel) -> Bool {
            return lhs.rawValue < rhs.rawValue
        }
        public static func >= (lhs: LogLevel, rhs: LogLevel) -> Bool {
            return lhs.rawValue >= rhs.rawValue
        }
        public static func > (lhs: LogLevel, rhs: LogLevel) -> Bool {
            return lhs.rawValue > rhs.rawValue
        }
    }
    public static let defaultLoggerKey = "Database.defaultLoggerKey"
    public var defaultLogLevel: LogLevel?  {
        didSet(oldValue) {
            if let level = defaultLogLevel {
                self.loggers[Database.defaultLoggerKey] = Logger(logLevel: level)
            } else {
                self.loggers.removeValue(forKey: Database.defaultLoggerKey)
            }
        }
    }
    private var cachedStatements: [String: Statement] = [:]
    public let path: String
    public private(set) var sqliteHandle: OpaquePointer?
    public private(set) var inTransaction: Bool = false
    private var loggers: [String: DatabaseLogProtocol] = [:]
    public private(set) var cacheStatementsable: Bool
    fileprivate let sqlitePath: (UnsafeMutablePointer<Int8>, Int)
    fileprivate var container: Unmanaged<DatabaseContainer> = Unmanaged<DatabaseContainer>.passRetained(DatabaseContainer()).retain()

    public var maxBusyRetryTimeInterval: TimeInterval = 1 {
        didSet {
            configBusyHandler()
        }
    }
    
    private func configBusyHandler() {
        guard let handle = self.sqliteHandle else {
            return
        }
        if (maxBusyRetryTimeInterval > 0) {
            sqlite3_busy_handler(handle, databaseBusyHandler, self.container.toOpaque())
        } else {
            sqlite3_busy_handler(handle, nil, nil)
        }
    }
    
    public init(path: String?, cacheStatementsable: Bool = true) {
        self.cacheStatementsable = cacheStatementsable
        if let pathValue = path {
            if pathValue == "" {
                self.sqlitePath = pathValue.withCString({ (cString) -> (UnsafeMutablePointer<Int8>, Int) in
                    let length = strlen(cString) + 1
                    let cPath = UnsafeMutablePointer<Int8>.allocate(capacity: length)
                    
                    memcpy(cPath, cString, length)
                    return (cPath, length)
                })
            } else {
                let cString = (pathValue as NSString).fileSystemRepresentation
                let length = strlen(cString) + 1
                let cPath = UnsafeMutablePointer<Int8>.allocate(capacity: length)
                memcpy(cPath, cString, length)
                self.sqlitePath = (cPath, length)
            }
            self.path = pathValue
        } else {
            self.path = Database.memoryPath
            self.sqlitePath = Database.memoryPath.withCString({ (cString) -> (UnsafeMutablePointer<Int8>, Int) in
                let length = strlen(cString) + 1
                let cPath = UnsafeMutablePointer<Int8>.allocate(capacity: length)
                
                memcpy(cPath, cString, length)
                return (cPath, length)
            })
        }
        
        self.loggers[Database.defaultLoggerKey] = Logger(logLevel: .error)
        self.defaultLogLevel = .error
        
        self.container.takeUnretainedValue().database = self
    }
    
    deinit {
        _ = self.close()
        self.sqlitePath.0.deallocate(capacity: self.sqlitePath.1)
        self.container.release()
    }

    public static var sqliteLibVersion: String {
        return String(cString: sqlite3_libversion())
    }
    public static var isThreadSafe: Bool {
        return sqlite3_threadsafe() != 0
    }
    public var queue: DispatchQueue {
        return DatabaseQueueManager.queue(self.path)
    }
    
    private func cache(statement: Statement, forSql: String) {
        if self.cacheStatementsable && !statement.isClosed {
            self.cachedStatements[forSql] = statement;
        }
    }
    private func cachedStatement(forSql: String) -> Statement? {
        guard self.cacheStatementsable else {
            return nil
        }
        guard let statement = self.cachedStatements[forSql] else {
            return nil
        }
        if statement.isClosed {
            self.cachedStatements.removeValue(forKey: forSql)
            return nil
        } else {
            return statement
        }
    }
    
    
    public func enQueue<T>(_ work : (_ database: Database) throws -> T) rethrows -> T {
        if (DatabaseQueueManager.isInQueue(self.queue)) {
            return try work(self)
        } else {
            return try self.queue.sync { () -> T in
                return try work(self)
            }
        }
    }
    
    private func log(_ level: LogLevel, _ message: @autoclosure () -> Any) {
        let array = self.loggers.flatMap { (item) -> DatabaseLogProtocol? in
            if item.value.logLevel >= level {
                return item.value
            } else {
                return nil
            }
        }
        guard !array.isEmpty else {
            return
        }
        let time = CFAbsoluteTimeGetCurrent()
        let messageValue = message()
        _ = array.map { (logger) -> Void in
            logger.log(database: self, time: time, level: level, messageValue)
        }
    }
    

    public struct OpenFlag: OptionSet {
        public var rawValue: Int32
        public init(rawValue: Int32) {
            self.rawValue = rawValue
        }
        public static let readWrite: OpenFlag = OpenFlag(rawValue: SQLITE_OPEN_READWRITE)
        public static let readOnly: OpenFlag = OpenFlag(rawValue: SQLITE_OPEN_READONLY)
        public static let createIfNotExit: OpenFlag = OpenFlag(rawValue: SQLITE_OPEN_CREATE)
        public static let noMutex: OpenFlag = OpenFlag(rawValue: SQLITE_OPEN_NOMUTEX)
        public static let sharedCache: OpenFlag = OpenFlag(rawValue: SQLITE_OPEN_SHAREDCACHE)

//        static let Down: Directions = Directions(rawValue: 1 << 1)
//        static let Left: Directions = Directions(rawValue: 1 << 2)
//        static let Right: Directions = Directions(rawValue: 1 << 3)
    }
    
    /*
     sqlite3* pDb;
     char* filename="/u01/sqlite/test.db";
     sqlite3_open_v2(filename, &pDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
     ....
     ....
     sqlite3_close_v2(pDb);
     */
    public func open(flag: Int32, vfsName: String? = nil) -> Bool {
        if self.sqliteHandle != nil {
            return true
        } else {
            let error: Int32
            if let vfsNameValue = vfsName {
                error = vfsNameValue.withCString({ (cString) -> Int32 in
                    return sqlite3_open_v2(self.sqlitePath.0, &self.sqliteHandle, flag, cString)
                })
            } else {
                error = sqlite3_open_v2(self.sqlitePath.0, &self.sqliteHandle, flag, nil)
            }
            
            if error != SQLITE_OK {
                self.log(LogLevel.error, "error opening!: \(error)")
                return false
            } else {
                configBusyHandler()
                return true
            }
        }
    }
    
    //    func openWithFlags:(int)flags {
    //    return [self openWithFlags:flags vfs:nil];
    //    }
    //    - (BOOL)openWithFlags:(int)flags vfs:(NSString *)vfsName {
    //    #if SQLITE_VERSION_NUMBER >= 3005000
    //    if (_db) {
    //    return YES;
    //    }
    //
    //    int err = sqlite3_open_v2([self sqlitePath], (sqlite3**)&_db, flags, [vfsName UTF8String]);
    //    if(err != SQLITE_OK) {
    //    NSLog(@"error opening!: %d", err);
    //    return NO;
    //    }
    //
    //    if (_maxBusyRetryTimeInterval > 0.0) {
    //    // set the handler
    //    [self maxBusyRetryTimeInterval:_maxBusyRetryTimeInterval];
    //    }
    
    //    return YES;
    //    #else
    //    NSLog(@"openWithFlags requires SQLite 3.5");
    //    return NO;
    //    #endif
    //    }

    
    public func open() -> Bool {
        if self.sqliteHandle != nil {
            return true
        } else {
            let error = sqlite3_open(self.sqlitePath.0, &self.sqliteHandle)
            
            if error != SQLITE_OK {
                self.log(LogLevel.error, "error opening!: \(error)")
                return false
            } else {
                configBusyHandler()
                return true
            }
        }
    }
    public func close() -> Bool {
        clearCachedStatements()
        
        guard let handle = self.sqliteHandle else {
            return true
        }
        var retry = false
        var resultCode: Int32
        var triedFinalizingOpenStatements: Bool = false
        repeat {
            resultCode = sqlite3_close_v2(handle)
            if SQLITE_BUSY == resultCode || SQLITE_LOCKED == resultCode {
                if !triedFinalizingOpenStatements {
                    triedFinalizingOpenStatements = true
                    while let pStmt = sqlite3_next_stmt(handle, nil) {
                        self.log(Database.LogLevel.info, "Closing leaked statement")
                        sqlite3_finalize(pStmt)
                        retry = true
                    }
                }
            } else {
                self.log(Database.LogLevel.error, "error closing!: \(resultCode)")
            }
        } while retry
        self.sqliteHandle = nil
        return true
    }

    
//    func openWithFlags:(int)flags {
//    return [self openWithFlags:flags vfs:nil];
//    }
//    - (BOOL)openWithFlags:(int)flags vfs:(NSString *)vfsName {
//    #if SQLITE_VERSION_NUMBER >= 3005000
//    if (_db) {
//    return YES;
//    }
//
//    int err = sqlite3_open_v2([self sqlitePath], (sqlite3**)&_db, flags, [vfsName UTF8String]);
//    if(err != SQLITE_OK) {
//    NSLog(@"error opening!: %d", err);
//    return NO;
//    }
//
//    if (_maxBusyRetryTimeInterval > 0.0) {
//    // set the handler
//    [self maxBusyRetryTimeInterval:_maxBusyRetryTimeInterval];
//    }
    
//    return YES;
//    #else
//    NSLog(@"openWithFlags requires SQLite 3.5");
//    return NO;
//    #endif
//    }

//    - (BOOL)hasOpenResultSets {
//    return [_openResultSets count] > 0;
//    }

    
    func closeOpenResultSets() {
//    //Copy the set so we don't get mutation errors
//    NSSet *openSetCopy = FMDBReturnAutoreleased([_openResultSets copy]);
//    for (NSValue *rsInWrappedInATastyValueMeal in openSetCopy) {
//    FMResultSet *rs = (FMResultSet *)[rsInWrappedInATastyValueMeal pointerValue];
//
//    [rs setParentDB:nil];
//    [rs close];
//
//    [_openResultSets removeObject:rsInWrappedInATastyValueMeal];
//    }
//    }
//
//    - (void)resultSetDidClose:(FMResultSet *)resultSet {
//    NSValue *setValue = [NSValue valueWithNonretainedObject:resultSet];
//
//    [_openResultSets removeObject:setValue];
    }


    func clearCachedStatements() {
        for item in self.cachedStatements {
            item.value.close(self)
        }
        self.cachedStatements.removeAll()
    }


    public let noOpenedError: Database.Error = {
        return Database.Error(code: Database.Error.Code.notOpen, message: "database not open")
    }()
    public static let invalidSetError: Database.Error = {
        return Database.Error(code: Database.Error.Code.invalidSet, message: "invalidSet")
    }()
    public func goodConnection() -> Bool {
        guard let _ = self.sqliteHandle else {
            return false
        }
        do {
            return try self.query(sql: "select name from sqlite_master where type='table'", values: []) { (resultSet) -> Bool in
                return true
            }
        } catch let error {
            print(error)
            return false
        }
    }

    public func databaseExists() -> Bool {
        if self.sqliteHandle == nil {
            self.log(Database.LogLevel.error, "The Database \(self) is not open.")
            return false
        }
        return true
    }

    func lastErrorMessage() -> String {
        guard let handle = self.sqliteHandle else {
            return ""
        }
        return String(cString: sqlite3_errmsg(handle))
    }
    
    public func hadError() -> Bool {
        let lastErrCode = lastErrorCode()
        return (lastErrCode > SQLITE_OK && lastErrCode < SQLITE_ROW)
    }
    
    public func lastErrorCode() -> Int32 {
        return sqlite3_errcode(self.sqliteHandle);
    }

    public func lastError() -> Error {
        return Database.Error(code: Database.Error.Code.sqliteError(self.lastErrorCode()), message: self.lastErrorMessage())
    }
    public func lastInsertRowId() -> Int64? {
        guard let handle = self.sqliteHandle else {
            return nil
        }
        return sqlite3_last_insert_rowid(handle)
    }
    public func changes() -> Int32? {
        guard let handle = self.sqliteHandle else {
            return nil
        }
        return sqlite3_changes(handle)
    }
    
    //Data Definition Language, like create、 drop
    public func define(sql: String) throws {
        guard let handle = self.sqliteHandle else {
            throw self.noOpenedError
        }
        var pStmt: OpaquePointer?
        
        let resultCode: Int32 = sql.withCString { (sqlCString) -> Int32 in
            return sqlite3_prepare_v2(handle, sqlCString, -1, &pStmt, nil);
        }
        
        if SQLITE_OK != resultCode {
            let error = self.lastError()
            self.log(Database.LogLevel.error, "DB Define Error, error: \(error), sql: \(sql), path: \(self.path)")
            if let stmt = pStmt {
                sqlite3_finalize(stmt)
            }
            throw error
        }
        guard let stmt = pStmt else {
            let error = self.lastError()
            self.log(Database.LogLevel.error, "DB Define Error, error: \(error), sql: \(sql), path: \(self.path)")
            throw error
        }
        let stepResultCode = sqlite3_step(stmt)
        
        if SQLITE_DONE == stepResultCode {
            // all is well, let's return.
        } else if SQLITE_ROW == stepResultCode {
            let message = "A definition is being called with a query sql: '\(sql)'"
            let error = self.lastError()
            self.log(Database.LogLevel.error, "DB Manipulate Error, error: \(error), message: \(message), sql: \(sql), path: \(self.path)")
            throw self.lastError()
        } else {
            let message = String(cString: sqlite3_errmsg(handle))
            
            if SQLITE_ERROR == stepResultCode {
                self.log(Database.LogLevel.error, "Error calling sqlite3_step (SQLITE_ERROR: \(message) SQLITE_ERROR")
                self.log(Database.LogLevel.error, "DB Query: \(sql)");
            } else if SQLITE_MISUSE == stepResultCode {
                // uh oh.
                self.log(Database.LogLevel.error, "Error calling sqlite3_step (SQLITE_MISUSE: \(message) SQLITE_MISUSE")
                self.log(Database.LogLevel.error, "DB Query: \(sql)")
            } else {
                // wtf?
                self.log(Database.LogLevel.error, "Unknown error calling sqlite3_step (\(stepResultCode): \(message) eu")
                self.log(Database.LogLevel.error, "DB Query: \(sql)")
            }
            
            throw self.lastError()
        }
        
        let closeErrorCode = sqlite3_finalize(stmt);
        if SQLITE_OK != closeErrorCode {
            let message = String(cString: sqlite3_errmsg(handle))
            
            self.log(Database.LogLevel.error, "Unknown error finalizing or resetting statement (\(closeErrorCode): \(message)")
            self.log(Database.LogLevel.error, "DB Query: \(sql)")
        }
//        return (resultCode == SQLITE_DONE || resultCode == SQLITE_OK)
    }
    
    //Data Manipulation Language, like insert、update、delete
    public func manipulate<T>(sql: String, values: [SqliteValue], resultHandler: (ManipulateResultSet)-> T) throws -> T {
        guard let handle = self.sqliteHandle else {
            throw self.noOpenedError
        }
        
        var statement: Statement
        if let cached = self.cachedStatement(forSql: sql) {
            statement = cached
        } else {
            var pStmt: OpaquePointer?
            let resultCode: Int32 = sql.withCString { (sqlCString) -> Int32 in
                return sqlite3_prepare_v2(handle, sqlCString, -1, &pStmt, nil);
            }
            if SQLITE_OK != resultCode {
                let error = self.lastError()
                self.log(Database.LogLevel.error, "DB Manipulate Error, error: \(error), sql: \(sql), path: \(self.path)")
                if let stmt = pStmt {
                    sqlite3_finalize(stmt)
                }
                throw error
            }
            guard let stmt = pStmt else {
                let error = self.lastError()
                self.log(Database.LogLevel.error, "DB Manipulate Error, error: \(error), sql: \(sql), path: \(self.path)")
                throw error
            }
            statement = Statement(sql: sql, statement: stmt)
            self.cache(statement: statement, forSql: sql)
        }
        for item in values.enumerated() {
            try statement.bind(item.element, at: Int32(item.offset + 1), database: self)
        }
        
        let stepResultCode = statement.step()
        if SQLITE_DONE == stepResultCode {
            // all is well, let's return.
        } else if SQLITE_ROW == stepResultCode {
            let message = "A manipulation is being called with a query sql: '\(sql)'"
            let error = self.lastError()
            self.log(Database.LogLevel.error, "DB Manipulate Error, error: \(error), message: \(message), sql: \(sql), path: \(self.path)")
            throw self.lastError()
        } else {
            let message = String(cString: sqlite3_errmsg(handle))
            
            if SQLITE_ERROR == stepResultCode {
                self.log(Database.LogLevel.error, "Error calling sqlite3_step (SQLITE_ERROR: \(message) SQLITE_ERROR")
                self.log(Database.LogLevel.error, "DB Query: \(sql)");
            } else if SQLITE_MISUSE == stepResultCode {
                // uh oh.
                self.log(Database.LogLevel.error, "Error calling sqlite3_step (SQLITE_MISUSE: \(message) SQLITE_MISUSE")
                self.log(Database.LogLevel.error, "DB Query: \(sql)")
            } else {
                // wtf?
                self.log(Database.LogLevel.error, "Unknown error calling sqlite3_step (\(stepResultCode): \(message) eu")
                self.log(Database.LogLevel.error, "DB Query: \(sql)")
            }
            
            throw self.lastError()
        }
        let context: Context = Context(statement: statement, database: self)
        let resultSet = ManipulateResultSet(context: context)
        let result = resultHandler(resultSet)
        statement.reset()
        resultSet.context = nil
        return result
//        return (stepResultCode == SQLITE_DONE || stepResultCode == SQLITE_OK)
    }
    
    //Data Query Language, like select
    public func query<T>(sql: String, values: [SqliteValue], resultHandler: (QueryResultSet) throws ->  T) throws -> T {
        guard let handle = self.sqliteHandle else {
            throw self.noOpenedError
        }
        
        var statement: Statement
        if let cached = self.cachedStatement(forSql: sql) {
            statement = cached
        } else {
            var pStmt: OpaquePointer?
            let resultCode: Int32 = sql.withCString { (sqlCString) -> Int32 in
                return sqlite3_prepare_v2(handle, sqlCString, -1, &pStmt, nil);
            }
            
            if (SQLITE_OK != resultCode) {
                self.log(Database.LogLevel.error, "DB Error: \(self.lastErrorCode()) \"\(self.lastError())\"")
                self.log(Database.LogLevel.error, "DB Query: \(sql)")
                self.log(Database.LogLevel.error, "DB Path: \(self.path)")
                if let stmt = pStmt {
                    sqlite3_finalize(stmt);
                }
                throw self.lastError()
            }
            guard let stmt = pStmt else {
                let error = self.lastError()
                self.log(Database.LogLevel.error, "DB Query Error, error: \(error), sql: \(sql), path: \(self.path)")
                throw self.lastError()
            }
            statement = Statement(sql: sql, statement: stmt)
            self.cache(statement: statement, forSql: sql)
        }
        for item in values.enumerated() {
            try statement.bind(item.element, at: Int32(item.offset + 1), database: self)
        }
        
        let context: Context = Context(statement: statement, database: self)
        let resultSet = QueryResultSet(context: context)
        let result = try resultHandler(resultSet)
        resultSet.context = nil
        statement.reset()
        return result
    }
    
    public func rollback() throws -> Bool {
        let sql = "rollback transaction"
        let result = try self.manipulate(sql: sql, values: [], resultHandler: { (set) -> Bool in
            return  true
        })
        if result {
            self.inTransaction = false
        }
        return result
    }
    public func commit() throws -> Bool {
        let sql = "commit transaction"
        let result = try self.manipulate(sql: sql, values: [], resultHandler: { (set) -> Bool in
            return true
        })
        if result {
            self.inTransaction = false
        }
        return result
    }
    public func beginDeferredTransaction() throws -> Bool {
        let sql = "begin deferred transaction"

        let result = try self.manipulate(sql: sql, values: [], resultHandler: { (set) -> Bool in
            return true
        })
        if result {
            self.inTransaction = true
        }
        return result
    }
    public func beginTransaction() throws -> Bool {
        let sql = "begin exclusive transaction"
        let result = try self.manipulate(sql: sql, values: [], resultHandler: { (set) -> Bool in
            return true
        })
        if result {
            self.inTransaction = true
        }
        return result
    }
}


fileprivate final class DatabaseContainer {
    fileprivate weak var database: Database? = nil
    fileprivate var startBusyRetryTime: TimeInterval = 0
    
    fileprivate init() {
    }
    
}

