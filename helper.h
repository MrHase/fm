#ifndef HELPER_H
#define HELPER_H


#include <string>
#include <sstream>
#include <list>
#include <math.h>
#include <iostream>

using namespace std;

#include "myml.h"


#define LOG __FILE__<<": "<<__LINE__<<endl

static int StringToInt(string str){

    stringstream ss(str);
    //hier auf Double überprüfen... casten!
    int i;
    ss >> i;

    return i;
}
static int INT(string str){
    return StringToInt(str);
}

static string IntToString(int i){
    stringstream ss;
    ss<<i;
    return ss.str();
}
static string STR(int i){
    return IntToString(i);
}
static string STR(double i){
    stringstream ss;
    ss<<i;
    return ss.str();
}
static double DOUBLE(string str){
    stringstream ss(str);
    double i;
    ss >> i;
    return i;
}



vector<string> splitstring(string str, char split='.');
string Remove(string str, int pos);
string RemoveFirst(string str);
string RemoveLast(string str);
string Last(string str);
string First(string str);

class Helper
{
public:
    Helper();


};

template<typename Func>
string FindInnerStringAndReplace(string str,string bopen,string bclose,Func func){


    string nstr="";
    int level=0;
    vector<string> tmpstr;
    tmpstr.push_back(string());

    if(str.find(bopen)!=string::npos){

        for(int i=0;i<str.size();i++){
            string c=str.substr(i,1);

            if(c==bopen){
                tmpstr.push_back(string());
                level++;
            }else if(c==bclose){
                string fullstr=tmpstr.back();

                //FUNKTIONAUFRUF
                //string ret=BLA();
                string ret=func(fullstr);
                tmpstr[level-1]+=ret;

                tmpstr.pop_back();
                level--;
            }else{

                tmpstr[level]+=c;

            }
        }
    }else{
        tmpstr[0]=str;
    }
    nstr=tmpstr[0];

    return nstr;
}


static unsigned int ABS(int v){
    if(v<0)
        return (v*(-1));
    else
        return v;
}

static int SIGN(int v){
    if(v>0)
        return 1;
    else if (v<0)
        return -1;
    else return 0;
    //return (ABS(v)/v);
}

int Distance(MyMLPos p1,MyMLPos p2);
MyML GotoPos(MyMLPos start,MyMLPos end, int distance=0);

#endif // HELPER_H
