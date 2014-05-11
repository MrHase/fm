#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <string>
#include "team.h"

#include "myml.h"

#define GE_WIDTH 18
#define GE_HEIGHT 22

class GEException{
public:
    GEException(string error){this->error=error;}

    string error;
};

using namespace std;
class GameEngine
{
public:
    GameEngine();

    void Action();
    void ExecuteActions(string name, MyML& actions, MyML &all_actions);
    MyML Action(MyML &ge, MyML& player, string team, bool invert_pos);
    MyML Draw();
    MyML& GetPlayer(string id);
    MyML& GetML(){return data;}

    int GetRND(int s,int e);

    void Start();


    MyML data;
private:
    bool invert_positions;
    MyML INV(MyML& pos);
    MyML invert(MyML& pos);

    std::mt19937 rnd;
    uint32_t seed_val;
};

#endif // GAMEENGINE_H
