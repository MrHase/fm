#include "player.h"
#include "gameengine.h"
#include "pos.h"
#include "helper.h"
#include <iostream>
#include "script.h"

#include "brain.h"

#include "rnd.h"
#include "actions.h"
using namespace std;

ActionManager actionmanager;

Player::Player()
{
    //data.AddA("id",IntToString(id));
    rnd.seed(seed_val);
}



MyML PlayerAction(MyML &ge, MyML &player)
{
    MyML& data=player;
    Pos ball(ge.E("Ball.Pos"));
    Pos mypos(data.E("Pos"));
    MyML ret("Action");
    string id=data.A("id");


    if(ball==mypos){
        if(ge.A("Ball.owner")==data.A("id")){
            cout<<id<<": ich hab den ball"<<endl;

            ret.E("Action",ge.E("Config.Actions").E("Kick"));
            //Pos dst(ge.GetRND(1,GE_WIDTH-1),ge.GetRND(1,GE_HEIGHT-1));
            Pos dst(GetRND(1,GE_WIDTH-1),GetRND(1,GE_HEIGHT-1));
            ret.E("Action.Pos",dst.ToMyML().E("Pos"));

        }else{
            ret=NewNewPlayerAction(ge,player);

            //old:
//            cout<<player.A("id")<<": Ich will den ball"<<endl;
//            ret.E("Action",ge.E("Config.Actions").E("Takeball"));
        }

    }else{

        MyML mp_ball=GetMovePoints(mypos,ball);

        if(StringToInt(mp_ball.A("size"))>3){

            MyML mp_init_pos=GetMovePoints(mypos,data.E("InitPos"));
            if(mp_init_pos.A("size")!="0"){
                ret.E("Action",ge.E("Config.Actions").E("Move"));
                ret.E("Action").E("Pos",mp_init_pos.E("0.Pos"));
            }else{
                //ret.E("Action",ge.E("Config.Actions").E("None"));
                ret=NewNewPlayerAction(ge,player);
            }

        }else{
            /*
            MyML action=ge.E("Config.Actions").E("Move");
            //cout<<"INFO: "<<mp.Info()<<endl;
            action.E("Pos",mp_ball.E("0.Pos"));
            ret.E("Action",action);
            */
            ret=NewNewPlayerAction(ge,player);
        }
    }

    return ret;

}

MyML NewNewPlayerAction(MyML &ge, MyML& player){
    DisableCout(player.A("id"));
    MyML ret("Experience");

    stringstream str;
    str<<"Pos.x="<<player.A("Pos.x")<<";";
    str<<"Pos.y="<<player.A("Pos.y")<<";";

    string ball_distance="yes";
    str<<"ball="<<ball_distance; //muss später mal ne position sein...

    MyML situation(str.str());
    ret.AddE("Experience.C1",situation);


    bool try_something_new=RandomSucceed(player.A("Stats.curiosity"));

    try_something_new=true;



    if(try_something_new){
        cout<<"ill try something new"<<endl;

        MyML action=actionmanager.GetNewAction(ge,player);
        //MyML action=ge.E("Config.Actions.None"); //!


        ret=action;

    }else{

        //MyML action=ge.E("Config.Actions.None"); //!
        MyML action=ge.E("Config.Actions.Move");
        ret.AddE("Experience.A1",action);
        string sub_str="Pos.x=10;Pos.y=10";
        action.Do(sub_str);
        ret.AddE("Experience.A2",MyML(sub_str));


        ret.AddE("Experience.C2",MyML("AnySituation"));



        ret.E("Action",action);
    }



    //ret.AddE("Action.Exp",MyML(str.str()));
    //cout<<"asdf "<<ret.Info()<<endl;
    RestoreCout();
    return ret;
}

MyML NewPlayerAction(MyML &ge, MyML &player)
{

    DisableCout(player.A("id"));

    MyML ret;
    stringstream str;
    str<<"Pos.x="<<player.A("Pos.x")<<";";
    str<<"Pos.y="<<player.A("Pos.y")<<";";

    //string ball_distance=GetMovePoints(Pos(player.E("Pos")),Pos(ge.E("Ball.Pos"))).A("MP.size");
    string ball_distance="yes";
    str<<"ball="<<ball_distance; //muss später mal ne position sein...
    //str<<"InitPosDistance="<<"6"<<endl;

    MyML matcharray=MyMLArray(player.E("Experience")).ItemsMatch(MyML(str.str()));
    cout<<Info(player)<<"Kenn ich die Situation: ";
    bool try_something_new=RandomSucceed(player.A("Stats.curiosity"));


    if(matcharray.A("size")=="0"){
        cout<<"Nö!"<<endl;
        MyML action=ge.E("Config.Actions.None"); //!
        //MyML action=GetRandomAction(ge);
        cout<<Info(player)<<"Ich versuch mal: "<<action.A("action")<<endl;


        //also mal initialisieren:
        MyML exp(str.str());
        exp.AddArray("Actions");


        exp.E("Actions").Type<MyMLArray>().Add(MyML("action=@0|action;rewards=0",{action}));

        MyMLArray(player.E("Experience")).Add(exp);






        //action.AddE("Exp",MyML(str.str()));
        ret.E("Action",action);

    }else if(try_something_new){
        cout<<"Ja, aber ich will was neues ausprobieren!"<<endl;
        MyML action=actionmanager.GetRandomAction(ge);
        action.AddA("rewards","0");

        //! bah! das ist unschön!
        if(action.A("action")=="move"){
            int x=1-GetRND(0,2);
            int y=1-GetRND(0,2);
            x=INT(player.A("Pos.x"))+x;
            y=INT(player.A("Pos.y"))+y;
            if(x<1)x=1;
            if(x>GE_WIDTH-1)x=GE_WIDTH-1;
            if(y<1)y=1;
            if(y>GE_HEIGHT-1)y=GE_HEIGHT-1;

            MyML pos;
            pos.AddA("x",STR(x));
            pos.AddA("y",STR(y));
            action.AddE("TestPos",pos);
        }

        //cout<< MyMLArray(player.E("Experience")).Get(0).Info()<<endl;

        player.E("Experience").ForEachMatch(MyML(str.str()),[&](MyML& m){

            MyML match("action=@0|action",{action});

            if(MyMLArray(m.E("Actions")).ItemsMatch(match).Type<MyMLArray>().Size()==0){
                //ok, das die action wurde hier noch nie ausprobiert... also adden
                //cout<<"bah: "<<m.Info()<<endl;
                MyMLArray(m.E("Actions")).Add(action);
            }
        });
        //action.AddE("Exp",MyML(str.str()));
        ret.E("Action",action);

    }else{
        cout<<"Yap!"<<endl;

        // ich kenn die situation und will auch nichts neues ausprobieren...
        // wir nehmen den besten wert!

        MyML action;


        for(MyML& e:player.E("Experience")){
            if(e.Match(str.str())){
                cout<<Info(player)<<"Exp die ich mit der Situation("<<str.str()<<") habe: "<<endl;
                for(MyML& e2:e.E("Actions")){
                    cout<<e2.A("action")<<"["<<e2.A("rewards")<<"] ";
                }
                cout<<endl;

                MyMLArray(e.E("Actions")).Sort("rewards"); //sortieren
                MyMLArray(e.E("Actions")).Invert(); //hohe an den Anfang


                MyML match("rewards=@0|rewards",{MyMLArray(e.E("Actions")).Get(0)});

                MyML highest_ratings=MyMLArray(e.E("Actions")).ItemsMatch(match); //alle raussuchen die so ein hohe rating haben
                int i=GetRND(0,MyMLArray(highest_ratings).Size()-1);

                //! das geht so leider noch nicht. Das problem ist: Wie behandeln wir die Pos z.B. im Move...
                //! da muss noch ein zweiter layer der entscheidung her...
                action=MyMLArray(highest_ratings).Get(i);


            }
        }

        ret.E("Action",action);

    }


    try{
        ret.E("Action.Pos",ret.E("Action.TestPos"));
    }catch(MyMLException& me){
        cout<<"cout "<<me.Error()<<endl;
    }



    ret.AddE("Action.Exp",MyML(str.str()));
    //! sollte man hier vllt noch mal den reward und so rauslöschen? alle unrelevanten informationen halt
    cout<<Info(player)<<"Meine Entscheidung fiel auf: "<<ret.A("Action.action")<<endl;
    RestoreCout();
    return ret;
}



void ApplyActionResult(MyML& player,MyML action){


    if(!action.ExistsE("Action.Exp")){
        //! das hier passiert nur bei den nicht KI-entscheidungen... daher ist es später mal ein bug wenn die EXP fehlt
        //cout<<Info(player)<<"Ok, kein Exp enthaltne... bug... "<<endl;
        return;
    }

    DisableCout(action.A("id"));



    cout<<Info(player)<<"Result erhalten: "<<action.A("Action.action")<<": "<<action.A("Result.reward")<<endl;

    MyML exp=action.E("Action.Exp");

    MyML matcharray=MyMLArray(player.E("Experience")).ItemsMatch(exp);
    if(matcharray.A("size")=="0"){
        cout<<Info(player)<<"Eintrag existiert noch nicht."<<endl;
        cout<<Info(player)<<"Das ist ein fehler!!!"<<endl;
        exit(1);
    }else{
        //cout<<Info(player)<<"Exp Existiert schon.. eintrag bearbeiten.."<<endl;

        player.E("Experience").ForEachMatch(exp,[&](MyML& e){
            MyML match("action=@0|Action.action",{action});
            if((MyMLArray(e.E("Actions")).ItemsMatch(match)).A("size")=="0"){
                cout<<Info(player)<<"Action "<<match.A("action")<<" does not exist"<<endl;
                //exit(1);
            }

            e.E("Actions").ForEachMatch(match,[&](MyML& m){
                MyMLCalc(m).Add("rewards",action.A("Result.reward"));
            });

        });


    }

    RestoreCout();
}



int Player::GetRND(int s, int e)
{
    if(s==e){
        return s;
    }

    //s und e inklusive: s=3, e=6 -> <3,4,5,6> möglich!
    std::uniform_int_distribution<uint32_t> uint_dist10(s,e);
    uint test=uint_dist10(rnd);
    return test;
}
bool RandomSucceed(string value){
    int v=INT(value);
    int rnd=GetRND(0,100);
    return (v<=rnd);
}

string Info(MyML& player){
    stringstream str;
    str<<player.A("id")<<"("<<player.A("Pos.x")<<","<<player.A("Pos.y")<<"): ";
    return str.str();
}



std::streambuf *backup;
void SaveCout()
{
     backup= std::cout.rdbuf();
}


void RestoreCout()
{
    std::cout.rdbuf(backup);
}


void DisableCout(string id)
{
    SaveCout();
    if(id!=PLAYERID){
        std::cout.rdbuf(0);
    }
}
