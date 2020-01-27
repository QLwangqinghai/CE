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

#import "FileManagerBase.h"
#import "FileService.h"

FOUNDATION_EXPORT double FileServiceVersionNumber;
FOUNDATION_EXPORT const unsigned char FileServiceVersionString[];

