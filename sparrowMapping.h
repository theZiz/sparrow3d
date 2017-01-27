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

/* File: sparrowMapping
 *
 * sparrowMapping is for handling different mappings of controls. You
 * don't to have think like "A is jumping" but "Jump button is jumping".
 * You define your own buttons with mapping to real existing buttons,
 * which are changeable. ;)
 *
 * You first have to define a pool with every button the user shall be able
 * to choice. Afterwards you can add buttons and give the users possibilites to
 * change the button mapping.*/

#ifndef _SPARROW_MAPPING_H
#define _SPARROW_MAPPING_H
#include "sparrowDefines.h"
#include <SDL.h>

/* Define: SP_MAPPING_SET_MAX
 *
 * The total number of different mapping sets to create.*/
#define SP_MAPPING_SET_MAX 16

/* Define: SP_MAPPING_MAX
 *
 * The maximal number of buttons to create. This may appeal strange, why to
 * limit the count of buttons, but most of the sparrow3d does only have 8
 * buttons (most of the time A,B,X,Y,L,R and Start and Select or similar) and
 * using two mappings for one real buttons is not possible.*/
#define SP_MAPPING_MAX 8

/* Defines: Defines for the strategy of <spMapSetStrategy>.
 *
 * SP_MAPPING_NONE - Use no strategy at all. Just set the button.
 * SP_MAPPING_SWITCH - If the button is already used by another mapping switch
 * the mappings. Otherwise just set it.
 * SP_MAPPING_OTHER_INVALID - If the button is already used by another mapping
 * delete the other mapping.
 * SP_MAPPING_CANCEL - If the button is already used, don't set it to the new
 * mapping, but keep the old.*/
#define SP_MAPPING_NONE 0
#define SP_MAPPING_SWITCH 1
#define SP_MAPPING_OTHER_INVALID 2
#define SP_MAPPING_CANCEL 3

/* Function: spInitMapping
 *
 * Inits sparrowMapping. This function should be called by <spInitCore>.*/
PREFIX void spInitMapping( void );

/* Function: spMapClean();
 *
 * Deletes all mappings and all buttons in the pool.*/
PREFIX void spMapClean( void );

/* Function: spMapPoolAdd
 *
 * Every button, which shall be useable by the user have to be added to the
 * button pool.
 *
 * Parameters:
 * button_id - the real sparrow3d button id of the button, e.g. SP_BUTTON_A or
 * SP_BUTTON_LEFT.
 * caption - the caption of the button. Later used from <spMapButtonByID> and
 * <spMapButtonByName>.*/
PREFIX void spMapPoolAdd(int button_id,char* caption);

/* Function: spMapSetStrategy
 *
 * Sets the strategy to be used if a button mapping is changed, but the mapping
 * already exists.
 *
 * Parameters:
 * strategy - may be SP_MAPPING_NONE, which means no strategy use,
 * SP_MAPPING_SWITCH, which means, that the mappings are switched at conflict,
 * SP_MAPPING_OTHER_INVALID, which deletes the conflict mapping if it already
 * exists or SP_MAPPING_CANCEL, which means, if the button is already used,
 * don't set it to the new mapping, but keep the old one.*/
PREFIX void spMapSetStrategy(int strategy);

/* Function: spMapButtonAdd
 *
 * Adds a semantic button identified by name or a self given ID (advise: Use
 * #define MY_FIRE_BUTTON 0 or similar for the id).
 *
 * Parameters:
 * id - the self choosen id. Have to be between 0 and SP_MAPPING_MAX-1.
 * name - the name of the semantic button. However identifying via id is much
 * faster! However name is needed for saving the button mapping in a config file
 * caption - the caption of the button, may be "Fire" or "Select", whatever you
 * need. The user should semanticly understand this caption
 * poolButton - the real sparrow3d button (e.g. SP_BUTTON_A) to be used. Don't
 * forget to add this button via <spMapPoolAdd> _before_!*/
PREFIX void spMapButtonAdd(int id,char* name,char* caption,int poolButton);

/* Function: spMapChange
 *
 * Changes the mapping of an existing button with the setup strategy (see
 * <spMapSetStrategy>).
 *
 * Parameters:
 * id - button id to change
 * poolButton - real sparrow3d button to change the mapping to. Don't forget to
 * add this button to the pool first with <spMapPoolAdd>!
 *
 * Returns:
 * int - 0 if the button was changed or 1 if not. This can happen, if the
 * strategy is <SP_MAPPING_CANCEL> and the button was already used. Returns
 * -1 at error.*/
PREFIX int spMapChange(int id,int poolButton);

/* Function: spMapChangeNextInPool
 *
 * Sets the mapping to the next possible button in the pool.
 *
 * Parameters:
 * id - id the change.
 *
 * See also:
 * <spMapChangePreviousInPool>*/
PREFIX void spMapChangeNextInPool(int id);

/* Function: spMapChangePreviousInPool
 *
 * Sets the mapping to the previous possible button in the pool.
 *
 * Parameters:
 * id - id the change.
 *
 * See also:
 * <spMapChangeNextInPool>*/
PREFIX void spMapChangePreviousInPool(int id);

/* Function: spMapGetByID
 *
 * Returns the state of the button given by the id. This is much faster than
 * <spMapGetByName>.
 *
 * Parameters:
 * id - the id of the button
 *
 * Returns:
 * int - 0 not pressed, 1 pressed*/
PREFIX int spMapGetByID(int id);

/* Function: spMapSetByID
 *
 * Sets the state of the button given by the id. This is much faster than
 * <spMapSetByName>.
 *
 * Parameters:
 * id - the id of the button
 * value - value to set, where 0 means not pressed and 1 means pressed*/
PREFIX void spMapSetByID(int id, int value);

/* Function: spMapGetByName
 *
 * Returns the state of the button given by the name. This is much slower than
 * <spMapGetByID>.
 *
 * Parameters:
 * name - the name of the button
 *
 * Returns:
 * int - 0 not pressed, 1 pressed*/
PREFIX int spMapGetByName(char* name);

/* Function: spMapSetByName
 *
 * Sets the state of the button given by the name. This is much slower than
 * <spMapSetByID>.
 *
 * Parameters:
 * name - the name of the button
 * value - value to set, where 0 means not pressed and 1 means pressed*/
PREFIX void spMapSetByName(char* name, int value);

/* Function: spMapCaptionByID
 *
 * Returns the caption of the semantic button via it's ID. Much faster than
 * <spMapCaptionByName>.
 *
 * Parameters:
 * id - the id of the button
 *
 * Returns:
 * char* - the name of the button*/
PREFIX char* spMapCaptionByID(int id);

/* Function: spMapCaptionByName
 *
 * Returns the caption of the semantic button via it's name. Much slower than
 * <spMapCaptionByID>.
 *
 * Parameters:
 * name - the name of the button
 *
 * Returns:
 * char* - the name of the button*/
PREFIX char* spMapCaptionByName(char* name);

/* Function: spMapButtonByID
 *
 * Returns the caption of the real button behind the mapping via it's ID. Much
 * faster than <spMapButtonByName>.
 *
 * Parameters:
 * id - the id of the button
 *
 * Returns:
 * char* - the name of the mapped button*/
PREFIX char* spMapButtonByID(int id);

/* Function: spMapButtonByName
 *
 * Returns the caption of the real button behind the mapping via it's name. Much
 * slower than <spMapButtonByID>.
 *
 * Parameters:
 * name - the name of the button
 *
 * Returns:
 * char* - the name of the mapped button*/
PREFIX char* spMapButtonByName(char* name);

/* Function: spMapPoolByID
 *
 * Returns the real button id of the real button behind the mapping via it's ID.
 *
 * Parameters:
 * id - the id of the button
 *
 * Returns:
 * int - the button id of the mapped button like SP_BUTTON_A*/
PREFIX int spMapPoolByID(int id);

/* Function: spMapPoolByName
 *
 * Returns the real button id of the real button behind the mapping via it's name.
 * Much slower than <spMapPoolByID>.
 *
 * Parameters:
 * name - the name of the button
 *
 * Returns:
 * int - the button id of the mapped button like SP_BUTTON_A*/
PREFIX int spMapPoolByName(char* name);

/* Function: spMapNameByID
 *
 * Returns the mapping name via it's ID.
 *
 * Parameters:
 * id - the id of the button
 *
 * Returns:
 * char* - the button's mapping name*/
PREFIX char* spMapNameByID(int id);

/* Function: spMapIDByName
 *
 * Returns the mapping ID via it's name.
 *
 * Parameters:
 * name - the name of the button
 *
 * Returns:
 * int - the button's mapping ID*/
PREFIX int spMapIDByName(char* name);

/* Function: spMapStartChangeByID
 *
 * Starts the changing of the mapping of a buttons identified by it's ID. Use
 * <spMapContinueChange> to interpretate the user input.
 *
 * Parameters:
 * id - the id of the semantic button to change it's mapping*/
PREFIX void spMapStartChangeByID(int id);

/* Function: spMapStartChangeByName
 *
 * Starts the changing of the mapping of a buttons identified by it's name. Use
 * <spMapContinueChange> to interpretate the user input.
 *
 * Parameters:
 * name - the name of the semantic button to change it's mapping*/
PREFIX void spMapStartChangeByName(char* name);

/* Function: spMapContinueChange
 *
 * Interpretates to user input to change the mapping of a semantic button. The
 * whole process is started with <spMapStartChangeByID> or
 * <spMapStartChangeByName>.
 *
 * Returns:
 * int - 0 means changes are still in progress, so maybe some functions of the
 * program should be supressed like going back to another menu or similar.
 * 1 means the button mapping has been changed just now with this call, what is
 * e.g. a good time to save the mapping. -1 means no button changing process is
 * running right know. If 2 is returned the function is done, but no new button
 * was set. That is the case, if the strategy is <SP_MAPPING_CANCEL>, but
 * the mapped was already in use. The collision semantic button and pool button
 * can be read via <spMapLastCollisionName> and <spMapLastCollisionPool>. */
PREFIX int spMapContinueChange( void );

/* Function: spMapLastCollisionName
 *
 * Returns the name of the semantic button caption of last collision if
 * <spMapContinueChange> returned 2.
 *
 * Returns:
 * char* - caption of the semantic button, may be NULL!*/
PREFIX char* spMapLastCollisionCaption( void );

/* Function: spMapLastCollisionPool
 *
 * Returns the name of the pool button of last collision if
 * <spMapContinueChange> returned 2.
 *
 * Returns:
 * char* - name of the pool button, may be NULL!*/
PREFIX char* spMapLastCollisionPool( void );

/* Function: spMapCancelChange
 *
 * Cancels the mapping change started with <spMapStartChangeByID> or
 * <spMapStartChangeByName>. No new mapping is set! <spMapContinueChange> will
 * just throw -1 like always when nothing is to do. ;) */
PREFIX void spMapCancelChange( void );

/* Function: spMapLoad
 *
 * Loads a mapping. Important: You should first create the pool, than add your
 * semantic buttons (same as in the saved mapping!) and then load the config. If
 * no config is there yet, the value set by you will be kept.
 *
 * Parameters:
 * subfolder - subfolder for the config to be used. Not used on any system, but
 * this should be the name of your application, so that the config is read e.g.
 * from ~/.config/subfolder/filename
 * filename - filename to read from*/
PREFIX void spMapLoad(char* subfolder,char* filename);

/* Function: spMapSave
 *
 * Saves a mapping. Important: You should first create the pool, than add your
 * semantic buttons (same as in the saved mapping!) and then load the config. If
 * no config is there yet, the value set by you will be kept.
 *
 * Parameters:
 * subfolder - subfolder for the config to be used. Not used on any system, but
 * this should be the name of your application, so that the config is saved e.g.
 * to ~/.config/subfolder/filename
 * filename - filename to save to*/
PREFIX void spMapSave(char* subfolder,char* filename);

PREFIX void spMapPoolAddForDesktopHack();

PREFIX void spMapDesktopHack(int value);

PREFIX unsigned char spMapSDLKeyToChar(SDLKey key);

PREFIX void spMapSetMapSet(int set);
#endif
