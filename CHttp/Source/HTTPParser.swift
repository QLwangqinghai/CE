//
//  HTTPParser.swift
//  CHttp
//
//  Created by vector on 2019/7/9.
//  Copyright © 2019 vector. All rights reserved.
//

import Foundation
import CNIOHTTPParser


private extension UnsafeMutablePointer where Pointee == http_parser {
    /// Returns the `KeepAliveState` for the current message that is parsed.
    var keepAliveState: KeepAliveState {
        return c_nio_http_should_keep_alive(self) == 0 ? .close : .keepAlive
    }
}

private enum HTTPDecodingState {
    case beforeMessageBegin
    case afterMessageBegin
    case url
    case headerName
    case headerValue
    case trailerName
    case trailerValue
    case headersComplete
}

// Keep track of keep alive state.
internal enum KeepAliveState {
    // We know keep alive should be used.
    case keepAlive
    // We know we should close the connection.
    case close
    // We need to scan the headers to find out if keep alive is used or not
    case unknown
}


public enum HTTPDecoderKind {
    case request
    case response
}


private class BetterHTTPParser {
    var delegate: HTTPDecoderDelegate! = nil
    private var parser: http_parser? = http_parser() // nil if unaccessible because reference passed away exclusively
    private var settings = http_parser_settings()
    private var decodingState: HTTPDecodingState = .beforeMessageBegin
    private var firstNonDiscardableOffset: Int? = nil
    private var currentFieldByteLength = 0
    private var httpParserOffset = 0
    private var rawBytesView: UnsafeRawBufferPointer = .init(start: UnsafeRawPointer(bitPattern: 0xcafbabe), count: 0)
    private var httpErrno: http_errno? = nil
    private let kind: HTTPDecoderKind
    var requestHeads = CircularBuffer<HTTPRequestHead>(initialCapacity: 1)
    
    enum MessageContinuation {
        case normal
        case skipBody
        case error(http_errno)
    }
    
    private static func fromOpaque(_ opaque: UnsafePointer<http_parser>?) -> BetterHTTPParser {
        return Unmanaged<BetterHTTPParser>.fromOpaque(UnsafeRawPointer(opaque!.pointee.data)).takeUnretainedValue()
    }
    
    init(kind: HTTPDecoderKind) {
        self.kind = kind
        c_nio_http_parser_settings_init(&self.settings)
        self.withExclusiveHTTPParser { parserPtr in
            switch kind {
            case .request:
                c_nio_http_parser_init(parserPtr, HTTP_REQUEST)
            case .response:
                c_nio_http_parser_init(parserPtr, HTTP_RESPONSE)
            }
        }
        self.settings.on_body = { opaque, bytes, len in
            BetterHTTPParser.fromOpaque(opaque).didReceiveBodyData(UnsafeRawBufferPointer(start: bytes, count: len))
            return 0
        }
        self.settings.on_header_field = { opaque, bytes, len in
            BetterHTTPParser.fromOpaque(opaque).didReceiveHeaderFieldData(UnsafeRawBufferPointer(start: bytes, count: len))
            return 0
        }
        self.settings.on_header_value = { opaque, bytes, len in
            BetterHTTPParser.fromOpaque(opaque).didReceiveHeaderValueData(UnsafeRawBufferPointer(start: bytes, count: len))
            return 0
        }
        self.settings.on_status = { opaque, bytes, len in
            BetterHTTPParser.fromOpaque(opaque).didReceiveStatusData(UnsafeRawBufferPointer(start: bytes, count: len))
            return 0
        }
        self.settings.on_url = { opaque, bytes, len in
            BetterHTTPParser.fromOpaque(opaque).didReceiveURLData(UnsafeRawBufferPointer(start: bytes, count: len))
            return 0
        }
        self.settings.on_chunk_complete = { opaque in
            BetterHTTPParser.fromOpaque(opaque).didReceiveChunkCompleteNotification()
            return 0
        }
        self.settings.on_chunk_header = { opaque in
            BetterHTTPParser.fromOpaque(opaque).didReceiveChunkHeaderNotification()
            return 0
        }
        self.settings.on_message_begin = { opaque in
            BetterHTTPParser.fromOpaque(opaque).didReceiveMessageBeginNotification()
            return 0
        }
        self.settings.on_headers_complete = { opaque in
            let parser = BetterHTTPParser.fromOpaque(opaque)
            switch parser.didReceiveHeadersCompleteNotification(versionMajor: Int(opaque!.pointee.http_major),
                                                                versionMinor: Int(opaque!.pointee.http_minor),
                                                                statusCode: Int(opaque!.pointee.status_code),
                                                                isUpgrade: opaque!.pointee.upgrade != 0,
                                                                method: http_method(rawValue: opaque!.pointee.method),
                                                                keepAliveState: opaque!.keepAliveState) {
            case .normal:
                return 0
            case .skipBody:
                return 1
            case .error(let err):
                parser.httpErrno = err
                return -1 // error
            }
        }
        self.settings.on_message_complete = { opaque in
            BetterHTTPParser.fromOpaque(opaque).didReceiveMessageCompleteNotification()
            return 0
        }
    }
    
    private func start(bytes: UnsafeRawBufferPointer, newState: HTTPDecodingState) {
        assert(self.firstNonDiscardableOffset == nil)
        self.firstNonDiscardableOffset = bytes.baseAddress! - self.rawBytesView.baseAddress!
        self.decodingState = newState
    }
    
    private func finish(_ callout: (inout HTTPDecoderDelegate, UnsafeRawBufferPointer) -> Void) {
        var currentFieldByteLength = 0
        swap(&currentFieldByteLength, &self.currentFieldByteLength)
        let start = self.rawBytesView.startIndex + self.firstNonDiscardableOffset!
        let end = start + currentFieldByteLength
        self.firstNonDiscardableOffset = nil
        precondition(start >= self.rawBytesView.startIndex && end <= self.rawBytesView.endIndex)
        callout(&self.delegate, .init(rebasing: self.rawBytesView[start ..< end]))
    }
    
    private func didReceiveBodyData(_ bytes: UnsafeRawBufferPointer) {
        self.delegate.didReceiveBody(bytes)
    }
    
    private func didReceiveHeaderFieldData(_ bytes: UnsafeRawBufferPointer) {
        switch self.decodingState {
        case .headerName, .trailerName:
            ()
        case .headerValue:
            self.finish { delegate, bytes in
                delegate.didReceiveHeaderValue(bytes)
            }
            self.start(bytes: bytes, newState: .headerName)
        case .trailerValue:
            self.finish { delegate, bytes in
                delegate.didReceiveTrailerValue(bytes)
            }
            self.start(bytes: bytes, newState: .trailerName)
        case .url:
            self.finish { delegate, bytes in
                delegate.didReceiveURL(bytes)
            }
            self.start(bytes: bytes, newState: .headerName)
        case .headersComplete:
            // these are trailers
            self.start(bytes: bytes, newState: .trailerName)
        case .afterMessageBegin:
            // in case we're parsing responses
            self.start(bytes: bytes, newState: .headerName)
        case .beforeMessageBegin:
            preconditionFailure()
        }
        self.currentFieldByteLength += bytes.count
    }
    
    private func didReceiveHeaderValueData(_ bytes: UnsafeRawBufferPointer) {
        switch self.decodingState {
        case .headerValue, .trailerValue:
            ()
        case .headerName:
            self.finish { delegate, bytes in
                delegate.didReceiveHeaderName(bytes)
            }
            self.start(bytes: bytes, newState: .headerValue)
        case .trailerName:
            self.finish { delegate, bytes in
                delegate.didReceiveTrailerName(bytes)
            }
            self.start(bytes: bytes, newState: .trailerValue)
        case .beforeMessageBegin, .afterMessageBegin, .headersComplete, .url:
            preconditionFailure()
        }
        self.currentFieldByteLength += bytes.count
    }
    
    private func didReceiveStatusData(_ bytes: UnsafeRawBufferPointer) {
        // we don't do anything special here because we'll need the whole 'head' anyway
    }
    
    private func didReceiveURLData(_ bytes: UnsafeRawBufferPointer) {
        switch self.decodingState {
        case .url:
            ()
        case .afterMessageBegin:
            self.start(bytes: bytes, newState: .url)
        case .beforeMessageBegin, .headersComplete, .headerName, .headerValue, .trailerName, .trailerValue:
            preconditionFailure()
        }
        self.currentFieldByteLength += bytes.count
    }
    
    private func didReceiveChunkCompleteNotification() {
        // nothing special to do, we handle chunks just like any other body part
    }
    
    private func didReceiveChunkHeaderNotification() {
        // nothing special to do, we handle chunks just like any other body part
    }
    
    private func didReceiveMessageBeginNotification() {
        switch self.decodingState {
        case .beforeMessageBegin:
            self.decodingState = .afterMessageBegin
        case .headersComplete, .headerName, .headerValue, .trailerName, .trailerValue, .afterMessageBegin, .url:
            preconditionFailure()
        }
    }
    
    private func didReceiveMessageCompleteNotification() {
        switch self.decodingState {
        case .headersComplete:
            ()
        case .trailerValue:
            self.finish { delegate, bytes in
                delegate.didReceiveTrailerValue(bytes)
            }
        case .beforeMessageBegin, .headerName, .headerValue, .trailerName, .afterMessageBegin, .url:
            preconditionFailure()
        }
        self.decodingState = .beforeMessageBegin
        self.delegate.didFinishMessage()
    }
    
    private func didReceiveHeadersCompleteNotification(versionMajor: Int,
                                                       versionMinor: Int,
                                                       statusCode: Int,
                                                       isUpgrade: Bool,
                                                       method: http_method,
                                                       keepAliveState: KeepAliveState) -> MessageContinuation {
        switch self.decodingState {
        case .headerValue:
            self.finish { delegate, bytes in
                delegate.didReceiveHeaderValue(bytes)
            }
        case .url:
            self.finish { delegate, bytes in
                delegate.didReceiveURL(bytes)
            }
        case .afterMessageBegin:
            // we're okay here for responses (as they don't have URLs) but for requests we must have seen a URL/headers
            precondition(self.kind == .response)
        case .beforeMessageBegin, .headersComplete, .headerName, .trailerName, .trailerValue:
            preconditionFailure()
        }
        assert(self.firstNonDiscardableOffset == nil)
        self.decodingState = .headersComplete
        let success = self.delegate.didFinishHead(versionMajor: versionMajor,
                                                  versionMinor: versionMinor,
                                                  isUpgrade: isUpgrade,
                                                  method: method,
                                                  statusCode: statusCode,
                                                  keepAliveState: keepAliveState)
        guard success else {
            return .error(HPE_INVALID_VERSION)
        }
        
        if self.kind == .response {
            // http_parser doesn't correctly handle responses to HEAD requests. We have to do something
            // annoyingly opaque here, and in those cases return 1 instead of 0. This forces http_parser
            // to not expect a request body.
            //
            // The same logic applies to CONNECT: RFC 7230 says that regardless of what the headers say,
            // responses to CONNECT never have HTTP-level bodies.
            //
            // Finally, we need to work around a bug in http_parser for 1XX, 204, and 304 responses.
            // RFC 7230 says:
            //
            // > ... any response with a 1xx (Informational),
            // > 204 (No Content), or 304 (Not Modified) status
            // > code is always terminated by the first empty line after the
            // > header fields, regardless of the header fields present in the
            // > message, and thus cannot contain a message body.
            //
            // However, http_parser only does this for responses that do not contain length fields. That
            // does not meet the requirement of RFC 7230. This is an outstanding http_parser issue:
            // https://github.com/nodejs/http-parser/issues/251. As a result, we check for these status
            // codes and override http_parser's handling as well.
            let method = self.requestHeads.removeFirst().method
            if method == .HEAD || method == .CONNECT {
                return .skipBody
            }
            
            if (statusCode / 100 == 1 ||  // 1XX codes
                statusCode == 204 ||
                statusCode == 304) {
                return .skipBody
            }
        }
        
        return .normal
    }
    
    func start() {
        self.withExclusiveHTTPParser { parserPtr in
            parserPtr.pointee.data = Unmanaged.passRetained(self).toOpaque()
        }
    }
    
    func stop() {
        self.withExclusiveHTTPParser { parserPtr in
            let selfRef = parserPtr.pointee.data
            Unmanaged<BetterHTTPParser>.fromOpaque(selfRef!).release()
            parserPtr.pointee.data = UnsafeMutableRawPointer(bitPattern: 0xdedbeef)
        }
    }
    
    @inline(__always) // this need to be optimised away
    func withExclusiveHTTPParser<T>(_ body: (UnsafeMutablePointer<http_parser>) -> T) -> T {
        var parser: http_parser? = nil
        assert(self.parser != nil, "parser must not be nil here, must be a re-entrancy issue")
        swap(&parser, &self.parser)
        defer {
            assert(self.parser == nil, "parser must not nil here")
            swap(&parser, &self.parser)
        }
        return body(&parser!)
    }
    
    func feedInput(_ bytes: UnsafeRawBufferPointer?) throws -> Int {
        var parserErrno: UInt32 = 0
        let parserConsumed = self.withExclusiveHTTPParser { parserPtr -> Int in
            let parserResult: Int
            if let bytes = bytes {
                self.rawBytesView = bytes
                defer {
                    self.rawBytesView = .init(start: UnsafeRawPointer(bitPattern: 0xdafbabe), count: 0)
                }
                parserResult = c_nio_http_parser_execute_swift(parserPtr,
                                                               &self.settings,
                                                               bytes.baseAddress! + self.httpParserOffset,
                                                               bytes.count - self.httpParserOffset)
            } else {
                parserResult = c_nio_http_parser_execute(parserPtr, &self.settings, nil, 0)
            }
            parserErrno = parserPtr.pointee.http_errno
            return parserResult
        }
        assert(parserConsumed >= 0)
        // self.parser must be non-nil here because we can't be re-entered here (ByteToMessageDecoder guarantee)
        guard parserErrno == 0 else {
            // if we chose to abort (eg. wrong HTTP version) the error will be in self.httpErrno, otherwise http_parser
            // will tell us...
            // self.parser must be non-nil here because we can't be re-entered here (ByteToMessageDecoder guarantee)
            let err = http_errno(rawValue: self.httpErrno?.rawValue ?? parserErrno)
            throw HTTPParserError.httpError(fromCHTTPParserErrno: err)!
        }
        if let firstNonDiscardableOffset = self.firstNonDiscardableOffset {
            self.httpParserOffset += parserConsumed - firstNonDiscardableOffset
            self.firstNonDiscardableOffset = 0
            return firstNonDiscardableOffset
        } else {
            // By definition we've consumed all of the http parser offset at this stage. There may still be bytes
            // left in the buffer though: we didn't consume them because they aren't ours to consume, as they may belong
            // to an upgraded protocol.
            //
            // Set the HTTP parser offset back to zero, and tell the parent that we consumed
            // the whole buffer.
            let consumedBytes = self.httpParserOffset + parserConsumed
            self.httpParserOffset = 0
            return consumedBytes
        }
    }
}

private protocol HTTPDecoderDelegate {
    mutating func didReceiveBody(_ bytes: UnsafeRawBufferPointer)
    mutating func didReceiveHeaderName(_ bytes: UnsafeRawBufferPointer)
    mutating func didReceiveHeaderValue(_ bytes: UnsafeRawBufferPointer)
    mutating func didReceiveTrailerName(_ bytes: UnsafeRawBufferPointer)
    mutating func didReceiveTrailerValue(_ bytes: UnsafeRawBufferPointer)
    mutating func didReceiveURL(_ bytes: UnsafeRawBufferPointer)
    mutating func didFinishHead(versionMajor: Int,
                                versionMinor: Int,
                                isUpgrade: Bool,
                                method: http_method,
                                statusCode: Int,
                            keepAliveState: KeepAliveState) -> Bool
    mutating func didFinishMessage()
}
