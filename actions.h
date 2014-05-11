#ifndef ACTIONS_H
#define ACTIONS_H
#include "myml.h"

class Action{
public:
    Action(){};
    //Action(function<void(MyML,int)> reward_function):reward(reward_function){}
    Action(function<void(MyML,int)> reward_function){reward=reward_function;}

    MyML& State(){return state;}
    void State(MyML & state){this->state=state;}
    
    void RewardFunction(function<void(MyML,int)> reward_function){reward=reward_function;}

private:
    std::function<void(MyML,int)> reward;

    MyML state;

};



class ActionManager
{
public:
    ActionManager();


    MyML GetRandomAction(MyML &action_list);

    MyML GetNewAction(MyML& ge, MyML& player);

    //auto func;
    //string MyML &actionon &action);
    string SaveAction(string id, MyML exp);


    void NewApplyActionResult(MyML &player, MyML action);
private:
    int actionid=0;
    int GetActionID(){return ++actionid;}

    map<string,Action> actions;



};
ActionManager& GetCurrentActionManager();

#endif // ACTIONS_H
