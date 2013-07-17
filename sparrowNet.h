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

#ifndef _SPARROW_NET_H
#define _SPARROW_NET_H

#include "sparrowDefines.h"

/* File: sparrowNet
 * 
 * SparrowNet / libSparrowNet will be the networking part of sparrow3D.
 * However, at the moment, nothing is implemented.*/
 
typedef struct spNetC4AScoreStruct *spNetC4AScorePointer;
typedef struct spNetC4AScoreStruct {
	spNetC4AScorePointer next;
} spNetC4AScoreType;


/* Just definitions! Not working! */
PREFIX char* spNetC4AGetPrid(char* prid,int max_len);

PREFIX spNetC4AScorePointer spNetC4AGetScore(char* prid,char* game);

PREFIX void spNetC4ADeleteScores(spNetC4AScorePointer firstScore);

PREFIX void spNetC4ACommitScore(char* prid,char* game,int score,spNetC4AScorePointer firstScore);

#endif
