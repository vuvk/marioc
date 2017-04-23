#include "player.h"


void PlayerUpdateFrames (SCreature* player)
{
    if (player == NULL)
        return;

    /* �������? */
    if (!player->isGrounded)
    {
        CreatureSetFrameRange (player, 5, 5);
    }
    else
    {
        /* ��������? */
        if (abs (player->impulse.x) > EPSILON)
        {
            /* �������� ���������� */
            if ((moveL && (player->impulse.x > 0.0f)) ||
                (moveR && (player->impulse.x < 0.0f)))
            {
                CreatureSetFrameRange (player, 4, 4);
            }
            else
            {
                /* ������ */
                CreatureSetFrameRange (player, 1, 3);
            }
        }
        else
        {
            CreatureSetFrameRange (player, 0, 0);
        }
    }
}


void PlayerGetDamage (SCreature* player, int damage)
{
    if (player == NULL)
        return;

    printf ("I kick Player!\n");
}
