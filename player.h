#pragma once

#include "creature.h"

SCreature* player;
bool playerCanDamaged;
int16 playerPrevHealth;
bool moveL;
bool moveR;

/*typedef struct
{
    SCreature* creature;
    bool moveL;
    bool moveR;
} SPlayer;*/


/* update player state */
void PlayerUpdate ();
/* apply frames for events*/
void PlayerUpdateFrames ();
