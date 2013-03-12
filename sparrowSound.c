/*
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms of the
 GNU Lesser General Public license (the  "LGPL License") version 2 or higher, in
 which case the provisions of LGPL License are applicable instead of those above
 
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "sparrowSound.h"

int spSoundIsInitialized = 0;
int spNoSoundAvaible = 1;
Mix_Music *spBackgroundMusic = NULL;
int spMusicVolume = SP_VOLUME_MAX;
int spSoundVolume = SP_VOLUME_MAX;
int spSoundInPause = 0;

PREFIX int spSoundInit()
{
	if (spSoundIsInitialized)
		return !spNoSoundAvaible;
	spSoundIsInitialized = 1;
	// load support for the OGG and MOD sample/music formats
	#if (SDL_MIXER_PATCHLEVEL >= 10)
		int result = Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_MOD);
		if(result & MIX_INIT_OGG)
			printf("Support for ogg: Yes\n");
		else
			printf("Support for ogg: No (%s)\n",Mix_GetError());
		if(result & MIX_INIT_MP3)
			printf("Support for mp3: Yes\n");
		else
			printf("Support for mp3: No (%s)\n",Mix_GetError());
		if(result & MIX_INIT_FLAC)
			printf("Support for flac: Yes\n");
		else
			printf("Support for flac: No (%s)\n",Mix_GetError());
		if(result & MIX_INIT_MOD)
			printf("Support for mod: Yes\n");
		else
			printf("Support for mod: No (%s)\n",Mix_GetError());
	#endif
	if (!
	#ifdef F100
		Mix_OpenAudio(44100,AUDIO_S16SYS,2,256)
	#else
		Mix_OpenAudio(44100,AUDIO_S16SYS,2,2048)
	#endif
	)
	{
		printf("Try to open Sound: Success\n");
		printf("  %i channels are avaible as default\n",Mix_AllocateChannels(-1));
		spNoSoundAvaible = 0;
	}
	else
	{
		printf("Try to open Sound: Failed\n");
		spNoSoundAvaible = 1;
	}
	spSoundSetMusicVolume(spMusicVolume);
	spSoundSetVolume(spSoundVolume);
	return !spNoSoundAvaible;
}

PREFIX void spSoundSetChannels(int channels)
{
	Mix_AllocateChannels(channels);
}

PREFIX void spSoundPauseAll(int pause)
{
	if (spNoSoundAvaible)
		return;	
	if (pause && !spSoundInPause)
	{
		if (spBackgroundMusic)
			Mix_PauseMusic();
		Mix_Pause(-1);
		spSoundInPause = 1;
	}
	else
	if (!pause && spSoundInPause)
	{
		if (spBackgroundMusic)
			Mix_ResumeMusic();
		Mix_Resume(-1);
		spSoundInPause = 0;
	}
}

PREFIX spSound* spSoundLoad(char* filename)
{
	if (spNoSoundAvaible)
		return NULL;
  spSound* sound = Mix_LoadWAV(filename);
  if (!sound)
    printf("Failed loading \"%s\" because %s\n", filename, Mix_GetError());
  return sound;
}

PREFIX int spSoundPlay(spSound* sound,int channel,int loops,int fadeIn,int maxTime)
{
	if (spNoSoundAvaible && !sound)
		return 0;
	Mix_VolumeChunk(sound,spSoundVolume);
	int result;
	if (fadeIn > 0)
	{
		if (maxTime >= 0)
			result = Mix_FadeInChannelTimed(channel, sound,loops,fadeIn,maxTime);
		else
			result = Mix_FadeInChannel(channel, sound,loops,fadeIn);
	}
	else
	{
		if (maxTime >= 0)
			result = Mix_PlayChannelTimed(channel, sound,loops,maxTime);
		else
			result = Mix_PlayChannel(channel, sound,loops);
	}
	if (result == -1)	
  {
    printf("Failed playing sound because %s\n", Mix_GetError());
    return 0;
  }
  else
  if (spSoundInPause)
		Mix_Pause(result);
  return result;
}

PREFIX void spSoundStop(int soundChannel, int fadeOut)
{
	if (spNoSoundAvaible)
		return;
	if (fadeOut > 0)
		Mix_FadeOutChannel(soundChannel,fadeOut);
	else
		Mix_HaltChannel(soundChannel);
}

PREFIX void spSoundPause(int pause, int channel)
{
	if (spNoSoundAvaible)
		return;	
	if (pause)
	{
		Mix_Pause(channel);
	}
	else
		Mix_Resume(channel);
}

PREFIX void spSoundDelete(spSound* sound)
{
	if (spNoSoundAvaible || !sound)
		return;
	Mix_FreeChunk(sound);
}

PREFIX int spSoundSetMusic(char* filename)
{
	if (spNoSoundAvaible)
		return 0;
	if (spBackgroundMusic)
		Mix_FreeMusic(spBackgroundMusic);
  spBackgroundMusic = Mix_LoadMUS(filename);
  if (!spBackgroundMusic)
  {
    printf("Failed loading \"%s\" because %s\n", filename, Mix_GetError());
    return 0;
  }
  else
		Mix_VolumeMusic(spMusicVolume);
  return 1;
}

PREFIX int spSoundPlayMusic(int fadeIn,int loops)
{
	if (spNoSoundAvaible && !spBackgroundMusic)
		return 0;
	int result;
	if (fadeIn > 0)
		result = Mix_FadeInMusic(spBackgroundMusic, loops, fadeIn);
	else
		result = Mix_PlayMusic(spBackgroundMusic, loops);
	if (result == -1)	
  {
    printf("Failed playing music because %s\n", Mix_GetError());
    return 0;
  }
  return 1;
}

PREFIX int spSoundStopMusic(int fadeOut)
{
	if (spNoSoundAvaible && !spBackgroundMusic)
		return 0;
	int result;
	if (fadeOut > 0)
		result = Mix_FadeOutMusic(fadeOut);
	else
		result = Mix_HaltMusic();
	if (result == -1)	
  {
    printf("Failed halting music because %s\n", Mix_GetError());
    return 0;
  }
  return 1;
}

PREFIX void spSoundPauseMusic(int pause)
{
	if (spNoSoundAvaible)
		return;	
	if (pause)
	{
		if (spBackgroundMusic)
			Mix_PauseMusic();
	}
	else
	if (spBackgroundMusic)
			Mix_ResumeMusic();
}

PREFIX int spSoundGetMusicVolume()
{
	return spMusicVolume;
}

PREFIX void spSoundSetMusicVolume(int volume)
{
	if (volume < 0)
		volume = 0;
	if (volume > SP_VOLUME_MAX)
		volume = SP_VOLUME_MAX;
	spMusicVolume = volume;
	if (!spNoSoundAvaible)
		Mix_VolumeMusic(volume);
}

PREFIX int spSoundGetVolume()
{
	return spSoundVolume;
}

PREFIX void spSoundSetVolume(int volume)
{
	if (volume < 0)
		volume = 0;
	if (volume > SP_VOLUME_MAX)
		volume = SP_VOLUME_MAX;
	spSoundVolume = volume;
	if (!spNoSoundAvaible)
		Mix_Volume(-1,volume);
}

PREFIX void spSoundQuit()
{
	if (spBackgroundMusic)
		Mix_FreeMusic(spBackgroundMusic);
	spBackgroundMusic = NULL;
	Mix_CloseAudio();
	#if (SDL_MIXER_PATCHLEVEL >= 10)
		Mix_Quit();
	#endif
}
