#include "helper.h"

Helper::Helper()
{
}


vector<string> splitstring(string str, char split){

    vector<string> ret;
    string tmp="";
    for(int i=0;i< str.length();i++){
        if(str[i]!=split)
            tmp+=str[i];
        else{
            if(tmp!=""){
                ret.push_back(tmp);
                tmp="";
            }
        }
    }
    if(tmp!="")
        ret.push_back(tmp);
    return ret;

}
string Remove(string str, int pos){
    vector<string> str_array=splitstring(str);
    string new_str="";
    for(int i=0;i<str_array.size();i++){
        if(i!=pos){
            if(new_str!="")
                new_str+=".";
            new_str+=str_array[i];
        }
    }
    return new_str;
}

string RemoveFirst(string str){
    return Remove(str,0);
}
string RemoveLast(string str){
    return Remove(str,splitstring(str).size()-1);
}
string Last(string str){
    vector<string> split=splitstring(str);
    return split[split.size()-1];
}
string First(string str){
    vector<string> split=splitstring(str);
    return split[0];
}



int Distance(MyMLPos p1, MyMLPos p2)
{
    int xdiff=ABS(p1.X()-p2.X());
    int ydiff=ABS(p1.Y()-p2.Y());

    return xdiff+ydiff;

}



MyML GotoPos(MyMLPos start, MyMLPos end, int distance)
{
    const int x=(end.X()-start.X());
    const int y=(end.Y()-start.Y());


    MyML positions("size=0");


    if(ABS(x)>ABS(y)){

        for(int i=SIGN(x);i!=(x+SIGN(x));i+=SIGN(x)){


            int pos_x=start.X()+i;//(i*SIGN(x));

            float r=(float)i/(float)x;
            int i_y=(int)((float)y*r);
            int pos_y=start.Y()+i_y;

            MyML pos;
            pos.AddA("x",STR(pos_x));
            pos.AddA("y",STR(pos_y));
            MyMLArray(positions).Add(pos);

        }
    }else{
        for(int i=SIGN(y);i!=(y+SIGN(y));i+=SIGN(y)){


            int pos_y=start.Y()+i;//(i*SIGN(x));

            float r=(float)i/(float)y;
            int i_x=(int)((float)x*r);
            int pos_x=start.X()+i_x;

            MyML pos;
            pos.AddA("x",STR(pos_x));
            pos.AddA("y",STR(pos_y));
            MyMLArray(positions).Add(pos);

        }

    }



    return positions;
}
