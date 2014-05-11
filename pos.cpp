#include "pos.h"

#include "helper.h"
#include <sstream>
Pos::Pos()
{
}

Pos::Pos(MyML p){

    X(StringToInt(p.A("x")));
    Y(StringToInt(p.A("y")));

}


MyML Pos::ToMyML()
{
    MyML tmp;
    tmp.AddE("Pos");
    tmp.E("Pos").AddA("x",IntToString(x));
    tmp.E("Pos").AddA("y",IntToString(y));
    return tmp;
}

string Pos::ToString()
{
    stringstream ss;
    ss<<x<<","<<y;
    return ss.str();
}


bool operator ==(Pos& p1, Pos& p2){
    return (p1.X()==p2.X() && p1.Y()==p2.Y());

}

bool operator !=(Pos& p1, Pos& p2){
    return !(p1==p2);
}
