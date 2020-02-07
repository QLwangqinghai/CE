//
//  CxxBridge.c
//  CC
//
//  Created by vector on 2020/2/6.
//  Copyright © 2020 vector. All rights reserved.
//

#include "CxxBridge.h"

#include <iostream>

using namespace std;

class LSManager {

    
private:
    
    LSManager(u_int32_t handlerCount) {
        cout<< "LSManager 默认构造函数" << endl;
        
    }

    LSManager(const LSManager &p){
        cout<< "LSManager 拷贝构造函数" << endl;
    }
    ~LSManager(){
        cout<< "LSManager 析构函数" << endl;
    }
    
    
    
    
};
