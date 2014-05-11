#ifndef MYML_H
#define MYML_H

#include <map>
#include <string>
#include <list>
#include <vector>
#include <functional>
#include <sstream>



using namespace std;
/*
void CheckObject(MyML& obj,MyML& c){

}
*/

class MyML;
template<typename T>
class Iter
{
    public:
    Iter ( T& p_vec, int pos)
        : _pos( pos )
        , _p_vec( p_vec )
    { }

    // these three methods form the basis of an iterator for use with
    // a range-based for loop
    bool
    operator!= (Iter& other) const
    {
        return _pos != other._pos;
    }

    // this method must be defined after the definition of IntVector
    // since it needs to use it
    //T& operator* (){stringstream ss;ss<<_pos;return _p_vec.E(ss.str());}
    MyML& operator* (){return _p_vec.IterationPos(_pos);}


    const Iter& operator++ ()
    {
        ++_pos;
        // although not strictly necessary for a range-based for loop
        // following the normal convention of returning a value from
        // operator++ is a good idea.
        return *this;
    }

    private:
    int _pos;
     T& _p_vec;
};

class Iterable{
    //virtual MyML& operator* ()=0;
    virtual MyML& IterationPos(int pos)=0;
};

class MyML:Iterable
{
public:

    virtual MyML& IterationPos(int pos){stringstream ss;ss<<pos;return E(ss.str());}
    MyML();
    MyML(string str);
    MyML(string str, map<string, MyML> references, map<string, string> strings={});
    MyML(string str, vector<MyML> references,vector<string> strings={});

    virtual ~MyML(){}
    void Do(string str);
    void Do(string str, vector<MyML> references,vector<string> strings={});
    void Do(string str, map<string, MyML> references, map<string, string> strings);

    MyML& E(string k);
    void E(string k,MyML v);
    void SetE(string k, MyML v);
    void SingleE(string k);
    void SingleE(string k, MyML v);
    void DelE(string k);

    bool Empty();


    void AddArray(string k, int size=0);
    void AddToArray(string k, MyML v);
    vector<MyML> Array(string k);

    MyML& ArrayAtIndex(int i);
    MyML& ArrayAdd();
    MyML& ArrayAdd(int index,MyML v);

    MyML& AddE(string k, MyML v);
    MyML& AddE(string k);

    bool ExistsE(string k);
    void CheckE(string k);


    void CpE(MyML other, string k);
    void CpE(MyML other, list<string> keys);


    string A(string k);

    void A(string k, string v);
    MyML *AddA(string k, string v); //! return wert ist mist!

    void DelA(string k);

    template<typename T>
    T Type() {return T(*this);}


    template<typename Func>
    MyML& As(Func func) {func(*this);return (*this);}
/*
    template<typename T,typename Func>
    MyML& As(Func func) {func(T(*this));return (*this);}
*/
    bool ExistsA(string k);
    void CheckA(string k);

    bool Equals(MyML& o);

    void CpA(MyML other, string k);
    void CpA(MyML other, list<string> keys);

    map<string, string>& Attributes(){return attributes;}
    map<string, MyML>& Elements(){return elements;}

    string Info(string offset ="");

    //die hier machen keinen kompletten vergleich
    bool Match(MyML myml);
    MyML &FindMatch(MyML myml);
    bool ExistsMatch(MyML myml);
    template<typename Func>
    void ForEachMatch(MyML myml, Func func){
        for(auto& e:elements){
            if(e.second.Match(myml)){
                func(e.second);
            }
        }
    }


    template<typename Func>
    void ForEachElement(Func func){
        for(auto& e:elements){
            func(e.second);
        }
    }



    //iteration:
    Iter<MyML> begin();
    Iter<MyML> end();
private:
    string* GetAttribute(string k);
    MyML* GetElement(string k);


    map<string, string> attributes;
    map<string, MyML> elements;
};

bool operator ==(MyML& first,MyML& second);

class MyMLException{
public:
    MyMLException(string e){this->e=e;}
    string Error(){return e;}
private:
    string e;

};


/*
 * Hier kommen die extra Classen...
 *
 *
 *
 *
 */


class MyMLClass{
public:
    MyMLClass():ref(mem){}
    MyMLClass(MyML& myml):ref(myml){}

    MyML& Base(){return ref;}
protected:
    MyML &ref;
    MyML mem;
};

class MyMLArray:Iterable{
public:
    virtual MyML& IterationPos(int pos){stringstream ss;ss<<pos;return array.E(ss.str());}
    MyMLArray(MyML& myml);
    void AddAtIndex(int index, MyML data);
    void Add(MyML data);

    void Sort(string a);
    void Invert();

    MyML& Get(int index);

    MyML ItemsMatch(MyML myml);

    template<typename Func>
    void ForEachItem(Func func){
        for(auto& e:array.Elements()){
            func(e.second);
        }
    }
    template<typename Func>
    void ForEachMatch(MyML myml, Func func){
        for(auto& e:array.Elements()){
            if(e.second.Match(myml)){
                func(e.second);
            }
        }
    }

    Iter<MyMLArray> begin();
    Iter<MyMLArray> end();

    int Size();

private:
    MyML& array;
};



class MyMLMap:public MyMLClass{
public:
    MyMLMap(MyML& myml):MyMLClass(myml){}
    //MyMLMap(){ref=MyML();}

    void Add(MyML k, MyML v);
    MyML &Get(MyML k);
    vector<MyML> Keys();

    bool Include(MyML k);

};

class MyMLPrint:public MyMLClass{
public:
    MyMLPrint(MyML& myml):MyMLClass(myml){}
    //MyMLMap(){ref=MyML();}

    string NoReturn();

};

class MyMLPos:public MyMLClass{
public:
    MyMLPos(MyML& myml):MyMLClass(myml){}

    int X();
    MyMLPos& X(int x);

    int Y();
    MyMLPos& Y(int y);
};

class MyMLCalc{
public:
    MyMLCalc(MyML& myml);
    void Add(string a,string i);
    void Sub(string a,string i);
    void Mult(string a,string i);
    void Div(string a,string i);


    bool G(string a, string i);
    bool GE(string a,string i);

    bool L(string a,string i);
    bool LE(string a,string i);

    double Get(string a);
    void Set(string a, string i);
private:
    MyML& calc;
};



#include <jsoncpp/json/json.h>
class JSON:public MyMLClass{
public:
    JSON(MyML &myml):MyMLClass(myml){}

    Json::Value Out(){
        return Out(ref);
    }


    Json::Value Out(MyML& myml){
        Json::Value root;
        for(auto a:myml.Attributes()){
            root[a.first]=a.second;
        }

        for(auto e:myml.Elements()){
            root[e.first]=Out(e.second);
        }
        return root;
    }

    void In(string json){
        Json::Value root;   // will contains the root value after parsing.
        Json::Reader reader;
        bool parsingSuccessful = reader.parse( json, root );
        if ( !parsingSuccessful )
        {
            //! throw Exception oder so
            std::cout  << "Failed to parse configuration\n"
                       << reader.getFormatedErrorMessages();
            return;
        }
        In(ref,root);
    }
    void In(MyML& myml,Json::Value root){

        for(string s: root.getMemberNames()){
            //cout<<"MEMBER: "<<s<<endl;
            Json::Value m=root[s];
            if(m.isObject()){
                myml.AddE(s);
                In(myml.E(s),m);
                //cout<<"Object: "<<s<<endl;
            }else if(m.isConvertibleTo(Json::stringValue)){
                myml.AddA(s,m.asString());
            }else{
                cout<<"UNHANDLED: "<<s<<endl;
            }
        }
    }

};


//#include "../rapidxml-1.13/rapidxml.hpp"
//using namespace rapidxml;

#include "../xml-message-passing/tinyxml++/ticpp.h"
#include <sstream>
class XML:public MyMLClass{
public:
    XML(MyML &myml):MyMLClass(myml){}
    bool IsNumber(string str){

        std::istringstream i(str);
           double x;
           if (!(i >> x))
             return false;
           return true;
    }

    //! Memory leak?
    string Out(){
        stringstream ss;
        //! da es xml ist, können wir hier auch alle Elemente einfach rausschreiben? hat xml ein root element?
        ss<<(Out("MyML",ref));
        return ss.str();
    }

    ticpp::Element Out(string name,MyML& myml){


        if(IsNumber(name)){
            cout<<name<<" is a number!"<<endl;
            name="Item_"+name;
        }
        ticpp::Element element(name);

        for(auto a:myml.Attributes()){
            element.SetAttribute(a.first,a.second);
        }

        for(auto e:myml.Elements()){
            //element->LinkEndChild(Out(e.first,e.second));
            ticpp::Element child=Out(e.first,e.second);
            element.InsertEndChild(child);

        }


        return element;
    }


    void In(string xml){

        ticpp::Document doc("msg");
        doc.Parse(xml.c_str(),0,TIXML_ENCODING_UTF8);
        ticpp::Element* pElem=doc.FirstChildElement(); //root
        ticpp::Element* child=pElem;
         for( child = doc.FirstChildElement( false ); child; child = child->NextSiblingElement( false ) ){
            string name=child->Value();
            ref.AddE(name,MyML());
            In(child,ref.E(name));
        }
        //In(pElem,ref);

    }
    void In(ticpp::Element* e,MyML& myml){

        ticpp::Iterator< ticpp::Attribute > attribute;
        for ( attribute = attribute.begin( e ); attribute != attribute.end(); attribute++ ){
            //string a_name=attribute->Name()
            myml.AddA(attribute->Name(),attribute->Value());
        }

        ticpp::Iterator<ticpp::Element> child;

        //-----check the message:
        for (child=child.begin(e);child!=child.end();child++){
            string name=child->Value();
            myml.AddE(name,MyML());
            In(&(*child),myml.E(name));
        }

    }

};



#endif // MYML_H
