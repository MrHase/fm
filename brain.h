#ifndef BRAIN_H
#define BRAIN_H
#include "myml.h"
#include <vector>

#include <iostream>
#include "helper.h"
using namespace std;
//structure:
/*
ROOT{
    Situations<Array>{
        bedingung1,
        bedingung2,
        Actions<Array>{
            Exp{
                success,
                count,
                ...
            }
            action{
                USERSTUFF
            }
        }
    }
}
*/


class Action;
class Situation;
class Revard;

class Brain:public MyMLClass{
//class Brain{
public:
//    Brain(MyML& myml):ref(myml){}
    Brain(MyML& myml):MyMLClass(myml){}
    Brain(){ref=Create();}

    MyML FindAppropriateAction(MyML &situation);
    static MyML Create();
    void AddSituation(Situation situation);
    void AddAction(Situation situation_, Action action_);

    void AddExperience(MyML& situation,Action a, Revard r);


private:
//    MyML& ref;
    MyML *GetSituation(MyML& situation);
    MyML *GetAction(MyML &situation, MyML& action);
};



/*
class Action:MyMLClass{
public:
    Action(){ref=Create();}
    Action(MyML& myml):MyMLClass(myml){}//{ref=myml;}

    static MyML Create(){return MyML("Action");}

    MyML& AddAction(MyML myml){ref.E("Action",myml);return ref;}

    void AddSub(MyML brain){ref.AddE("Sub",brain);}

    void Revard(int r);


    MyML& ToMyML(){return ref;}

};
*/



class Situation:public MyMLClass{
public:

    Situation(MyML &myml):MyMLClass(myml){}
    Situation(){ref=Create();}

    MyML Create(){return MyML("Characteristics");}

    MyML& AddCharacteristics(MyML charas){
        for(auto a:charas.Attributes()){
            ref.E("Characteristics").AddA(a.first,a.second);
        }
        return ref;
    }

};

class Revard:public MyMLClass{
public:
    Revard(MyML& myml):MyMLClass(myml){}
    Revard(){ref.AddA("revard","0");}
    Revard(int r){ref.AddA("revard",STR(r));}

    void R(int r){ref.A("revard",STR(r));}
    int R(){return INT(ref.A("revard"));}
};

class BrainCell:public MyMLClass{
public:
    BrainCell(MyML& myml):MyMLClass(myml){}
    BrainCell(){ref=MyML("");}

    void AddSituation(MyML situation, MyML action, int revard);
    MyML& GetAction(MyML situation, MyML action);

    MyML GetBestAction(MyML situation);

    void Print(string offset="");

};


void BrainTest();
#endif // BRAIN_H
