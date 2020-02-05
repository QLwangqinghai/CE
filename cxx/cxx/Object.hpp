//
//  Object.hpp
//  cxx
//
//  Created by vector on 2020/2/5.
//  Copyright © 2020 angfung. All rights reserved.
//

#ifndef Object_hpp
#define Object_hpp

#include <iostream>




//typedef CCUInt CCRefCounterType;
//
//
//#pragma pack(push, 1)
//
//typedef struct {
//    uintptr_t flag: 1;
//    uintptr_t type: 12;
//    uintptr_t hasWeak: 1;
//#if CCBuild64Bit
//    uintptr_t count: 50;
//#else
//    uintptr_t count: 18;
//#endif
//} CCRefDecreasedBaseInfo;
//
//
//typedef struct {
//#if CCBuild64Bit
//    CCUInt count: 56;
//#else
//    CCUInt count: 24;
//#endif
//    CCUInt hasWeak: 1;
//    CCUInt domain: 7;
//} CCRefCounter;
//
//

using namespace std;

struct Struct {
    int a;

    Struct(){
        cout<< "默认构造函数" << endl;
    }

    Struct(const Struct &p){
        this->a = p.a;
        cout<< "拷贝构造函数" << endl;
    }
    
    
    ~Struct(){
        cout<< "Objc_object 析构函数" << endl;
    }
    
};

struct Type: Struct {
    int c;

    ~Type(){
        cout<< "Objc_classs 析构函数" << endl;
    }
};










#endif /* Object_hpp */
