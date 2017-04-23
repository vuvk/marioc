#pragma once

#include "creature.h"

SCreature* player;
bool moveL;
bool moveR;

/*typedef struct
{
    SCreature* creature;
    bool moveL;
    bool moveR;
} SPlayer;*/

/* apply frames for events*/
void PlayerUpdateFrames (SCreature* player);

void PlayerGetDamage (SCreature* player, int damage);
