//
//  main.cpp
//  cxx
//
//  Created by vector on 2020/2/5.
//  Copyright © 2020 angfung. All rights reserved.
//

#include <iostream>

using namespace std;


struct Objc_object {
    int a;

    Objc_object(){
        cout<< "默认构造函数" << endl;
    }

    Objc_object(const Objc_object &p){
        this->a = p.a;
        cout<< "拷贝构造函数" << endl;
    }
    
    
    ~Objc_object(){
        cout<< "Objc_object 析构函数" << endl;
    }
    
};

struct Objc_classs: Objc_object {
    int c;

    ~Objc_classs(){
        cout<< "Objc_classs 析构函数" << endl;
    }
};


struct StackObject {
    void * ref;
    StackObject(void * ref){
        this->ref = ref;
        cout<< "StackObject 默认构造函数" << endl;
    }

    StackObject(const StackObject &p){
        this->ref = p.ref;
        cout<< "StackObject 拷贝构造函数" << endl;
    }
    ~StackObject(){
        cout<< "StackObject 析构函数" << endl;
    }
};

void funccc(StackObject obj) {
    printf("StackObject - %ld\n", obj.a);
}

void funccc2(const StackObject &obj) {
    printf("StackObject2 - %ld\n", obj.a);
}


int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    std::cout << sizeof(Objc_object) << "\n";
    std::cout << sizeof(Objc_classs) << "\n";

    Objc_classs * cls = new Objc_classs();
    
    Objc_object * cls2 = (Objc_object *)cls;
    delete cls2;
    
    StackObject obj = StackObject();
    StackObject obj2 = obj;
    StackObject obj3 = obj2;

    printf("begin funccc\n");

    funccc(obj3);
    printf("end funccc\n");

    printf("begin funccc2\n");
    funccc2(obj3);
    printf("end funccc2\n");
    
    
    return 0;
}
