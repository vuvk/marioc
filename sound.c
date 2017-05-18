#include <stdio.h>

#include "sound.h"
#include "additions.h"

Mix_Chunk* LoadSound (const char* fileName)
{
    if (!FileExists(fileName))
    {
        printf ("File '%s' doesn't exists!\n", fileName);
        return NULL;
    }

    Mix_Chunk* snd = Mix_LoadWAV (fileName);
    return snd;
}

void LoadSounds()
{
    sndCoin = LoadSound ("./media/snd/smb_coin.wav");
    sndPowerUp = LoadSound ("./media/snd/smb_powerup.wav");

    sndKick = LoadSound ("./media/snd/smb_kick.wav");
    sndJump = LoadSound ("./media/snd/smb_jump-small.wav");

    sndMarioDie = LoadSound ("./media/snd/smb_mariodie.wav");

    sndBump = LoadSound ("./media/snd/smb_bump.wav");
    sndBreakBlock = LoadSound ("./media/snd/smb_breakblock.wav");
}

void FreeSounds()
{
    Mix_FreeChunk (sndCoin);
    Mix_FreeChunk (sndPowerUp);
    Mix_FreeChunk (sndKick);
    Mix_FreeChunk (sndJump);
    Mix_FreeChunk (sndMarioDie);
    Mix_FreeChunk (sndBump);
    Mix_FreeChunk (sndBreakBlock);

    sndCoin = NULL;
    sndPowerUp = NULL;
    sndKick = NULL;
    sndJump = NULL;
    sndMarioDie = NULL;
    sndBump = NULL;
    sndBreakBlock = NULL;
}

void SoundPlay (Mix_Chunk* snd, uint16 loops)
{
    if (snd == NULL)
        return;

    if (!audioSystemLoaded)
        return;

    Mix_PlayChannel (-1, snd, loops);
}

void StopAllSounds()
{
    Mix_HaltChannel(-1);
}
