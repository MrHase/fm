#include "script.h"

#include <fstream>
#include <algorithm>
#include "helper.h"
#include "parser.h"

static int tmp_ref_counter=0;
string GetTmpName(){
    string name="TMPNAME";
    name+=IntToString(tmp_ref_counter);
    tmp_ref_counter++;
    return name;
}

Script::Script()
{
}

Script::~Script()
{
    for(auto e:new_elements){
        delete e;
    }
}

#include <sstream>
/*
vector<string> GetCodeLines(map<string, Ref>& references,string str){
    //pos=str.find("\n{");
    //str=str.replace(pos,2,"{");
    //
    vector<string> lines;
    vector<vector<string> > codestack;
    codestack.push_back(vector<string>());

    vector<string> line;
    line.push_back(string(""));
    int layer=0;


    for(int i=0;i<str.size();i++){
        string c=str.substr(i,1);//ing(&str[i]);
        if(c=="\n"){
            string ready=line[layer];
            if( ready!="" && ready!="\n" && ready!=" "){
                lines.push_back(line[layer]);
                codestack[layer].push_back(line[layer]);

                line[layer]="";
            }
        }else{

            if(c=="{"){
                codestack.push_back(vector<string>());
                line.push_back(string(""));
                layer++;
            }else if(c=="}"){
                //cout<<"test: "<<codestack[layer][0]<<endl;
                Ref r;
                r.type=FN;
                r.code=codestack[layer];
                string tmpname=GetTmpName();
                references[tmpname]=r;
                codestack.pop_back();
                line.pop_back();
                layer--;

                line[layer]+=tmpname;
                //cout<<"gogogo:<"<< references["TMPNAME0"].code[0]<<endl;

            }else{
                if(c!="\t"){
                    line[layer]+=str[i];
                }
            }
        }
    }

    return codestack[0];
    //return lines;
}
*/

vector<string> GetCodeLines(map<string, Ref>& references,string str){
    //pos=str.find("\n{");
    //str=str.replace(pos,2,"{");
    //
    std::replace(str.begin(),str.end(),'\t',' ');
    vector<string> lines;
    string current_line;
    string current_code;
    int layer=0;

    for(int i=0;i<str.size();i++){
        string c=str.substr(i,1);
        if(c=="\n"){
            if(layer==0){
                if(current_line!="" && current_line!="\n" && current_line!=" "){
                    lines.push_back(current_line);
                }
                current_line="";
            }else{
                current_code+=c;
            }

        }else if(c=="{"){
            if(layer==0){

            }else{
                current_code+=c;
            }
            layer++;

        }else if(c=="}"){
            layer--;
            if(layer==0){
                Ref r;
                r.type=FN;
                r.code=current_code;
                //r.code=GetCodeLines(references,current_code);
                string tmpname=GetTmpName();
                references[tmpname]=r;
                cout<<"Code: "<<current_code<<endl;
                //! hier ncoh vorher ein leerzeichen?
                current_line+=tmpname;
                current_code="";
            }else{
                current_code+=c;
            }
        }else{
            if(layer==0){
                current_line+=c;
            }else{
                current_code+=c;
            }
        }
    }
    if(current_line!=""){
        lines.push_back(current_line);
    }
    if(current_code!=""){
        cout<<"Das ist merkwürdig! fehlt ne klammer?"<<endl;
    }
    return lines;
}



MyML Script::Run(string filename, vector<MyML*> myml, string function)
{
    ifstream script(filename);
    if(!script.is_open())
        throw MyMLException("Could not open file: "+filename);


    vector<string> lines;
    string code="";
    while ( script.good() )
    {
      string line;
      getline (script,line);
      //cout << line << endl;
      if(line!=""){
          code+=line;
          code+="\n";
        lines.push_back(line);
      }
    }
    script.close();

    map<string,Ref> refs;
    //cout<<code<<endl;
    code=FindInnerStringAndReplace(code,string("["),string("]"),[](string str){
             //erst mal alle \n aus [myml] entfernen
            std::replace( str.begin(), str.end(), '\n', ';');
            std::replace( str.begin(), str.end(), '\t', ';');
            string nstr;
            nstr="[";nstr+=str;nstr+="]";
            return nstr;
    });
    code=FindInnerStringAndReplace(code,string("<"),string(">"),[](string str){
            //funktionsaufrufe auch über mehrere zeilen erlauben
            std::replace( str.begin(), str.end(), '\n', ' ');
            std::replace( str.begin(), str.end(), '\t', ' ');
            string nstr;
            nstr="<";nstr+=str;nstr+=">";
            return nstr;
    });

    //cout<< "CODE: "<<code<<endl;

    lines=GetCodeLines(refs,code);

    string current_fn="";

    for(int i=0;i<lines.size();i++){
        cout<<"line: "<<lines[i]<<endl;
        vector<string> split=splitstring(lines[i],' ');
        string cmd=split[0];
        if(cmd=="def"){
            Ref r;
            r.type=FN;
            const vector<string> c=split;
            for(int j=2;j<c.size()-1;j++){
                r.para.push_back(c[j]);
            }
            r.code=refs[c[c.size()-1]].code;
            refs[c[1]]=r;
        }else {
            cout<<"Dont know what to do!!!"<<endl; //!
        }

    }
    //cout<<"ok, bis hierhin"<<endl;
    //cout<<"size: "<<refs["_if"].code.size()<<endl;
    //! das muss in die Call funktion
    map<string,Ref> myrefs=refs;
    int counter=0;
    for(MyML* m: myml){
        Ref r;
        r.type=ELEMENT;
        r.myml=m;
//        cout<<"Para: "<<refs[function].para[0]<<endl;
        myrefs[refs[function].para[counter]]=r;
        counter++;

    }

    MyML ret;
    Execute(refs[function].code,myrefs);
    if(myrefs["RETURN"].type!=ELEMENT)
    {
        //cout<<"no valid return"<<endl;
    }else{
        if(myrefs["RETURN"].myml!=NULL){
            ret=*myrefs["RETURN"].myml;
        }
    }


    return ret;

}

string CppString(string str){
    if(str=="\"\""){
        return "";
    }
    if(str.find("\"")!=string::npos){
        string n_str=str.substr(1,str.size()-2);
        return n_str;
    }
    return str;
}
bool isRef(string str,map<string,Ref>& refs){
    //cout<<"is ref: "<<str<<endl;
    return (refs.find(str)!=refs.end());
}
bool isString(string str){
    //cout<<str<<": "<<str.find("\"")<<"...."<<string::npos<<endl;
    return (str.find("\"")!=string::npos);
}


string GetRef(string str){
    string n_str=splitstring(str,'|')[0];

    return n_str;
}

string GetPath(string str){
    if (str.find("|")==string::npos)
        return "";
    string n_str=splitstring(str,'|')[1];
    return n_str;
}

string GetAttribute( map<string, Ref>& references,string str){
        if(references[GetRef(str)].type==STRING){
            return references[GetRef(str)].str;
        }else{
            return references[GetRef(str)].myml->A(GetPath(str));
        }

}



RefType GetType(map<string,Ref>& refs,string str){
    RefType t;
    string ref=GetRef(str);
    if(!isRef(ref,refs)){
        cout<<"Ref does not exist: "<<ref<<endl;
        exit(1);
    }
    t=refs[ref].type;
    if(t==ELEMENT && str.find("|")!=string::npos){
        try{
            GetAttribute(refs,str);
            t=ATTRIBUTE;

        }catch(MyMLException& me){
            //cout<<"not an attribute..."<<endl;
        }

    }
    return t;

}

string GenerateString(map<string, Ref>& references,string str){
    string nstr="";
    bool is_string=false;
    string tmp_str="";

    for(int i=0;i<str.size();i++){
        string c=str.substr(i,1);//ing(&str[i]);
        if(c=="\""){
            if(is_string){
                string name=GetTmpName();
                Ref r;
                r.type=STRING;
                r.str=tmp_str;
                references[name]=r;
                tmp_str="";
                nstr+= name;
            }
            is_string=!is_string;
        }else{
            if(is_string){
                tmp_str+=c;
            }else{
                nstr+=c;
            }
        }

    }
    return nstr;
}

vector<string> GenerateString(map<string, Ref>& references,vector<string> strs){

    vector<string> n_strs;
    for(string s:strs){
        if(isString(s)){
            Ref r;
            r.type=STRING;
            r.str=CppString(s);
            string tmpname=GetTmpName();
            references[tmpname]=r;
            n_strs.push_back(tmpname);

        }else{
            n_strs.push_back(s);
        }
    }

    return n_strs;
}

string Script::ExecuteFunctions(map<string, Ref>& references,string str){
    const string bopen="<";
    const string bclose=">";

    string nstr="";
    int level=0;
    vector<string> tmpstr;
    tmpstr.push_back(string());

    if(str.find(bopen)!=string::npos){

        for(int i=0;i<str.size();i++){
            string c=str.substr(i,1);

            if(c==bopen){
                tmpstr.push_back(string());
                level++;
            }else if(c==bclose){
                string fullstr=tmpstr.back();
                vector<string> fullstrs=splitstring(fullstr,' ');
                string fn=fullstrs[0];
                vector<string> para;
                for(int i=1;i<fullstrs.size();i++){
                    para.push_back(fullstrs[i]);
                }
                //cout<<"CALL: "<<references[fn].code<<endl;
                string ret=Call(fn,references,para);
                tmpstr[level-1]+=ret;

                tmpstr.pop_back();
                level--;
            }else{

                tmpstr[level]+=c;

            }
        }
    }else{
        tmpstr[0]=str;
    }
    nstr=tmpstr[0];
    //cout<<"BLA: "<<nstr<<endl;
    return nstr;
}

string Script::GenerateMyML(map<string, Ref>& references, string str){

    string nstr;
    nstr=FindInnerStringAndReplace(str,"[","]",[&](string str){
            MyML* new_e=new MyML();
            (*new_e)=parse(str);
            new_elements.push_back(new_e);
            Ref new_ref;
            new_ref.type=ELEMENT;
            new_ref.myml=new_e;
            string tmpname=GetTmpName();
            references[tmpname]=new_ref;
            return tmpname;
    });
    //cout<<"NEWSTR: "<<nstr<<endl;
    return nstr;

}


void Script::Execute(string code, map<string, Ref> &references)
{
    Execute(GetCodeLines(references,code),references);
}

void Script::Execute(vector<string> code, map<string, Ref>& references)
{

    for(string s:code){

        s=ExecuteFunctions(references,s);
        s=GenerateString(references,s);
        s=GenerateMyML(references,s);

        //s=ExecuteFunctions(references,s);
        vector<string> c=splitstring(Clean(s),' ');
        //c=GenerateString(references,c); //! das ist neu

        string cmd=c[0];
        //cout<<"CMD: "<<cmd<<endl;
        //cout<<"LineOfCode: "<<s<<endl;
        if(cmd=="info"){
            if(isRef(c[1],references)){
                references[c[1]].CmdInfo();
            }
        }else if(cmd=="AddE"){
            references[GetRef(c[1])].CmdAddE(GetPath(c[1]));
        }else if(cmd=="AddA"){
            references[GetRef(c[1])].CmdAddA(GetPath(c[1]),GetAttribute(references,c[2]));
        }else if(cmd=="Ref"){
            references[c[1]]=references[GetRef(c[2])].GetRef(GetPath(c[2]));

        }else if(cmd=="New"){
            string para="";
            if(c.size()>2){
                para=references[c[2]].str; //! kann das auch funktion sein?
            }
            MyML* new_e=new MyML(para);
            new_elements.push_back(new_e);
            Ref new_ref;
            new_ref.type=ELEMENT;
            new_ref.myml=new_e;

            references[c[1]]=new_ref;
        }else if(cmd=="Copy"){
            references[c[1]]=references[GetRef(c[2])].Copy(GetPath(c[2]));

        }else if(cmd=="return"){
            references["RETURN"]=references[c[1]];
            break;
        }else if(cmd=="print"){
            cout<<"Script: ";
            for(int i=1;i<c.size();i++){
                cout<<" ";
                cout<<references[GetRef(c[i])].ToString(GetPath(c[i]));

            }
            cout<<endl;



        }else if(cmd=="if"){
            string fn_false="";
            if(c.size()==6){
                fn_false=c[5];
            }
            const string second=c[3];
            CmdCmp(references,c[1],c[2],second,c[4],fn_false);
        }else{
            //cout<<"unknow cmd: "<<cmd<<endl;
            //! einzelne dinger werden im moment noch nicht beachtet sowas wie z.B. "ichbineinecodezeileohnesinn"
            if(c.size()>1){

                if( ! isRef(GetRef(cmd),references)){
                    Ref ref=references[GetRef(c[1])];
                    RefType rt=GetType(references,c[1]);
                    Ref new_ref;
                    new_ref.type=rt;

                    //new_ref.myml=new_e;
                    references[cmd]=new_ref;
                }
                RefType rt=references[GetRef(cmd)].type;
                Ref& ref=references[GetRef(cmd)];
                Ref& newref=references[GetRef(c[1])];
                if(ref.type!=newref.type){
                    cout<<"Hier läuft was gewaltig schief!!!!"<<endl; //!
                }
                if(rt==STRING){
                    ref.str=newref.str;
                }else if(rt==ELEMENT){
                    ref.myml=newref.myml;
                    //cout<<"bla..."<<ref.myml->Info()<<endl;
                }else if (rt==ATTRIBUTE){
                    cout<<"NOT IMPLEMENTED"<<endl;
                }else if (rt==FN){
                    ref =newref;
                }
            }
        }
        //cout<<"Code: "<<c[0]<<endl;
    }


    //Ref ret;

}


string Script::Clean(string str)
{
    string n_str;
    for(int i=0;i<str.size();i++){
        if(str[i]!='\t'){ //! '' ist scheiße wegen utf8?
            if(str[i]=='='){
//                n_str+=" ";
                n_str+=str[i];
//                n_str+=" ";
            }else if(str[i]=='{' || str[i]=='}'){
//                n_str+='\n';
//                n_str+=str[i];
//                n_str+='\n';
            }else{
                n_str+=str[i];
            }

        }
    }
    return n_str;
}

string Script::Call(string fn, map<string, Ref> &references,vector<string> para)
{

    map<string,Ref> myrefs;
    //Funktionen kopieren
    for(pair<string,Ref> r:references){
        if(r.second.type==FN){
            myrefs[r.first]=r.second;

        }
    }

    // parameter kopieren
    int counter=0;
    for(string s:para){

        myrefs[references[fn].para[counter]]=references[s];
        counter++;
    }
    //cout<<"CALL: "<<references[fn].code<<endl;
    Execute(references[fn].code, myrefs);
    string retname=GetTmpName();
    references[retname]=myrefs["RETURN"];
    return retname;


}


void Ref::CmdInfo()
{
    if(type==ELEMENT){
        cout<<myml->Info()<<endl;
    }
}

void Ref::CmdAddE(string e)
{
    if(type==ELEMENT){
        myml->AddE(e);
    }

}


void Ref::CmdAddA(string a,string v)
{
    if(type==ELEMENT){
        //cout<<"AddA: "<<a<<" with v: "<<v<<endl;
        myml->AddA(a,v);

    }
}


Ref Ref::GetRef(string path)
{
    Ref r;
    r.type=type;
    r.str=str;
    r.code=code;
    r.para=para;
    //! wie siehts mit attribute aus

    if(type==ELEMENT){
        r.myml=&myml->E(path);

    }
    return r;
}

Ref Ref::Copy(string path)
{
    Ref r;
    r.type=type;
    r.str=str;
    r.code=code;
    r.para=para;

    if(type==ELEMENT){
        MyML* tmp=new MyML();

        *tmp=(myml->E(path));
        r.myml=tmp;

    }

    return r;
}

string Ref::ToString(string path)
{
    if(type==STRING){
        return str;
    }else if(type==ELEMENT){
        try{
            return myml->A(path);
        }catch(MyMLException& me){
            //
        }

        return myml->E(path).Info();

    }else if(type==ATTRIBUTE){
        return myml->A(path);
    }
}

void TypeError(RefType t1, RefType t2){
    string t1str="", t2str="";
    if(t1==STRING)t1str="STRING";
    if(t1==ELEMENT)t1str="ELEMENT";
    if(t1==ATTRIBUTE)t1str="ATTRIBUTE";
    if(t1==FN)t1str="FN";

    if(t2==STRING)t2str="STRING";
    if(t2==ELEMENT)t2str="ELEMENT";
    if(t2==ATTRIBUTE)t2str="ATTRIBUTE";
    if(t2==FN)t2str="FN";
    cout<<"TypeError: t1 -> "<<t1str<<" t2 -> "<<t2str<<endl;

}

void CmdCmp(map<string, Ref> &refs, string first, string cmp, string second, string fn_true, string fn_false)
{

    RefType t1=GetType(refs,first);
    Ref r1=refs[GetRef(first)];
    RefType t2=GetType(refs,second);
    Ref r2=refs[GetRef(second)];

    bool result=false;

    if(false){

    }else if(t1 == STRING && t2 == STRING){
        result=(r1.str==r2.str);
    }else if(t1 == STRING && t2 == ATTRIBUTE){
        result=(r1.str==GetAttribute(refs,second));
    }else if(t1 == STRING && t2 == ELEMENT){
        TypeError(t1,t2);
    }else if(t1 == STRING && t2 == FN){
        TypeError(t1,t2);
    }else if(t1 == ATTRIBUTE && t2 == STRING){
        result=(GetAttribute(refs,first) == r2.str);

    }else if(t1 == ATTRIBUTE && t2 == ATTRIBUTE){
        result=(GetAttribute(refs,first) == GetAttribute(refs,second));
    }else if(t1 == ATTRIBUTE && t2 == ELEMENT){
        TypeError(t1,t2);
    }else if(t1 == ATTRIBUTE && t2 == FN){
        TypeError(t1,t2);
    }else if(t1 == ELEMENT && t2 == STRING){
        cout<<" TEST "<<first<<" "<<second<<endl;
        TypeError(t1,t2);
    }else if(t1 == ELEMENT && t2 == ATTRIBUTE){
        TypeError(t1,t2);
    }else if(t1 == ELEMENT && t2 == ELEMENT){
        result=r1.myml->E(GetPath(first))==r2.myml->E(GetPath(second));
    }else if(t1 == ELEMENT && t2 == FN){
        TypeError(t1,t2);
    }else if(t1 == FN && t2 == STRING){
        TypeError(t1,t2);
    }else if(t1 == FN && t2 == ATTRIBUTE){
        TypeError(t1,t2);
    }else if(t1 == FN && t2 == ELEMENT){
        TypeError(t1,t2);
    }else if(t1 == FN && t2 == FN){
        TypeError(t1,t2);
    }

    if(cmp=="!="){
        result= !result;
    }

    Script script;

    if(result){
        script.Execute(refs[fn_true].code,refs);
    }else{
        if(fn_false!=""){
            script.Execute(refs[fn_false].code,refs);
            //cout<<"FALSE"<<endl;
        }
    }

}
