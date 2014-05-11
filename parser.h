#ifndef PARSER_H
#define PARSER_H

#include "myml.h"
#include <iostream>
using namespace std;

static MyML parse(string str){
    cout<<"String to parse: "<<str<<endl;
    MyML ret("testa=hallo");


    return ret;
}

class Parser
{
public:
    Parser();
};

#endif // PARSER_H
