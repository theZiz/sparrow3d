 /* This file is part of sparrow3d.
  * Sparrow3d is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * Sparrow3d is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with Foobar.  If not, see <http://www.gnu.org/licenses/>
  * 
  * For feedback and questions about my Files and Projects please mail me,
  * Alexander Matthes (Ziz) , zizsdl_at_googlemail.com */

/* File: sparrowSound
 * 
 * SparrowSound is for sound output. Most calls and types are just
 * renames of libSDL_mixer functions with error handling. For more
 * details (especially about what is not wrapped here) have a look at
 * http://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_frame.html */

#ifndef _SPARROW_SOUND_H
#define _SPARROW_SOUND_H

#include "sparrowDefines.h"
#include <SDL_mixer.h>

/* Define: SP_VOLUME_MAX
 * 
 * The maximal volume of a channel or chunk/spSound. Same as
 * MIX_MAX_VOLUME from SDL Mixer.*/
#define SP_VOLUME_MAX MIX_MAX_VOLUME

/* Type: spSound
 * 
 * This type ist just Mix_Chunk from SDL Mixer renamed for better usage
 * (I don't like the SDL name). Of course you can use the SDL-type-name
 * or use this types with SDL-mixer functions */
typedef Mix_Chunk spSound;

/* Function: spSoundInit
 * 
 * This inits spSound. Without no sound will be possible! It returns 1
 * at success and 0 at failure. If it failure, every spSound function
 * will still be callable without crash - there will just be no sound.
 * 
 * Returns:
 * int - 1 at success, 0 at failure*/
PREFIX int spSoundInit( void );

/* Function: spSoundSetChannels
 * 
 * Sets the maximum count of channels. More channels need more space and
 * time to get handled. The default is 8 on most systems.
 * 
 * Parameters:
 * channels - the new count of channels*/
PREFIX void spSoundSetChannels(int channels);

/* Function: spSoundQuit
 * 
 * This has to be called at the end of the program.*/
PREFIX void spSoundQuit( void );

/* Function: spSoundPauseAll
 * 
 * Pauses or resumes all music and sounds. Every new started sound will
 * be pause, too.
 * 
 * Parameters:
 * pause - 1 if you want to pause, 0 if you want to resume
 * 
 * See Also:
 * <spSoundPause>, <spSoundPauseMusic>*/
PREFIX void spSoundPauseAll(int pause);

/* Functions: Background music functions
 * 
 * These functions are for the background music.*/

/* Function: spSoundSetMusic
 * 
 * Sets the background music file. Every function for background music
 * uses this function. It's like <spBindTexture> for music. ^^' To
 * *start* the music use <spSoundPlayMusic> afterwards.
 * 
 * Parameters:
 * filename - the music file to be used. Many formats are supported
 * depending on the system. ogg, mp3, flac, wav should always be fine,
 * but I already had system without mpe-support. In doubt use ogg or
 * flac.
 * 
 * Returns:
 * int - 1 at success, 0 at failure
 * 
 * See Also:
 * <spSoundLoad>*/
PREFIX int spSoundSetMusic(char* filename);

/* Function: spSoundPlayMusic
 * 
 * Plays the loaded music from <spSoundSetMusic> with optional fade in
 * and repeating.
 * 
 * Parameters:
 * fadeIn - if greater 0, the duration of the fade in
 * loops - the amount of times the background music will be played. -1
 * means "infinite" times and 0 means just once (0 repeats ;))
 * 
 * Returns:
 * int - 1 at success, 0 at failure*/
PREFIX int spSoundPlayMusic(int fadeIn,int loops);

/* Function: spSoundStopMusic
 * 
 * Stops playing the loaded music with optional fade out.
 * 
 * Parameters:
 * fadeOut - if greater 0, the duration of the fade out
 * 
 * Returns:
 * int - 1 at success, 0 at failure*/
PREFIX int spSoundStopMusic(int fadeOut);

/* Function: spSoundPauseMusic
 * 
 * Pauses or resumes the music.
 * 
 * Parameters:
 * pause - 1 pauses and 0 resumes
 * 
 * See Also:
 * <spSoundPauseAll>, <spSoundPauseMusic>*/ 
PREFIX void spSoundPauseMusic(int pause);

/* Function: spSoundSetMusicVolume
 * 
 * Sets the volume of the background music.
 * 
 * Parameters:
 * volume - the new volume of the music. Music and (all) sounds have
 * different volumes!
 * 
 * See Also:
 * <spSoundGetMusicVolume>, <spSoundSetVolume>*/
PREFIX void spSoundSetMusicVolume(int volume);

/* Function: spSoundGetMusicVolume
 * 
 * Returns the volume of the background music.
 * 
 * Returns
 * int - the set volume of the music. Music and (all) sounds have
 * different volumes!
 * 
 * See Also:
 * <spSoundSetMusicVolume>, <spSoundGetVolume>*/
PREFIX int spSoundGetMusicVolume( void );

/* Functions: Sound sample functions
 * 
 * These functions are for the sound samples.*/

/* Function: spSoundLoad
 * 
 * Loads a <spSound> from a file. The loaded file is not played
 * automaticly, use <spSoundPlay> for this.
 * 
 * Parameters:
 * filename - the file to be loaded. Many formats are possible like
 * wav, ogg, mp3 or flac. But some systems don't support everything so
 * it is a good idea to stay with ogg and flac.
 * 
 * See Also:
 * <spSoundSetMusic>*/
PREFIX spSound* spSoundLoad(char* filename);

/* Function: spSoundPlay
 * 
 * Plays a <spSound> sound.
 * 
 * Parameters:
 * sound - the sound to be played
 * channel - if you want to play on a specific channel, use this
 * parameter, if you don't care (what you will most of the time) use -1
 * loops - the amount of times the sound will be played. -1 means
 * "infinite" times and 0 means just once (0 repeats ;))
 * fadeIn - if greater 0, the duration of the fade in
 * maxTime - the maximal time of playing the sound even with "infinite"
 * loops. -1 means no maximal duration
 * 
 * Returns:
 * int - the channel, in which the sound is played*/
PREFIX int spSoundPlay(spSound* sound,int channel, int loops,int fadeIn,int maxTime);

/* Function: spSoundStop
 * 
 * Stops a channel. You have to stop channels instead of spSounds,
 * because one sound can be played multiple times on multiple channels.
 * 
 * Parameters:
 * soundChannel - the channel to be stopped. -1 stops every channel
 * fadeOut - if greater 0 this is the duration of a fade out*/
PREFIX void spSoundStop(int soundChannel,int fadeOut);

/* Function: spSoundPause
 * 
 * Pauses or resumes a channel.
 * 
 * Parameters:
 * pause - use 1 for pause and 0 for resume
 * channel - the channel to be paused or resumed. -1 means every channel
 * is pause or resumed. To pause or resume every channel *and* the
 * background music use <spSoundPauseAll>
 * 
 * See Also:
 * <spSoundPauseAll>, <spSoundPauseMusic>*/
PREFIX void spSoundPause(int pause, int channel);

/* Function: spSoundDelete
 * 
 * Deletes a sound.
 * 
 * Parameters:
 * sound - the sound to be deleted*/
PREFIX void spSoundDelete(spSound* sound);

/* Function: spSoundSetVolume
 * 
 * Sets the volume of all sound.
 * 
 * Parameters:
 * volume - the new volume of all sounds. Music and (all) sounds have
 * different volumes!
 * 
 * See Also:
 * <spSoundGetVolume>, <spSoundSetMusicVolume>*/
PREFIX void spSoundSetVolume(int volume);

/* Function: spSoundGetVolume
 * 
 * Returns the volume of all sounds.
 * 
 * Returns
 * int - the set volume of all sounds. Music and (all) sounds have
 * different volumes!
 * 
 * See Also:
 * <spSoundSetVolume>, <spSoundGetMusicVolume>*/
PREFIX int spSoundGetVolume( void );

#endif
