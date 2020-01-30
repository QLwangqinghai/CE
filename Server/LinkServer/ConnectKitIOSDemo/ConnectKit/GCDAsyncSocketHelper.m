//
//  SwiftHelper.m
//  IMDemo
//
//  Created by wangqinghai on 16/5/28.
//  Copyright © 2016年 st. All rights reserved.
//

#import "GCDAsyncSocketHelper.h"
#import "GCDAsyncSocket.h"

@implementation GCDAsyncSocketHelper

+ (nullable NSDictionary *)loadGCDSocketCertificateSettingsWithPeerName:(nonnull NSString *)peer passwd:(nonnull NSString *)passwd pkcs12data:(nonnull NSData *)pkcs12data
{
    CFDataRef inPKCS12Data = (__bridge CFDataRef)pkcs12data;
    
    CFStringRef password = CFSTR("jiuxiu");
    const void *keys[] = { kSecImportExportPassphrase };
    const void *values[] = { password };
    CFDictionaryRef options = CFDictionaryCreate(NULL, keys, values, 1, NULL, NULL);
    
    CFArrayRef items = CFArrayCreate(NULL, 0, 0, NULL);
    
    OSStatus securityError = SecPKCS12Import(inPKCS12Data, options, &items);
    CFRelease(options);
    CFRelease(password);
    
    if(securityError == errSecSuccess)
        NSLog(@"Success opening p12 certificate.");
    
    CFDictionaryRef identityDict = CFArrayGetValueAtIndex(items, 0);
    SecIdentityRef myIdent = (SecIdentityRef)CFDictionaryGetValue(identityDict,kSecImportItemIdentity);
    
    SecIdentityRef  certArray[1] = { myIdent };
    CFArrayRef myCerts = CFArrayCreate(NULL, (void *)certArray, 1, NULL);
    
    NSMutableDictionary *settings = [NSMutableDictionary dictionaryWithCapacity:3];
    [settings setObject:peer forKey:(NSString *)kCFStreamSSLPeerName];
    //    [settings setObject:(NSString *)kCFStreamSocketSecurityLevelSSLv3 forKey:(NSString*)kCFStreamSSLLevel];
    [settings setObject:(NSString *)kCFStreamSocketSecurityLevelSSLv3 forKey:(NSString*)kCFStreamPropertySocketSecurityLevel];
    [settings setObject:(__bridge id)myCerts forKey:(NSString *)kCFStreamSSLCertificates];
    [settings setObject:[NSNumber numberWithBool:NO] forKey:(NSString *)kCFStreamSSLIsServer];
    
    //    kSSLProtocolUnknown = 0,                /* no protocol negotiated/specified; use default */
    //    kSSLProtocol3       = 2,				/* SSL 3.0 */
    //    kTLSProtocol1       = 4,				/* TLS 1.0 */
    //    kTLSProtocol11      = 7,				/* TLS 1.1 */
    //    kTLSProtocol12      = 8,				/* TLS 1.2 */
    //    kDTLSProtocol1      = 9,                /* DTLS 1.0 */
    //
    //    /* DEPRECATED on iOS */
    //    kSSLProtocol2       = 1,				/* SSL 2.0 */
    //    kSSLProtocol3Only   = 3,                /* SSL 3.0 Only */
    //    kTLSProtocol1Only   = 5,                /* TLS 1.0 Only */
    //    kSSLProtocolAll     = 6,                /* All TLS supported protocols */
    //
    //    *		kSSLProtocol3
    //    * 		kTLSProtocol1
    //    * 		kTLSProtocol11
    //    * 		kTLSProtocol12
    [settings setObject:[NSNumber numberWithInteger:2] forKey:GCDAsyncSocketSSLProtocolVersionMin];
    
    //    * For TLS contexts, legal values for minVersion are :
    //        *		kSSLProtocol3
    //        * 		kTLSProtocol1
    //        * 		kTLSProtocol11
    //        * 		kTLSProtocol12
    //        *
    //        * For DTLS contexts, legal values for minVersion are :
    //            *      kDTLSProtocol1
    [settings setObject:[NSNumber numberWithInteger:4] forKey:GCDAsyncSocketSSLProtocolVersionMax];
    
    settings[(NSString *)kCFStreamSSLValidatesCertificateChain]	= [NSNumber numberWithBool:NO];
#if TARGET_OS_IPHONE
    settings[GCDAsyncSocketUseCFStreamForTLS] = [NSNumber numberWithBool:YES];
#endif
    return settings;
}

@end

