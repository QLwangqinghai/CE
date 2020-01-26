#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#else
#ifndef FOUNDATION_EXPORT
#if defined(__cplusplus)
#define FOUNDATION_EXPORT extern "C"
#else
#define FOUNDATION_EXPORT extern
#endif
#endif
#endif

#import "SITPIndexShift.h"
#import "SITPParser.h"
#import "SITPType.h"
#import "SITP.h"

FOUNDATION_EXPORT double SITPVersionNumber;
FOUNDATION_EXPORT const unsigned char SITPVersionString[];

