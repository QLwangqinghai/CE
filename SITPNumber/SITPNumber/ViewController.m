//
//  ViewController.m
//  SITPNumber
//
//  Created by vector on 2019/12/23.
//  Copyright © 2019 angfung. All rights reserved.
//

#import "ViewController.h"
#include <CoreText/CoreText.h>
#include <CoreGraphics/CoreGraphics.h>

#include <stdatomic.h>


typedef struct {
    uint32_t type;
    _Atomic(uint_fast32_t) ref1;
} CCValueBase0;




typedef struct {
    _Atomic(uint_fast32_t) ref;
    _Atomic(uint_fast32_t) ref1;
} CCValueBase1;
typedef struct {
    _Atomic(uint_fast32_t) ref;
    _Atomic(uint_fast16_t) ref1;
    _Atomic(uint_fast16_t) ref2;

} CCValueBase2;


static uint8_t a __attribute__((aligned(64))) = 8;

static uint8_t b __attribute__((aligned(32))) = 18;

static uint32_t c __attribute__((aligned(128))) = 16;


typedef struct CFRuntimeClass2 {
    CFIndex version;
    const char *className; // must be a pure ASCII string, nul-terminated
    void (*init)(CFTypeRef cf);
    CFTypeRef (*copy)(CFAllocatorRef allocator, CFTypeRef cf);
    void (*finalize)(CFTypeRef cf);
    Boolean (*equal)(CFTypeRef cf1, CFTypeRef cf2);
    CFHashCode (*hash)(CFTypeRef cf);
    CFStringRef (*copyFormattingDesc)(CFTypeRef cf, CFDictionaryRef formatOptions);    // return str with retain
    CFStringRef (*copyDebugDesc)(CFTypeRef cf);    // return str with retain

} CFRuntimeClass2_t;

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.

    CFStringRef s = CFStringCreateMutable(NULL, 2000);
    {
        uintptr_t _cfisa = 0;
        memcpy(&_cfisa, s, sizeof(uintptr_t));
        printf("%lx\n", _cfisa);
        
    }
    
    NSString * str = [NSString stringWithFormat:@";akldfsl;werwr%p", self];
    CFStringRef strRef = (__bridge CFStringRef)str;
    
    NSString * str2 = (__bridge NSString *)strRef;
    printf("%p, %p, %p\n", str, strRef, str2);

    uintptr_t _cfisa = 0;
    memcpy(&_cfisa, strRef, sizeof(uintptr_t));
    printf("%lx\n", _cfisa);
    
    Class rcls = [str class];
    
    CFRuntimeClass2_t * cls = _cfisa;
    CFRuntimeClass2_t * cls2 = (CFRuntimeClass2_t *)((__bridge void *)rcls);

    CFRuntimeClass2_t * cls3 = NULL;
    {
        uintptr_t cfisa = 0;
        memcpy(&cfisa, cls2, sizeof(uintptr_t));
        cls3 = cfisa;
    }

    
    NSLog(@"sizeof: %ld", sizeof(CCValueBase1));
    NSLog(@"sizeof: %ld", sizeof(CCValueBase2));

    uint8_t bytes[100] = {1, 0};
    char * ptr = bytes;
    
    printf("str::::   -%s\n", ptr);
    
    uintptr_t ap = &a;
    uintptr_t bp = &b;
    uintptr_t cp = &c;
    NSLog(@"%ld, %ld, %ld", sizeof(a), bp % 32, cp % 128);

    
    NSLog(@"%ld, %ld, %ld", ap % 64, bp % 32, cp % 128);
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
