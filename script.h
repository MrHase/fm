#ifndef SCRIPT_H
#define SCRIPT_H



#include <string>
#include <iostream>
#include <vector>
#include "myml.h"
using namespace std;

enum RefType{
    FN,
    STRING,
    ELEMENT,
    ATTRIBUTE
};


class Ref{
public:
    Ref(){myml=NULL;}

    RefType type;
    //FN
    //vector<string> code;
    string code;
    vector<string> para;

    //ELEMENT
    MyML* myml;

    //STRING

    string str;

    //CMD
    void CmdInfo();
    void CmdAddE(string e);
    void CmdAddA(string a, string v);


    Ref GetRef(string path);
    Ref Copy(string path);

    string ToString(string path);


};

void CmdCmp(map<string, Ref> &refs, string first, string cmp, string second, string fn_true, string fn_false);
class Script
{
public:
    Script();
    ~Script();

    MyML Run(string filename, vector<MyML *> myml, string function);
    void Execute(vector<string> code, map<string, Ref> &references);
    void Execute(string code, map<string, Ref> &references);
private:
    string Clean(string str);
    string ExecuteFunctions(map<string, Ref>& references,string str);
    string GenerateMyML(map<string, Ref>& references, string str);
    string Call(string fn, map<string, Ref> &references, vector<string> para);

    list<MyML*> new_elements;
};

#endif // SCRIPT_H
