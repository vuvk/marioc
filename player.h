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


/* update player state */
void PlayerUpdate (SCreature* player);
/* apply frames for events*/
void PlayerUpdateFrames (SCreature* player);
