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

#import "CCAllocator.h"
#import "CCBase.h"
#import "CCClosure.h"
#import "CCConfig.h"
#import "CCInteger.h"
#import "CCNet.h"
#import "CCReferenceBase.h"
#import "CCType.h"
#import "CCFoundation.h"

FOUNDATION_EXPORT double CCFoundationVersionNumber;
FOUNDATION_EXPORT const unsigned char CCFoundationVersionString[];

