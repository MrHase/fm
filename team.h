#ifndef TEAM_H
#define TEAM_H


#include "player.h"

#include <list>

using namespace std;

class Team
{
public:

    Team();
    void AddPlayer(Player* p);
    list<Player*> GetPlayer(){return player;}
private:

    list<Player*> player;

};

#endif // TEAM_H
