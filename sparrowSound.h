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

/* sparrowSound is for sound output. Most calls and types are just renames of
 * libSDL_mixer functions with error handling.
 * For more details (especially about what is not wrapped here) have a look at
 * http://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_frame.html */

#ifndef _SPARROW_SOUND_H
#define _SPARROW_SOUND_H

#include "sparrowDefines.h"
#include <SDL_mixer.h>

#define SP_VOLUME_MAX MIX_MAX_VOLUME

/* This type ist just renamed for better usage (I don't like the SDL name).
 * Of course you can use the SDL-type-name or use this types with SDL-mixer
 * functions */
typedef Mix_Chunk spSound;

/* This inits spSound. Without not sound will be possible! It returns 1 at
 * success and 0 at failure. If it failure, every spSound function will still
 * work - there will just be no sound. Btw, SDL_mixer would crash ;-) */
PREFIX int spSoundInit();

/* Sets the maximum count of channels. More channels need more space and time
 * to get handled. The default is 8 on most systems */
PREFIX void spSoundSetChannels(int channels);

/* This has to be called at the end of the program */
PREFIX void spSoundQuit();

/* Pauses all music and sounds if 1 and resume with 0. Every new started 
 * sound will be pause, too. */
PREFIX void spSoundPauseAll(int pause);

// ---- background music ----

/* If any sparrow call about background music is called, this Sound file will
 * be used. It can be any often used sound format like ogg, mp3, wav and so one.
 * Sometimes even mid. */
PREFIX int spSoundSetMusic(char* filename);

/* Plays the loaded music from above. If fadeIn is > 0, a fade in of fadeIn
 * milli seconds is made. loops is the amount of times the background music
 * will be played. -1 means "infinite" times*/
PREFIX int spSoundPlayMusic(int fadeIn,int loops);

/* Stops playing the loaded music and uses a fadeOut, if it is greater than 0 */
PREFIX int spSoundStopMusic(int fadeOut);

/* Pauses the music if pause = 1 and resumes with pause = 0 */
PREFIX void spSoundPauseMusic(int pause);

/* Returns the set volume of the music. Music and (all) sounds have different
 * volumes! */
PREFIX int spSoundGetMusicVolume();

/* Sets the volume of the music. Music and (all) sounds have different
 * volumes! The max value is SP_VOLUME_MAX */
PREFIX void spSoundSetMusicVolume(int volume);

// ---- Playing samples ----

/* Loads a sound from the file at filename. Many formats like ogg, mp3 or wav
 * are possible. */
PREFIX spSound* spSoundLoad(char* filename);

/* Plays the sound and returns the channel, where it is played (e.g. for further
 * use with SDL_mixer calls. Most of the times you will not need it. Channel is
 * the channel, where it should be played. If you don't care, use -1, than the
 * choosen channel is returned. But sometimes it makes sense to e.g. use channel
 * 0 for the giant laser canon and channel 1 for the sound of the crying
 * hostages and you don't want to mind them, just always using the same. :)
 * loops determines, how often the sound will be played. 0 means once, -1
 * infinite. In this case you should mind the returned channel to stop it
 * yourself ;). If fadeIn is greater 0 a fade in of fadeIn ms will be done.
 * maxTime is the maximum time of playing the sound, if it doesn't stop before
 * because of being to short or too few loops. */
PREFIX int spSoundPlay(spSound* sound,int channel, int loops,int fadeIn,int maxTime);

/* This stops a channel. You have to stop channels because of spSounds, because
 * on sound can be played multiple times on multiple channels. However: If
 * you pass -1 every sound stops. fadeOut is again a possibility for a fade Out
 * in milli seconds. */
PREFIX void spSoundStop(int soundChannel,int fadeOut);

/* Pauses the channel if pause = 1 and resumes with pause = 0. If you want
 * to pause everything use channel = -1 or spSoundPauseAll if you want to pause
 * all sounds AND the background music. */
PREFIX void spSoundPause(int pause, int channel);

/* Deletes the sound */
PREFIX void spSoundDelete(spSound* sound);

/* Returns the set volume of the sounds. Music and (all) sounds have different
 * volumes! */
PREFIX int spSoundGetVolume();

/* Sets the volume of the sounds. Music and (all) sounds have different
 * volumes! The max value is SP_VOLUME_MAX */
PREFIX void spSoundSetVolume(int volume);

#endif
