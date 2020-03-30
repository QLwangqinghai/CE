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

#import "de265.h"
#import "de265-version.h"

FOUNDATION_EXPORT double libde265VersionNumber;
FOUNDATION_EXPORT const unsigned char libde265VersionString[];

