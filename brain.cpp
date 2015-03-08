#include "brain.h"
#include <iostream>
#include "player.h"
#include "helper.h"
using namespace  std;

#include "../xmp/xmplib.hpp"
void Viewer(xmp::XmpConnector& xmp,string name,MyML& myml){
    MyML msg;
    msg.AddA("name",name);
    msg.E("data",myml);

    xmp.Send("MyMLViewer",XML(msg).Out());

}


void BrainTest(){
    //extra test:

    MyML as("hallo=4");
    as.As([](MyMLCalc t){t.Add("hallo","5");t.Add("hallo","2");});
    cout<<as.Info()<<endl;

    cout<<"BRAINTEST"<<endl;
    try{
        /*
        xmp::XmpConnector xmp("127.0.0.1",30000);
        xmp.Register("FM","Brain");
        */

        Player rng;



        BrainCell bc1;
        bc1.Base().Do("type=array;size=0");

        int revards[6]={1,2,3,4,5,6};
        for(int i=0;i<10;i++){
            //brain.AddSituation(Situation().AddCharacteristics(MyML("posx=@0",{},{STR(10)})));
            //brain.AddSituation(Situation().AddCharacteristics(MyML("posx=12")));
            //brain.AddSituation(Situation().AddCharacteristics(MyML("posx=13")));


            int a=rng.GetRND(1,6);
            cout<<"RNG: "<<a<<endl;
            bc1.AddSituation(MyML("x=1;y=1"),MyML("action=@%s|0",{},{STR(a)}),1);
            //cout<<bc1.Base().Info()<<endl;

        }
        cout<<bc1.Base().Info()<<endl;
        cout<<"BEST action: "<<endl;

        MyML best_action=bc1.GetBestAction(MyML("x=1;y=1"));
        cout<<best_action.Info()<<endl;


/*

        cout<<"BRAIN: "<<endl<<Player.Info()<<endl;
        MyML test("a0.test=5");
        Viewer(xmp,"test",test);

        Viewer(xmp,"Brain",Player);

        MyML bestaction=brain.FindAppropriateAction(Situation().AddCharacteristics(MyML("pos=1")));
        Viewer(xmp,"BestMatch2",bestaction);

        MyML find("Characteristics.posx=10;Sub.Characteristics.subc=10");
        MyML s=brain.FindAppropriateAction(find);



        MyML xmlin;
        XML(xmlin).In(XML(s).Out());
        cout<<"GOGO "<<xmlin.Info()<<endl;


        MyML msg("name=FoundAction");
        msg.AddE("data",s);
        xmp.Send("MyMLViewer",XML(msg).Out());

*/



/*
        string json=JSON(s).Out().toStyledString();
        cout<<"JSON:"<<json<<endl;
        MyML j;
        JSON(j).In(json);
        cout<<j.Info()<<endl;
        */

//    }catch(MyMLException& me){
//        cout<<me.Error()<<endl;
    }catch(xmp::Exception& e){
        cout<<e.GetError()<<endl;
    }
}


void BrainCell::AddSituation(MyML situation, MyML action, int revard)
{
    //Wenn situation noch nicht im array ist, dann hinzufügen

    if(!MyMLMap(ref).Include(situation)){
        MyML action_array("type=array;size=0");
        //cout<<"Adde situation "<<situation.Info()<<endl;

        //hier vllt noch weiter abstrahieren
        MyML brain_action;
        brain_action.AddE("Action",action);
        brain_action.AddA("revard",STR(revard));
        brain_action.AddE("Sub",MyML("size=0;type=array"));


        MyMLArray(action_array).Add(brain_action);

        MyMLMap(ref).Add(situation,action_array);


    }else{


        MyMLArray action_array(MyMLMap(ref).Get(situation));

        bool added=false;
        for(auto& i:action_array){
            //cout<<"Already existing: "<<i.Info()<<endl;
            if(i.E("Action")==action){
                //cout<<"Action "<<action.Info()<<" a1lready existing: "<<i.Info()<<endl;
                MyMLCalc(i).Add("revard",STR(revard));
                added=true;
            }
        }

        if(!added){
            //cout<<"--"<<endl;
            MyML brain_action;
            brain_action.AddE("Action",action);
            brain_action.AddA("revard",STR(revard));
            brain_action.AddE("Sub",MyML("size=0;type=array"));
            action_array.Add(brain_action);
        }

    }


}

MyML &BrainCell::GetAction(MyML situation,MyML action)
{
    if(!MyMLMap(ref).Include(situation)){
        //ERROR
    }else{
        MyMLArray action_array(MyMLMap(ref).Get(situation));

        for(auto& i:action_array){
            if (i.E("Action")==action){
                //cout<<"AHA"<<endl;
                return i;
            }
        }

    }
    throw MyMLException("Return value does not exist");
}

MyML BrainCell::GetBestAction(MyML situation)
{
    if(!MyMLMap(ref).Include(situation)){
        //ERROR
    }else{
        MyMLArray action_array(MyMLMap(ref).Get(situation));

        MyML highest("revard=0");

        for(auto& i:action_array){
            if(MyMLCalc(i).G("revard",(highest.A("revard")))){
                highest=i;
            }
        }
        return highest;
    }
    return MyML();
}

void BrainCell::Print(string offset)
{
    vector<MyML> keys=MyMLMap(ref).Keys();
    for(auto k:keys){
        //cout<<offset<<"Bla" <<k.Info();
        //cout<<offset<<"L1 Situation: ball="<<k.A("ball")<<" "<<k.A("Pos.x")<<","<<k.A("Pos.y")<<endl;
        cout<<offset<<"Situation: "<<MyMLPrint(k).NoReturn()<<endl;

        MyMLArray actions(MyMLMap(ref).Get(k));
        for(auto a:actions){
            cout<<offset<<"-> Action: "<<MyMLPrint(a.E("Action")).NoReturn()<<"  r="<<a.A("revard")<<endl;
            BrainCell(a.E("Sub")).Print(offset+"++");

        }
        cout<<endl;

        //cout<<" -> Action: "<<MyMLMap(player.E("Brain.C1")).Get(k).A("action")<<" revard: "<<


    }
    //cout<<endl;
}
