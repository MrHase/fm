#include "team.h"

Team::Team()
{
}

void Team::AddPlayer(Player *p)
{
    player.push_back(p);
}
