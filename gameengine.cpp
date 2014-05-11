#include "gameengine.h"
#include <iostream>
#include "helper.h"
#include <random>
#include <sstream>
#include "script.h"


#include "actions.h"

GameEngine::GameEngine()
{
    MyML ball("Pos.x=4;Pos.y=6;Gui.OldPos.x=4;Gui.OldPos.y=6"); //! das hier soll mal so aussehen: MyML ball("Pos[x=4;y=6];Gui[OldPos[x=4;y=6]")
    ball.AddA("color",IntToString(0xffffff));
    ball.AddA("owner","");
    data.AddE("Ball",ball);

    rnd.seed(seed_val);
    invert_positions=false;


    data.Do("Config.Actions");
    MyML& actions=data.E("Config.Actions");

    actions.E("None",MyML("action=none;prio=10"));
    actions.E("Move",MyML("action=move;prio=20"));
    //actions.E("Tackle",MyML("action=tackle;prio=80"));
    //actions.E("Dive",MyML("action=dive;prio=0")); //schwalbe
    //actions.E("Pass",MyML("action=pass;prio=30"));
    //actions.E("Cross",MyML("action=cross;prio=40"));//flanke
    //actions.E("Shot",MyML("action=shot;prio=30")); //torschuss
    //actions.E("Foul",MyML("action=foul;prio=90"));

    //! hier noch hinzufügen:
    //Kopfball -> Ball muss in der Luft sein, aber sehr hohe prio
    //Raumdeckung -> wie Tackle, aber nicht so extrem... behindert den gegenspieler nur und führt selten zu ner ballübernahme

    // die mussen mal raus:
    //actions.E("Kick",MyML("action=kick;prio=30")); // is das gleich wie shot...
    //actions.E("Takeball",MyML("action=takeball;prio=90")); // is das gleich wie tackle...


}


void GameEngine::Action()
{

    // alle OldPos erst mal setzen!
    data.E("Ball.Gui.OldPos",data.E("Ball.Pos"));

    for(MyML& player:data.E("Team1.Player")){
        player.E("Gui.OldPos",player.E("Pos"));
    }
    for(MyML& player:data.E("Team2.Player")){
        player.E("Gui.OldPos",player.E("Pos"));
    }
    // old positions gesetzt; nun aktion

    MyML ref=this->data;

    MyML PosActions;
    for(int h=0;h<GE_HEIGHT;h++){
        for(int w=0;w<GE_WIDTH;w++){
            stringstream name;
            name<<w<<","<<h;
            MyML action("Pos.x;Pos.y");
            action.AddArray("Actions");

            PosActions.AddE(name.str(),action);

        }
    }

    for(MyML& player:data.E("Team1.Player")){
        MyML ge=ref;//this->data;
        MyML a=Action(ge,player,"Team1",false);


        stringstream pos;
        pos<<a.A("OnPos.x")<<","<<a.A("OnPos.y");
        MyMLArray mymlarray(PosActions.E(pos.str()).E("Actions"));
        mymlarray.Add(a);
    }

    for(MyML& player:data.E("Team2.Player")){
        MyML ge=ref;//this->data;
        MyML a=Action(ge,player,"Team2",true);


        stringstream pos;
        pos<<a.A("OnPos.x")<<","<<a.A("OnPos.y");
        MyMLArray mymlarray(PosActions.E(pos.str()).E("Actions"));
        mymlarray.Add(a);

    }
//    cout<<"##################ACTIONS#######################"<<endl;
//    for(pair<string,MyML> e:PosActions.Elements()){
//        if(e.second.E("Actions").Elements().size()>1){
//            cout<<"-----------------------------------------------"<<endl;
//            cout<<"Name: "<<e.first<<endl;
//            for(MyML action:e.second.E("Actions")){
//                cout<<"Item"<<" -> "<<endl<<action.Info()<<endl;
//            }
//        }
//    }
//    cout<<"################################################"<<endl;



    //hier erst mal alle möglichen Actions in die richtige reihenfolge bringen (prio)
    MyML possible_actions_sorted;
    possible_actions_sorted.AddArray("SortedActions");
    for(pair<string,MyML> aaa:data.E("Config.Actions").Elements()){
        string name =aaa.first;

        MyMLArray mymlarray(possible_actions_sorted.E("SortedActions"));
        mymlarray.Add(aaa.second);
    }
    (MyMLArray(possible_actions_sorted.E("SortedActions"))).Sort("prio");
    (MyMLArray(possible_actions_sorted.E("SortedActions"))).Invert();

    for(pair<string,MyML> e:PosActions.Elements()){
        for(MyML& current_action: possible_actions_sorted.E("SortedActions")){
            MyML same_actions("type=array;size=0");

            MyML& all_actions_on_this_field=e.second.E("Actions"); //hier sind alle actions in beliebiger reihenfolge drin.

            for(MyML action:all_actions_on_this_field){

                if(!action.ExistsA("Action.action")){
                    cout<<LOG<<"Action does not exist! "<<action.Info()<<endl;
                }
                if(action.A("Action.action")==current_action.A("action")){
                    MyMLArray(same_actions).Add(action);
                }

            }

            // HIER KÖNNEN WIR ENDLICH WAS MACHEN
            if(same_actions.A("size")!="0"){
//                cout<<"Field: "<<e.first<<" action: "<<current_action.A("action");
//                for(MyML a:same_actions){
//                    cout<<" "<<a.A("id")<<",";
//                    if(a.A("Action.action")=="move"){
//                        MyML& player=GetPlayer(a.A("id"));
//                        player.SetE("Pos",a.E("Action.Pos"));
//                    }
//                }
//                cout<<endl;

                ExecuteActions(current_action.A("action"),same_actions,e.second.E("Actions"));
            }
        }
    }





}

void GameEngine::ExecuteActions(string name, MyML &actions,MyML& all_actions)
{

    /*
    if(name!="none"){
        if(INT(all_actions.A("size"))>1){
            cout<<"Action: "<<name<<" other actions: "<<endl;
            for(MyML& oa:all_actions){
                cout<<" -> "<<oa.Info()<<endl;
            }
        }
    }
    */
    //erst mal alles schlecht machen
    cout<<"CURRENT BALL OWNER: "<<data.A("Ball.owner")<<endl;
    for(MyML& a:actions){
        a.Do("Result;Result.reward=0");// je nach ballposition und je nach InitPos
    }

    if(name=="none"){
        for(MyML& a:actions){
            MyMLCalc(a).Add("Result.reward","-1.5"); // je nach ballposition und je nach InitPos

        }
    }
    if(name=="move"){
        for(MyML& a:actions){

            MyML& player=GetPlayer(a.A("id"));
            if(player.E("Pos")==a.E("Action.Pos")){
                //! extrem negative reward!
            }
            player.SetE("Pos",a.E("Action.Pos"));



            MyMLCalc(a).Add("Result.reward","-1"); //je nach ballposition
            MyMLCalc(a).Add("Result.reward","-1"); //je nach InitPos
        }
    }
    if(name=="kick"||name=="shot"){
        for(MyML& a:actions){
            if(data.A("Ball.owner")==a.A("id")){

                data.A("Ball.owner","");
                data.E("Ball.Pos",a.E("Action.Pos"));
                MyMLCalc(a).Add("Result.reward","1");
            }else{
                MyMLCalc(a).Add("Result.reward","-2");
            }
        }
    }

    if(name=="takeball" || name=="tackle"){
        for(MyML& a:actions){
            if (data.A("Ball.owner") == ""){
                MyML& player=GetPlayer(a.A("id"));
                if(player.E("Pos")==data.E("Ball.Pos")){
                    data.A("Ball.owner",player.A("id"));
                    MyMLCalc(a).Add("Result.reward","10");
                }else{
                    MyMLCalc(a).Add("Result.reward","-1");
                }

            }else{
                cout<< "Owner "<< data.A("Ball.owner")<<endl;
                MyMLCalc(a).Add("Result.reward","-1");

            }
            //a.Do("Result;Result.reward=10");
        }

    }

    if(name=="dive"){ //schwalbe
        for(MyML& a:actions){
            //man verliert auf jeden fall den ball
            a.A("Result.reward","10");
        }

    }
    if(name=="pass"){
        for(MyML& a:actions){
            if(data.A("Ball.owner")==a.A("id")){
                data.A("Ball.owner","");
                data.E("Ball.Pos",a.E("Action.Pos"));
                MyMLCalc(a).Add("Result.reward","1");
            }else{
                MyMLCalc(a).Add("Result.reward","-2");
            }

        }

    }
    if(name=="cross"){ //flanke
        for(MyML& a:actions){
            if(data.A("Ball.owner")==a.A("id")){
                data.A("Ball.owner","");
                data.E("Ball.Pos",a.E("Action.Pos"));
                MyMLCalc(a).Add("Result.reward","1");
            }else{
                MyMLCalc(a).Add("Result.reward","-2");
            }
        }
    }

    if(name=="foul"){
        //man bekommt auf jeden fall den ball.
        //
        for(MyML& a:actions){
            a.A("Result.reward","-10");
        }
    }




    //für jede actions muss power decreased werden..
    //dafür muss jeder spieler auch ne eigenschaft haben
    for(MyML& a:actions){
        //player.A("Power",player.A("Power")*a.A("PowerModifier"));
    }

    for(MyML& a:actions){
        if(a.ExistsE("Result")){
            GetCurrentActionManager().NewApplyActionResult(GetPlayer(a.A("id")),a); //Hier geht wieder was mit den positionen durcheinander

        }else{
            cout<<"Kein Result hinzugefügt!"<<endl;
        }
    }

}

MyML GameEngine::Action(MyML& ge,MyML &player, string team, bool invert_pos)
{
    invert_positions=invert_pos;


    if(invert_pos){
        //hier müssen jetzt alle Positionen aus ge vom anderen Team invertiert werden!!!
        //außerdem der Ball
        for(int i=0;i<INT(ge.A("Team1.Player.size"));i++){
            MyML& player=ge.E("Team1.Player").E(STR(i));
            player.E("Pos",invert(player.E("Pos")));
            player.E("InitPos",invert(player.E("InitPos")));

        }
        ge.E("Ball.Pos",invert(ge.E("Ball.Pos")));

    }


    MyML pa=PlayerAction(ge,player);
    pa.AddA("id",player.A("id"));
    pa.AddE("OnPos",INV(player.E("Pos")));

    return pa;
}

MyML GameEngine::Draw()
{
    MyML myml("T1;T2");


    myml.E("T1").AddArray("Player");


    for(int i=0;i<StringToInt(data.A("Team1.Player.size"));i++){
        MyML& p=data.E("Team1.Player").E(IntToString(i));
        MyML player("id=@0|id;color=@0|color;Pos=@0|Pos",{p});


        player.Do("Gui.OldPos=@0|Gui.OldPos",{p} );
        myml.E("T1.Player").Type<MyMLArray>().Add(player);
    }
    for(int i=0;i<StringToInt(data.A("Team2.Player.size"));i++){
        MyML& p=data.E("Team2.Player").E(IntToString(i));
        MyML player("id=@0|id;color=@0|color;Pos=@0|Pos",{p});


        player.Do("Pos.x=null;Pos.y=null;");
        player.E("Pos",invert(data.E("Team2.Player").E(STR(i)).E("Pos")));
        player.E("Gui.OldPos",invert(data.E("Team2.Player").E(STR(i)).E("Gui.OldPos")));
        myml.E("T1.Player").Type<MyMLArray>().Add(player);
    }


    myml.CpE(data,"Ball");


    //cout<<myml.E("T1").E("Player").attributes["posy"]<<endl;
    return myml;
}

MyML &GameEngine::GetPlayer(string id)
{
    for(MyML& p:data.E("Team1.Player")){
        if(p.A("id")==id)
            return p;
    }
    for(MyML& p:data.E("Team2.Player")){
        if(p.A("id")==id)
            return p;
    }
    throw GEException("GetPlayer: id does not exist");
}

int GameEngine::GetRND(int s, int e)
{
    std::uniform_int_distribution<uint32_t> uint_dist10(s,e);
    uint test=uint_dist10(rnd);
    return test;
}

void GameEngine::Start()
{
    //hier pos.x=Init.pos.x für jeden spieler
}

MyML GameEngine::INV(MyML &pos)
{
    if(invert_positions){
        //cout<<"INVERT POSITIONS!!!"<<endl;
        //cout<<pos.Info()<<endl;
        return invert(pos);
    }else{
        return pos;
    }
}

MyML GameEngine::invert(MyML &pos)
{

    MyML newpos("x=0;y=0");
    newpos.A("x", STR(GE_WIDTH-INT(pos.A("x"))));
    newpos.A("y", STR(GE_HEIGHT-INT(pos.A("y"))));

    //MyML newpos("x=@%s|0;y=@%s|1",vector<MyML>(),{STR(GE_WIDTH-INT(pos.A("x"))),STR(GE_HEIGHT-INT(pos.A("y")))});
    return newpos;
}
