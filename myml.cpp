#include "myml.h"
#include <sstream>
#include <iostream>

#include "helper.h"

using namespace std;
#include <vector>

MyML::MyML()
{
}

MyML::MyML(string str)
{
    Do(str);
}

MyML::MyML(string str, map<string, MyML> references, map<string, string> strings)
{
    Do(str,references,strings);
}

MyML::MyML(string str, vector<MyML> references, vector<string> strings)
{
    Do(str,references,strings);
}

void MyML::Do(string str)
{
    Do(str,map<string, MyML>(),map<string, string>());
}

void MyML::Do(string str, vector<MyML> references, vector<string> strings)
{
    map<string,MyML> ref_map;
    for(int i=0;i<references.size();i++){
        ref_map[STR(i)]=references[i];
    }
    map<string,string> string_map;
    for(int i=0;i<strings.size();i++){
        string_map[STR(i)]=strings[i];
    }
    Do(str,ref_map,string_map);
}

void MyML::Do(string str,map<string,MyML> references,map<string,string> strings)
{

    MyML mymlstrings;
    for(pair<string,string> s:strings){
        //cout<<s.first<<endl;
        mymlstrings.AddA(s.first,s.second);
    }
    references["%s"]=mymlstrings;


    vector<string> split=splitstring(str,';');
    for(string s: split){

        vector<string> split_1=splitstring(s,'=');
        vector<string> split_left=splitstring(split_1[0],'.');

        MyML *last=this;
        for(int i=0;i<split_left.size()-1;i++){
            if(!last->ExistsE(split_left[i]))
                last->AddE(split_left[i]);
            last=& last->E(split_left[i]);
        }
        if(split_1.size()<=1){
            //nur elements hinzufügen
            last->AddE(split_left[split_left.size()-1]);
        }else{
            string right=split_1[1];
            string c=right.substr(0,1);//ing(&str[i]);

            if(c=="@"){
                //cerr<<"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC    "<<c<<endl;
                right=right.substr(1,right.size()-1);
                vector<string> split_source=splitstring(right,'|');

                if(split_source.size()>1){
                    string p=split_source[0];
                    string l=split_source[1];
                    if(references.find(p)==references.end()){
                        cout<<"Reference "<<p<<" does not exist!"<<endl;
                    }
                    if(references[p].ExistsA(l)){
                        last->AddA(split_left[split_left.size()-1],references[p].A(l));
                    }else if(references[p].ExistsE(l)){
                        last->AddE(split_left[split_left.size()-1],references[p].E(l));
                    }else{
                        cout<<"Element/Attribute "<<l<<" does not exist in "<<p<<endl;
                    }
                }else{
                    //hier commands:
                    /*
                    if(split_source[0].substr(0,2)=="%s"){
                        cout<<"YYYaaaayy"<<endl;
                    }*/

                }
            }else{
                last->AddA(split_left[split_left.size()-1],right);
            }

            //last->AddA(split_left[split_left.size()-1],split_1[1]);
        }

    }


}
bool operator ==(MyML& first,MyML& second){

//    throw MyMLException("Not implemented");
//    return true;
    return first.Equals(second);
}

MyML &MyML::E(string k)
{
    if(k==""){
        return *this;
    }
    vector<string> split=splitstring(k);
    if(split.size()==1){
    //elements.find(k);
        std::multimap<string,MyML>::iterator it = elements.find(k);
        if(it==elements.end()){
            stringstream ss;
            ss<<"Element "<<k<<" does not exist"<<endl;
            throw MyMLException(ss.str());

        }
        return it->second;
    }else{

        MyML *last=this;
        for(int i=0;i<split.size()-1;i++){
            last=& last->E(split[i]);
        }
        return last->E(split.back());

    }
}

void MyML::E(string k, MyML v)
{
    SetE(k,v);

}

void MyML::SetE(string k, MyML v)
{

    //cout<<"set "<<k<<" to "<<v.Info()<<endl;
    //std::multimap<string,MyML>::iterator it = elements.find(k);
    vector<string> split=splitstring(k);
    if(split.size()==1){

        elements[split[0]]=v;
    }else{

        MyML *last=this;
        for(int i=0;i<split.size()-1;i++){
            if(!last->ExistsE(split[i]))
                last->AddE(split[i]);
            last=& last->E(split[i]);
        }
        last->SetE(split.back(),v);

    }

}

void MyML::SingleE(string k)
{
    MyML v;
    SingleE(k,v);
}

void MyML::SingleE(string k, MyML v)
{
    if(ExistsE(k)){
        elements[k]=v;
    }else{

        AddE(k,v);
    }

}

void MyML::DelE(string k)
{
    vector<string> split=splitstring(k);
    if(split.size()==1){
        elements.erase(k);
    }else{
        //cout<<RemoveLast(k)<<endl;
        E(RemoveLast(k)).DelE(Last(k));
    }
}

bool MyML::Empty()
{
    if(Elements().size()==0 && Attributes().size()==0){
        return true;
    }else{
        return false;
    }
}


void MyML::AddArray(string k,int size)
{
    MyML a("type=array;size=0");
    elements[k]=a;
    for(int i=0;i<size;i++){
        AddToArray(k,MyML());
    }
}

void MyML::AddToArray(string k, MyML v)
{
    //cout<<"AddToArray "<<k<<v.Info()<<endl;
    int index=StringToInt(E(k).A("size"));
    E(k).AddE(IntToString(index),v);
    E(k).A("size",IntToString(++index));
    //cout<<"Info nach dem adden: "<<index<<endl<<E(k).Info()<<endl;
}

vector<MyML> MyML::Array(string k)
{
    //cout<<"ARRAY"<<endl;
    vector<MyML> array;
    if(E(k).A("type")!="array"){
        throw MyMLException("This is not an array!");
    }
    int size=StringToInt(E(k).A("size"));
    //cout<<"Size: "<<size<<endl;
    for(int i=0;i<size;i++){
        array.push_back(E(k).E(IntToString(i)));
    }
    return array;
}



MyML &MyML::AddE(string k, MyML v)
{
    //elements.insert(pair<string,MyML>(k,v));
    vector<string> split=splitstring(k);
    if(split.size()==1){
        elements.insert(pair<string,MyML>(k,v));
    }else{
        //cout<<RemoveLast(k)<<endl;
        E(RemoveLast(k)).AddE(split[split.size()-1],v);
    }
    return (*this);
}

MyML &MyML::AddE(string k)
{
    MyML tmp;
    AddE(k,tmp);
}

bool MyML::ExistsE(string k)
{
    try{
        CheckE(k);
        return true;
    }catch(MyMLException& e){
        return false;
    }
}

void MyML::CheckE(string k)
{
    vector<string> split=splitstring(k);
    if(split.size()==1){
        std::multimap<string,MyML>::iterator it = elements.find(k);
        if(it==elements.end()){
            stringstream ss;
            ss<<"Element "<<k<<" does not exist"<<endl;
            throw MyMLException(ss.str());
        }
        return;
    }else{
        string new_k=RemoveFirst(k);
        //cout<<"new_k: "<<new_k<<endl;
        this->E(split[0]).CheckE(new_k);
    }
}

void MyML::CpE(MyML other, string k)
{
    AddE(k,other.E(k));
}

void MyML::CpE(MyML other, list<string> keys)
{
    for(auto str:keys){
        CpE(other,str);
    }
}


string MyML::A(string k)
{
    CheckA(k);
    vector<string> split=splitstring(k);
    if(split.size()==1){
        return attributes[split[0]];
    }else{
//        MyML *last=this;
//        for(int i=0;i<split.size()-1;i++){
//            last=& last->E(split[i]);
//        }
//        return last->A(split.back());
        return GetElement(RemoveLast(k))->A(split[split.size()-1]);
    }
}

void MyML::A(string k, string v)
{
    CheckA(k);
    *GetAttribute(k)=v;
    //attributes[k]=v;
}

MyML* MyML::AddA(string k, string v)
{
    vector<string> split=splitstring(k);
    if(split.size()==1){
        attributes[split[0]]=v;
    }else{
        //cout<<RemoveLast(k)<<endl;
        E(RemoveLast(k)).AddA(Last(k),v);
    }
    return (this);
}

void MyML::DelA(string k)
{
    if(ExistsA(k)){
        vector<string> split=splitstring(k);
        if(split.size()==1){
            attributes.erase(k);
        }else{
            E(RemoveLast(k)).DelA(Last(k));
        }
    }
}

bool MyML::ExistsA(string k)
{
    try{
        CheckA(k);
        return true;
    }catch(MyMLException& e){
        return false;
    }
}

void MyML::CheckA(string k)
{
    //cout<<"CheckA for "<<k<<endl;
    if(k==""){
        throw MyMLException("Attribute name is empty");
    }
    vector<string> split=splitstring(k);
    if(split.size()==1){
        std::map<string,string>::iterator it = attributes.find(k);
        if(it==attributes.end()){
            stringstream ss;
            ss<<"Attribute "<<k<<" does not exist"<<endl;
            throw MyMLException(ss.str());
        }
        //cout<<this->Info()<<endl;
        return;
    }else{
        string new_k=RemoveFirst(k);
        //cout<<"new_k: "<<new_k<<endl;
        this->E(split[0]).CheckA(new_k);

    }
}

bool MyML::Equals(MyML &o)
{
    for(auto& a: Attributes()){
        if(!o.ExistsA(a.first)){
            return false;
        }
        if(a.second!=o.A(a.first)){
            return false;
        }

    }
    for(auto& e:Elements()){
        if(!o.ExistsE(e.first)){
            return false;
        }
        if(!E(e.first).Equals(o.E(e.first))){
            return false;
        }
    }
    return true;
}

void MyML::CpA(MyML other, string k)
{
    AddA(k,other.A(k));
}

void MyML::CpA(MyML other, list<string> keys)
{
    for(string str: keys){
        CpA(other,str);
    }
}

string MyML::Info(string offset)
{

    stringstream str;
    for(pair<string,string> a:attributes){
        str<<" "<<offset<<a.first<<" = "<<a.second<<endl;
    }
    offset+=" ";
    for(auto e:elements){
        str<<offset<<e.first<<"{"<<endl;
        str<<e.second.Info(offset);
        str<<offset<<"}"<<endl;
    }
    return str.str();
}

bool MyML::Match(MyML myml)
{
    for(auto a:myml.Attributes()){
        if(!ExistsA(a.first))
            return false;
        if(A(a.first)!=a.second)
            return false;
    }

    for(auto e:myml.Elements()){
        if(!ExistsE(e.first))
            return false;
        if(!E(e.first).Match(e.second))
            return false;
    }

    return true;
}

MyML& MyML::FindMatch(MyML myml)
{
    //hmmm ne irgendwie uncool...was machen wir hier wenn das ding nicht existiert?
}

bool MyML::ExistsMatch(MyML myml)
{
    for(auto& e:elements){
        if(e.second.Match(myml)){
            return true;
        }
    }
    return false;
}

Iter<MyML> MyML::begin()
{
    return Iter<MyML>( *this, 0 );
}

Iter<MyML> MyML::end()
{
    if(!ExistsA("type"))
        throw MyMLException("No type attribute existing");

    if(A("type")!="array")
        throw MyMLException("type should be array but is: XXX");

    if(!ExistsA("size"))
        throw MyMLException("No size attribute existing");

    return Iter<MyML>(*this,INT(A("size")));

}

string *MyML::GetAttribute(string k)
{
    CheckA(k);
    vector<string> split=splitstring(k);
    if(split.size()==1){
        return &this->attributes[k];
    }else{
        string new_k=RemoveFirst(k);
        return this->E(split[0]).GetAttribute(new_k);
    }
}

MyML *MyML::GetElement(string k)
{
    CheckE(k);
    vector<string> split=splitstring(k);
    if(split.size()==1){
        return &this->elements[k];
    }else{
        string new_k=RemoveFirst(k);
        return this->E(split[0]).GetElement(new_k);
    }
}

/*
MyML& Iter::operator *()
{
    return _p_vec.E(STR(_pos));
}
*/


MyML &MyML::ArrayAdd(int index, MyML v)
{
    if(A("type")!="array")
        throw MyMLException("type should be array but is: XXX");
    if(!ExistsA("size"))
        throw MyMLException("No size attribute existing");

    throw MyMLException("Not implemented and never will be!");
}


MyMLArray::MyMLArray(MyML &myml):array(myml)
{
    //hier checken ob das überhaupt nen gültiges array ist
    /*
    if(!array.ExistsA("type"))
        throw MyMLException("No type attribute existing");
    if(array.A("type")!="array")
        throw MyMLException("type should be array but is: XXX");
    */
    if(!array.ExistsA("size"))
        throw MyMLException("No size attribute existing");
    //cout<<"ARRAYSIZE: "<<array.A("size")<<endl;
}

void MyMLArray::AddAtIndex(int index, MyML data)
{
    int size=INT(array.A("size"));
    if(index>=size || size==0){
        //cout<<"ADDE : i="<<index<<" "<<data.Info()<<endl;
        //liegt ausserhalb... was nu? einfach erst mal ans ende adden!
        //vllt ist es aber besser die dazwischen einfach
        Add(data);

    }else{

        for(int i=size;i>index;i--){
            array.E(STR(i),array.E(STR(i-1)));
        }
        array.E(STR(index),data);
        array.A("size",STR(++size));

    }
//    cout<<array.A("size")<<endl;
}

void MyMLArray::Add(MyML data)
{
    int index=INT(array.A("size"));
    array.AddE(STR(index),data);
    array.A("size",STR(++index));

}

//! hier wär nen template cool...
//! dann würden alle dinge gehen die man von nem string in nen objekt umwandeln kann und den == operator haben
//!
//! generell lässt sich die methode hier bestimmt schöner schreiben
void MyMLArray::Sort(string a)
{
    MyML new_array("type=array;size=0");
    MyMLArray na(new_array);

    int size=INT(array.A("size"));
    //! for_each...
    for(int i=0;i<size;i++){

        //! das hier brauchen wir!!!
        MyML& e=Get(i);
        double v1=DOUBLE(e.A(a));
        bool inserted=false;

        for(int j=0;j<na.Size()&& inserted==false;j++){
            double v2=DOUBLE(na.Get(j).A(a));
            if(v2>v1){
                na.AddAtIndex(j,e);
                inserted=true;
            }
        }
        if(!inserted){
            na.Add(e);
        }
        //array.E(STR(i),array.E(STR(i-1)));
    }

    array=new_array;
}

void MyMLArray::Invert()
{
    MyML new_array("type=array;size=0");
    MyMLArray na(new_array);

    int size=INT(array.A("size"));
    //! for_each...
    for(int i=0;i<size;i++){
        na.AddAtIndex(0,Get(i));
    }
    array=new_array;
}

MyML &MyMLArray::Get(int index)
{
    return array.E(STR(index));
}

MyML MyMLArray::ItemsMatch(MyML myml)
{
    MyML ret("type=array;size=0");
    for(auto e:array){
        if(e.Match(myml)){
            MyMLArray(ret).Add(e);
        }
    }
    return ret;
}

Iter<MyMLArray> MyMLArray::begin()
{

    return Iter<MyMLArray>( *this, 0 );

}

Iter<MyMLArray> MyMLArray::end()
{
/*
    if(!ExistsA("type"))
        throw MyMLException("No type attribute existing");

    if(A("type")!="array")
        throw MyMLException("type should be array but is: XXX");

    if(!ExistsA("size"))
        throw MyMLException("No size attribute existing");
*/
    return Iter<MyMLArray>(*this,INT(array.A("size")));


}


int MyMLArray::Size()
{
    return INT(array.A("size"));
}


MyMLCalc::MyMLCalc(MyML &myml):calc(myml)
{
}

void MyMLCalc::Add(string a, string i)
{
    double v=DOUBLE(calc.A(a));
    calc.A(a,STR(v+DOUBLE(i)));
}

void MyMLCalc::Sub(string a, string i)
{
    double v=DOUBLE(calc.A(a));
    calc.A(a,STR(v-DOUBLE(i)));
}

void MyMLCalc::Mult(string a, string i)
{
    double v=DOUBLE(calc.A(a));
    calc.A(a,STR(v*DOUBLE(i)));
}

void MyMLCalc::Div(string a, string i)
{
    double v=DOUBLE(calc.A(a));
    calc.A(a,STR(v/DOUBLE(i)));
}

bool MyMLCalc::G(string a, string i)
{
    double v=DOUBLE(calc.A(a));
    return (v>DOUBLE(i));
}

bool MyMLCalc::GE(string a, string i)
{
    double v=DOUBLE(calc.A(a));
    return (v>=DOUBLE(i));
}

bool MyMLCalc::L(string a, string i)
{

    double v=DOUBLE(calc.A(a));
    return (v<DOUBLE(i));
}

bool MyMLCalc::LE(string a, string i)
{

    double v=DOUBLE(calc.A(a));
    return (v<=DOUBLE(i));
}

double MyMLCalc::Get(string a)
{
    return DOUBLE(calc.A(a));
}

void MyMLCalc::Set(string a, string i)
{
    calc.A(a,i);
}




void MyMLMap::Add(MyML k, MyML v)
{
    MyML entry;
    entry.AddE("key",k);
    entry.AddE("value",v);
    MyMLArray(ref).Add(entry);

}

MyML& MyMLMap::Get(MyML k)
{
    MyMLArray array(ref);
    for(auto& e:array){
        if(e.E("key")==k){
            return e.E("value");
        }
    }
}

vector<MyML> MyMLMap::Keys()
{
    vector<MyML> keys;
    MyMLArray array(ref);
    for(auto e:array){
        keys.push_back(e.E("key"));
    }
    return keys;
}

bool MyMLMap::Include(MyML k)
{
    for(auto e:MyMLArray(ref)){
        if(e.E("key")==k){
            return true;
        }
    }
    return false;
}


string MyMLPrint::NoReturn()
{

    stringstream str;

    for(pair<string,string> a:Base().Attributes()){
        str<<a.first<<" = "<<a.second<<";";
    }

    for(auto e:Base().Elements()){
        str<<e.first<<"{";
        str<<MyMLPrint(e.second).NoReturn();
        str<<"};";
    }

    return str.str();
}


int MyMLPos::X()
{
    return INT(ref.A("x"));
}

MyMLPos &MyMLPos::X(int x)
{
    ref.A("x",STR(x));
    return (*this);
}

int MyMLPos::Y()
{
    return INT(ref.A("y"));
}

MyMLPos &MyMLPos::Y(int y)
{

    ref.A("y",STR(y));
    return (*this);

}
