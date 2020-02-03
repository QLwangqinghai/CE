//
//  main.cpp
//  CPPTest
//
//  Created by vector on 2018/12/13.
//  Copyright © 2018 com.wangqinghai. All rights reserved.
//

#include <iostream>
using namespace std;

class C
{
public:
    void out2(string s)
    {
        cout<<"A(out2):"<<s<<endl;
    }
};

class A
{
public:
    virtual void out2(string s)
    {
        cout<<"A(out2):"<<s<<endl;
    }
};

class B : A
{
public:
    virtual void out2(string s)
    {
        cout<<"A(out2):"<<s<<endl;
    }
};



class C1
{
    uint64_t aa;
    
public:
    void out2(string s)
    {
        cout<<"A(out2):"<<s<<endl;
    }
};

class A1
{
    uint64_t aa;

public:
    virtual void out2(string s)
    {
        cout<<"A(out2):"<<s<<endl;
    }
    virtual void out1(string s)
    {
        cout<<"A(out2):"<<s<<endl;
    }
};

class B1 : A1
{

public:
    virtual void out2(string s)
    {
        cout<<"A(out2):"<<s<<endl;
    }
};



int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    std::cout << sizeof(A) <<endl;
    std::cout << sizeof(B) <<endl;
    std::cout << sizeof(C) <<endl;
    std::cout << sizeof(A1) <<endl;
    std::cout << sizeof(B1) <<endl;
    std::cout << sizeof(C1) <<endl;
    return 0;
}
