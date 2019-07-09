//
//  CNIOHTTPParser.h
//  CNIOHTTPParser
//
//  Created by vector on 2019/7/9.
//  Copyright © 2019 vector. All rights reserved.
//

#import <Cocoa/Cocoa.h>

//! Project version number for CNIOHTTPParser.
FOUNDATION_EXPORT double CNIOHTTPParserVersionNumber;

//! Project version string for CNIOHTTPParser.
FOUNDATION_EXPORT const unsigned char CNIOHTTPParserVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <CNIOHTTPParser/PublicHeader.h>

#import <CNIOHTTPParser/CNIOHTTPParser.h>
#import <CNIOHTTPParser/c_nio_http_parser.h>

static inline size_t c_nio_http_parser_execute_swift(http_parser *parser,
                                                     const http_parser_settings *settings,
                                                     const void *data,
                                                     size_t len) {
    return c_nio_http_parser_execute(parser, settings, (const char *)data, len);
}
