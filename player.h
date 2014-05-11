#ifndef PLAYER_H
#define PLAYER_H

#include <map>
#include <string>
#include "myml.h"

#include <random>
using namespace std;

#include "pos.h"
//#define PLAYERID "15"
#define PLAYERID "1"


static list<MyML> GetMovePoints_(Pos p1, Pos p2){
    list<MyML> mp;
    int x1=p1.X();
    int y1=p1.Y();
    int x2=p2.X();
    int y2=p2.Y();

    int mv_x=x2-x1;
    int mv_y=y2-y1;

    int mv_x_abs=abs(mv_x);
    int mv_y_abs=abs(mv_y);

    int dia;
    if(mv_x_abs<mv_y_abs){
        dia=mv_x_abs;
    }else{
        dia=mv_y_abs;
    }
    //int dia=abs(mv_x_abs-mv_y_abs);


    int mv_dir_x=0;
    if(mv_x!=0)
        mv_dir_x=(mv_x/abs(mv_x));

    int mv_dir_y=0;
    if(mv_y!=0)
        mv_dir_y=(mv_y/abs(mv_y));

    Pos lastpos=p1;

    for(int i=0;i<dia;i++){
        Pos p_temp=lastpos;
        p_temp.X(p_temp.X()+mv_dir_x);
        p_temp.Y(p_temp.Y()+mv_dir_y);
        mp.push_back(p_temp.ToMyML());
        lastpos=p_temp;
    }

    int new_x_mp=abs(lastpos.X()-p2.X());
    int new_y_mp=abs(lastpos.Y()-p2.Y());

    for(int i=0;i<new_x_mp;i++){
        Pos p_temp=lastpos;
        p_temp.X(p_temp.X()+mv_dir_x);
        p_temp.Y(p_temp.Y());
        mp.push_back(p_temp.ToMyML());
        lastpos=p_temp;
    }
    for(int i=0;i<new_y_mp;i++){

        Pos p_temp=lastpos;
        p_temp.X(p_temp.X());
        p_temp.Y(p_temp.Y()+mv_dir_y);
        mp.push_back(p_temp.ToMyML());
        lastpos=p_temp;
    }

//    for(auto myml:mp){
//        cout<<myml.Info()<<endl;
//    }



    return mp;
}

static MyML GetMovePoints(Pos p1, Pos p2){

    MyML ret("type=array;size=0");

    for(auto myml:GetMovePoints_(p1,p2)){
        MyMLArray(ret).Add(myml);
    }
    return ret;
}

class GameEngine;
class Player
{
public:
    Player();


    MyML& GetData(){return data;}
    MyML data;
    int GetRND(int s, int e);


private:
    std::mt19937 rnd;
    uint32_t seed_val;


};

void SaveCout();
void DisableCout(string id);
void RestoreCout();

MyML GetRandomAction(MyML &ge);
bool RandomSucceed(string value);

MyML PlayerAction(MyML &ge, MyML& player);
MyML NewPlayerAction(MyML &ge, MyML& player);
MyML NewNewPlayerAction(MyML &ge, MyML& player);

void ApplyActionResult(MyML &player, MyML action);

string Info(MyML& player);

#endif // PLAYER_H
