#include <iostream>
#include <string>
#include "src/cacCore.h"
using namespace std;

int main(){
    CacCore h(4);
    size8BytesT foo [2] = {16, 32};
    std::vector<size8BytesT> expectedVector(std::begin(foo), std::end(foo));
    Register r(0, 0, 128, foo);    
    RegisterSnapshot rs(0);
    std::cout<<h.getHello()<<std::endl;

    //size8BytesT pc [] = {0xdeadbeef};
    //Info infoDut(0, "PC", "DUT", pc);
    //cout<<infoDut.getFormatInfo()<<std::endl;
    return(0);
}