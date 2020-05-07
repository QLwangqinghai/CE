//
//  ViewController.m
//  X
//
//  Created by vector on 2020/4/20.
//  Copyright © 2020 haoqi. All rights reserved.
//

#import "ViewController.h"
#import "CXX.hpp"
#import <string>
#include "CX.h"
#import "CXTester.h"

namespace XX {
    class StackObject {
        protected:
            XRef ptr;
        
        private:
//        uintptr_t ptr;
        void* operator new(size_t size){ //非严格实现，仅作示意之用
            return malloc(size) ;
        }
        void operator delete(void* pp){ //非严格实现，仅作示意之用
            free(pp) ;
        }
        
        StackObject() {
            printf("init\n");
            XRefRelease(this->ptr);
        }
        
        public:
        StackObject(XRef _Nonnull ptr) {
            this->ptr = XRefRetain(ptr);
            printf("init ptr\n");
        }
        
        StackObject(const StackObject & obj) {
            this->ptr = XRefRetain(obj.ptr);
            printf("copy init\n");
        }

        ~StackObject() {
            XRefRelease(this->ptr);
//        delete ptr ; //释放封装的资源
            printf("deinit\n");
        }
        
        // 重写=操作符，使之完成深拷贝
        void operator = (const StackObject & obj) {
            if (this->ptr != obj.ptr) {
                XRefRelease(this->ptr);
            }
            this->ptr = XRefRetain(obj.ptr);

            printf("copy = \n");
        }
    };
    
    class String: public StackObject {
        
    public:
        String(XRef _Nonnull ptr) : StackObject(ptr) {
            printf("string init ptr\n");
        }
        
        
    };
    
    // 重写=操作符，使之完成深拷贝
    String StringMake(String & obj) {
        return obj;
    }
    // 重写=操作符，使之完成深拷贝
    StackObject StackObjectMake(StackObject & obj) {
        return obj;
    }
}

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    [CXTester test0];
    
    return;
    NSLog(@"X::StackObject size %ld", sizeof(X::StackObject));
    
    static int objPtr = 0;
    static XRef _Nonnull objRef = &objPtr;

    
    XX::String obj(objRef);

    XX::String aobj = XX::String(obj);
    
    XX::String bobj = XX::StringMake(obj);

    bobj = aobj;
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}


@end
