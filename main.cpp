#include <iostream>

#include "player.h"

#include <math.h>
#include "script.h"
#include "brain.h"
#include "luascript.h"
using namespace std;

#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>

#define WIDTH 450
#define HEIGHT 640//480
#define BPP 4
#define DEPTH 32

void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    Uint32 *pixmem32;
    Uint32 colour;

    colour = SDL_MapRGB( screen->format, r, g, b );

    pixmem32 = (Uint32*) screen->pixels  + y + x;
    *pixmem32 = colour;
}

void fill_circle(SDL_Surface *surface, int cx, int cy, int radius, Uint32 pixel)
{
    // Note that there is more to altering the bitrate of this
    // method than just changing this value.  See how pixels are
    // altered at the following web page for tips:
    //   http://www.libsdl.org/intro.en/usingvideo.html

    double r = (double)radius;

    for (double dy = 1; dy <= r; dy += 1.0)
    {
        // This loop is unrolled a bit, only iterating through half of the
        // height of the circle.  The result is used to draw a scan line and
        // its mirror image below it.

        // The following formula has been simplified from our original.  We
        // are using half of the width of the circle because we are provided
        // with a center and we need left/right coordinates.

        double dx = floor(sqrt((2.0 * r * dy) - (dy * dy)));
        int x = cx - dx;

        // Grab a pointer to the left-most pixel for each half of the circle
        Uint8 *target_pixel_a = (Uint8 *)surface->pixels + ((int)(cy + r - dy)) * surface->pitch + x * BPP;
        Uint8 *target_pixel_b = (Uint8 *)surface->pixels + ((int)(cy - r + dy)) * surface->pitch + x * BPP;

        for (; x <= cx + dx; x++)
        {
            *(Uint32 *)target_pixel_a = pixel;
            *(Uint32 *)target_pixel_b = pixel;
            target_pixel_a += BPP;
            target_pixel_b += BPP;
        }
    }
}
#include "team.h"
#include "helper.h"
#include "gameengine.h"
void DrawScreen(SDL_Surface* screen,TTF_Font *font, GameEngine& ge,int i_counter)
{

    if(SDL_MUSTLOCK(screen))
    {
        if(SDL_LockSurface(screen) < 0) return;
    }
    //clear:
    SDL_FillRect(screen,NULL, 0x000000);


    float r_widht=(float)WIDTH/(float)GE_WIDTH;
    float r_height=(float)HEIGHT/(float)GE_HEIGHT;



    //TestText:
    SDL_Surface *text;
    SDL_Color text_color = {255, 255, 255};
    text = TTF_RenderText_Solid(font,"Bla",text_color);
    SDL_Rect pos;
    pos.x=10;
    pos.y=120;
    if (SDL_BlitSurface(text, NULL, screen, &pos) != 0)
    {
           cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
    }


    for( MyML bla :ge.Draw().E("T1").Array("Player")){
        float posx=INT(bla.A("Pos.x")); //.attributes["posx"]);
        float posy=INT(bla.A("Pos.y"));//attributes["posy"]);
        float oldposx=INT(bla.A("Gui.OldPos.x"));
        float oldposy=INT(bla.A("Gui.OldPos.y"));
        int c=StringToInt(bla.A("color"));


        float draw_pos_x=0,draw_pos_y=0;

        draw_pos_x=oldposx+((float)i_counter/30)*(posx-oldposx);
        draw_pos_y=oldposy+((float)i_counter/30)*(posy-oldposy);
        //cout<<"posx: "<<(int)posx<<" oldposx: "<<(int)oldposx<<" draw_x: "<<(int)draw_pos_x<<endl;


        float x=draw_pos_x;
        float y=draw_pos_y;
        x*=r_widht;
        y*=r_height;
        fill_circle(screen , (int)x,(int)y, 10,c);

        //draw id:
        SDL_Surface *text;{
            SDL_Color text_color = {255, 255, 255};
            text = TTF_RenderText_Solid(font,bla.A("id").c_str(),text_color);
        }
        SDL_Rect textpos;{
            textpos.x=(int)x;textpos.y=(int)y;
        }
        if (SDL_BlitSurface(text, NULL, screen, &textpos) != 0)
        {
               cerr << "SDL_BlitSurface() Failed: " << SDL_GetError() << endl;
        }

        //fill_circle(screen , 10,10, 5, 0xff00ff);

    }
//    for( MyML bla :ge.Draw().E("T2").E_all("Player")){
//        cout<<"HIER KOMM ICH NIE HIN"<<endl;
//        float posx=StringToInt(bla.E("Pos").A("x"));//.attributes["posx"]);
//        float posy=StringToInt(bla.E("Pos").A("y"));//.attributes["posy"]);
//        int c=StringToInt(bla.A("color"));
//        posx*=r_widht;
//        posy*=r_height;

//        fill_circle(screen , (int)posx,(int)posy, 10,c);
//        //fill_circle(screen , 10,10, 5, 0xff00ff);

//    }

    {
        try{
            float posx=INT(ge.Draw().A("Ball.Pos.x"));
            float posy=INT(ge.Draw().A("Ball.Pos.y"));
            float oldposx=INT(ge.Draw().A("Ball.Gui.OldPos.x"));
            float oldposy=INT(ge.Draw().A("Ball.Gui.OldPos.y"));
            int c=StringToInt(ge.Draw().A("Ball.color"));

            float draw_pos_x=0,draw_pos_y=0;

            draw_pos_x=oldposx+((float)i_counter/30)*(posx-oldposx);
            draw_pos_y=oldposy+((float)i_counter/30)*(posy-oldposy);
            //cout<<"posx: "<<(int)posx<<" oldposx: "<<(int)oldposx<<" draw_x: "<<(int)draw_pos_x<<endl;

//            float x=posx;
//            float y=posy;

            float x=draw_pos_x;
            float y=draw_pos_y;
            x*=r_widht;
            y*=r_height;


            //cout<< posx<<","<<posy<<endl;
            fill_circle(screen , (int)x,(int)y, 5,c);
        }catch(MyMLException& e){
            //nicht so wild...
            cout<<"Konnte den Ball nicht malen: "<<e.Error() <<endl;
        }
    }
    //fill_circle(screen , 10,10, 5, 0xff00ff);


    if(SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

    SDL_Flip(screen);
}



int main(int argc, char* argv[])
{

/*
    MyML p1("Pos;Pos.x=10;Pos.y=10");

    MyML p2("Pos;Pos.x=9;Pos.y=18");
    MyML path=GotoPos(p1,p2,0);

    for(auto p:MyMLArray(path)){
        cout<<p.Info()<<endl;
    }

    exit(1);
*/

    BrainTest();
    //exit(1);
    Script test;
    try{
        test.Run("/home/mrhase/testscript.txt",{},"scripttest");
    }catch(MyMLException& me){
        cout<<me.Error()<<endl;
    }

    /*
    MyML t("hallo=4;test;test2=3;");
    if(t.Match(MyML("hallo=4;test2=3;"))){
        cout<<"Yay"<<endl;
    }else{
        cout<<":("<<endl;
    }

    t.E("Array",MyML("type=array;size=0"));
    MyMLArray t_a(t.E("Array"));
    t_a.Add(MyML("zahl=-5"));

    t_a.Add(MyML("zahl=-5"));
    t_a.Add(MyML("zahl=-9"));
    t_a.Add(MyML("zahl=-1"));
    t_a.Add(MyML("zahl=-20"));

    for(MyML e:t.E("Array")){
        cout<<"zahl: "<<e.A("zahl")<<endl;
    }
    try{
        MyMLArray(t.E("Array")).Sort("zahl");
        MyMLArray(t.E("Array")).Invert();
    }catch(MyMLException e){
        cout<<e.Error()<<endl;
    }


    MyMLArray x(t.E("Array"));
    for(MyML e:t.E("Array").Type<MyMLArray>()){
        cout<<"zahl: "<<e.A("zahl")<<endl;
    }
    MyML t200("testvar=13;testpos.x=3;testpos.y=3");
    string zahl2="14";
    MyML t300("hallo=@0|testvar;hallo2=@%s|0;hallo3=@0|testpos",{t200},{zahl2});

    cout<<t300.Info()<<endl;
    */
    //LuaScript lua;
    //lua.Execute("/home/mrhase/dev/fm/fm/test.lua");
    //exit(1);

    //try{


        GameEngine ge;


        ge.data.E("Team1",MyML());
        ge.data.E("Team1").AddArray("Player");
        ge.data.E("Team2",MyML());
        ge.data.E("Team2").AddArray("Player");
        //18,22
        list<pair<int,int> > team1=
        {{4,3},{9,3},{14,3},
         {2,10},{7,10},{11,10},{16,10},
         {4,16},{9,18},{14,16}

                                   };

        list<pair<int,int> > team2=team1;

        team1={{4,3}};
        team2={};
        int counter=1;

        for(pair<int,int> p: team1){

            string x=IntToString(p.first);
            string y=IntToString(p.second);
            string id=IntToString(counter);
            MyML p1("InitPos;Pos;Gui.OldPos;");
            //p1.Do("Experience.size=0;Experience.type=array;");
            p1.Do("Brain;Brain.C1;Brain.C1.type=array;Brain.C1.size=0");
            p1.Do("Stats.learning=50;Stats.power=90;"); //! lerning kann raus, glaub ich
            p1.Do("Stats.curiosity=50");
            p1.AddA("InitPos.x",x);
            p1.AddA("InitPos.y",y);
            p1.AddA("Pos.x",x);
            p1.AddA("Pos.y",y);
            p1.AddA("Gui.OldPos.x",x);
            p1.AddA("Gui.OldPos.y",y);
            p1.AddA("id",id);
            p1.AddA("color",IntToString(0xff00ff));
            ge.data.E("Team1.Player").Type<MyMLArray>().Add(p1);
            counter++;
        }

        for(pair<int,int> p: team2){

            string x=IntToString(p.first);
            string y=IntToString(p.second);
            string id=IntToString(counter);
            MyML p1("InitPos;Pos;Gui.OldPos;");
            //p1.Do("Experience.size=0;Experience.type=array;");
            p1.Do("Brain;Brain.C1;Brain.C1.type=array;Brain.C1.size=0");
            p1.Do("Stats.learning=50;Stats.power=90;");
            p1.Do("Stats.curiosity=50");
            p1.AddA("InitPos.x",x);
            p1.AddA("InitPos.y",y);
            p1.AddA("Pos.x",x);
            p1.AddA("Pos.y",y);
            p1.AddA("Gui.OldPos.x",x);
            p1.AddA("Gui.OldPos.y",y);
            p1.AddA("id",id);
            p1.AddA("color",IntToString(0x00ff00));
            ge.data.E("Team2.Player").Type<MyMLArray>().Add(p1);
            counter++;
        }



        ge.Start();

        //training:
        const int training=100;
        for(int i=0;i<training;i++){
            //cout<<"TRAINING: "<<i<<" from "<<training<<endl;
            //ge.Action();
        }
        SDL_Surface *screen;
        SDL_Event event;


        int keypress = 0;
        int h=0;

        if (SDL_Init(SDL_INIT_VIDEO) < 0 ) return 1;

        //if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_FULLSCREEN|SDL_HWSURFACE)))
        if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, /*SDL_FULLSCREEN|*/SDL_HWSURFACE)))
        {
            SDL_Quit();
            return 1;
        }
        if (TTF_Init() != 0)
          {
             cerr << "TTF_Init() Failed: " << TTF_GetError() << endl;
             SDL_Quit();
             exit(1);
          }

          // Load a font
          TTF_Font *font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24);
          if(font==NULL){
            font = TTF_OpenFont("/usr/share/fonts/TTF/FreeSans.ttf", 24);
          }
          if (font == NULL)
          {
             cerr << "TTF_OpenFont() Failed: " << TTF_GetError() << endl;
             TTF_Quit();
             SDL_Quit();
             exit(1);
          }

        while(!keypress)
        {

            const int speed=30;
            //SDL_Delay(1000);
            for(int i=0;i<speed;i++){

                DrawScreen(screen,font,ge,i);
                SDL_Delay(1000/speed);
            }
            ge.Action();
            cout<<"=============================================================================================="<<endl;
            //cout<<"kick: oldpos: "<<ge.GetML().E("Ball.Gui.OldPos").Info()<<" new pos: "<<endl<<ge.GetML().E("Ball.Pos").Info()<<endl;

            while(SDL_PollEvent(&event))
            {
                 switch (event.type)
                 {
                     case SDL_QUIT:
                     keypress = 1;
                     break;
                     case SDL_KEYDOWN:
                          keypress = 1;
                         break;
                 }
            }
        }

//    }
//    catch(MyMLException& mle){
//        cout<<mle.Error()<<endl;
//    }

    TTF_Quit();
    SDL_Quit();

    return 0;
}


