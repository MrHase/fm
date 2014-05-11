#ifndef POS_H
#define POS_H

#include "myml.h"
#include <string>
class Pos
{
public:
    Pos();
    Pos(string str);

    Pos(MyML p);

    Pos(int x, int y){X(x);Y(y);}


    MyML ToMyML();
    int X(){return x;}
    int Y(){return y;}
    void X(int x){this->x=x;}
    void Y(int y){this->y=y;}

    string ToString();


private:
    int x,y;
};
bool operator ==(Pos& p1, Pos& p2);
bool operator !=(Pos& p1, Pos& p2);

#endif // POS_H
