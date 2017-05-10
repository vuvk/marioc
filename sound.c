#include "sound.h"

Mix_Chunk* LoadSound (const char* fileName)
{
    if (!FileExists(fileName))
    {
        printf ("File '%s' doesn't exists!\n");
        return NULL;
    }

    Mix_Chunk* snd = Mix_LoadWAV (fileName);
    return snd;
}

void LoadSounds()
{
    sndJump = LoadSound ("./media/snd/smb_jump-small.wav");
    sndMarioDie = LoadSound ("./media/snd/smb_mariodie.wav");
    sndKick = LoadSound ("./media/snd/smb_kick.wav");
}

void FreeSounds()
{
    Mix_FreeChunk (sndJump);
    Mix_FreeChunk (sndMarioDie);
    Mix_FreeChunk (sndKick);

    sndJump = NULL;
    sndMarioDie = NULL;
    sndKick = NULL;
}

void StopAllSounds()
{
    Mix_HaltChannel(-1);
}
