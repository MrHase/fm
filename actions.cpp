#include "actions.h"
#include "rnd.h"
#include "helper.h"
#include <sstream>

#include "brain.h"
using namespace std;

ActionManager* current_action_manager=NULL;

ActionManager &GetCurrentActionManager()
{
    return (*current_action_manager);
}


ActionManager::ActionManager()
{
    if(current_action_manager!=NULL){
        cout<<"HIER NE EXCEPTION"<<endl;

    }
    current_action_manager=this;
}

MyML ActionManager::GetRandomAction(MyML &action_list){
    map<string,MyML> action_map=action_list.Elements();
    vector<string> actions;
    for(auto a:action_map){
        actions.push_back(a.first);
    }
    int index=GetRND(0,actions.size()-1);
    return action_list.E(actions[index]);

}

MyML GetRandomAction2(MyMLArray array){
    int index=GetRND(0,array.Size()-1);
    return array.Get(index);

}

int CheckLevel(MyML& myml,int l){

}


void ActionMove(MyML& ge, MyML& player,MyML& bla){

    MyML moveactions_("size=0");
    MyMLArray moveactions(moveactions_);
    moveactions.Add(MyML("action=MoveToBall"));
    //moveactions.Add(MyML("action=MoveToMyPos"));

    //moveactions.Add(MyML("action=MoveToEnemy"));
    //moveactions.Add(MyML("action=MoveAwayFromEnemy"));
    //moveactions.Add(MyML("action=MoveToGoal"));


    MyML subaction=GetRandomAction2(moveactions);
    cout<<"subaction: "<<subaction.A("action")<<endl;

    MyML pos("x=10;y=10");

    string sub=subaction.A("action");
    if(sub=="MoveToBall"){

        MyML move_to_ball_positions=GotoPos(player.E("Pos"),ge.E("Ball.Pos"));

        if(MyMLArray(move_to_ball_positions).Size()>0){
            pos=MyMLArray(move_to_ball_positions).Get(0);
        }else{
            pos=player.E("Pos");
        }

    }


    MyML& main_action=bla.E("Action");


    main_action.E("Pos",pos);



    MyML& situation=bla.E("Experience");
    situation.E("A2",subaction);
    situation.E("C2",MyML("AnySituation"));

}

MyML ActionManager::GetNewAction(MyML& ge,MyML& player){


    int id_=GetActionID();
    //actions[id_]=testaction;
    string id=STR(id_);

    MyML ret("Experience");

    MyML action=GetRandomAction(ge.E("Config.Actions"));

    //MyML action=ge.E("Config.Actions.Move");



    ret.AddE("Action",action);
    ret.AddE("Experience.A1",action);
    ret.AddA("Experience.ID",id);
    ret.AddA("Experience.BrainID","BallDistance");

    stringstream str;

    string ball_distance=STR(Distance(player.E("Pos"),ge.E("Ball.Pos")));
    str<<"ball distance ="<<ball_distance; //muss später mal ne position sein...

    MyML s1(str.str());
    ret.AddE("Experience.C1",s1);


    if(action==ge.E("Config.Actions.Move")){
        ActionMove(ge,player,ret);

    }else if(action==ge.E("Config.Actions.None")){

    }else{
        cout<<LOG<<"Action not implemented: "<<action.A("action")<<endl;
    }
    //actions[id]=ret;

    SaveAction(id,ret);

    return ret;

}

string ActionManager::SaveAction(string id,MyML exp)
{
    Action a;
    a.State(exp);

    actions[id]=a;
    return "";

}



void ActionManager::NewApplyActionResult(MyML& player,MyML action){





    if(action.ExistsE("Experience")){
        const string id=action.A("Experience.ID");
        const string brain_id=action.A("Experience.BrainID");
        cout<<"ID: "<<id<<" Brain: "<<brain_id<<endl;

        int i=1;
        //BrainCell c(player.E("Brain.C1"));
        MyML* brain=&player.E("Brain.C1");

        MyML& exp=actions[id].State().E("Experience");

        while(exp.ExistsE("C"+STR(i))){


            BrainCell c(*brain);

            c.AddSituation(exp.E("C"+STR(i)),exp.E("A"+STR(i)),INT(action.A("Result.reward")));

            brain=&c.GetAction(exp.E("C"+STR(i)),exp.E("A"+STR(i))).E("Sub");
            i++;
        }

        /*
        while(action.ExistsE("Experience.C"+STR(i))){


            BrainCell c(*brain);

            c.AddSituation(action.E("Experience.C"+STR(i)),action.E("Experience.A"+STR(i)),INT(action.A("Result.reward")));

            brain=&c.GetAction(action.E("Experience.C"+STR(i)),action.E("Experience.A"+STR(i))).E("Sub");
            i++;
        }
        */
        //c.AddSituation(action.E("Experience.C1"),action.E("Action"),INT(action.A("Result.reward")));

    }


    cout<<"Brain >>>>>>>>>>>>>>>>>>"<<endl;
    BrainCell(player.E("Brain.C1")).Print();
    cout<<"Brain <<<<<<<<<<<<<<<<<<"<<endl;

}


