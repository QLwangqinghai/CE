//
//  CXX.hpp
//  X
//
//  Created by vector on 2020/4/20.
//  Copyright © 2020 haoqi. All rights reserved.
//

#ifndef CXX_hpp
#define CXX_hpp

#include <stdio.h>
#include "CX.h"


namespace X {
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
        public:
        StackObject(StackObject &obj) {
            this->ptr = XRefRetain(obj.ptr);
        }
        StackObject() {
        //此处可以获得需要封装的资源，并让ptr指针指向该资源
//            ptr = new Resource() ;
//            this->ptr = XRefRetain(_obj.ptr);

        }
        ~StackObject() {
            XRefRelease(this->ptr);
//        delete ptr ; //释放封装的资源
        }
        
        // 重写=操作符，使之完成深拷贝
        void operator= (StackObject & obj) {
            if (this->ptr != obj.ptr) {
                XRefRelease(this->ptr);
            }
            this->ptr = XRefRetain(obj.ptr);
        }
    };
}



#endif /* CXX_hpp */
