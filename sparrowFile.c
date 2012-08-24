/*
 The contents of this file are subject to the Mozilla Public License
 Version 1.1 (the "License"); you may not use this file except in
 compliance with the License. You may obtain a copy of the License at
 http://www.mozilla.org/MPL/

 Software distributed under the License is distributed on an "AS IS"
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 License for the specific language governing rights and limitations
 under the License.

 Alternatively, the contents of this file may be used under the terms
 of the GNU Lesser General Public license (the  "LGPL License"), in which case the
 provisions of LGPL License are applicable instead of those
 above.

 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "sparrowFile.h"

PREFIX int spFileExists( char* filename )
{
  SDL_RWops *file = SDL_RWFromFile(filename, "rb");
  if (file)
  {
    SDL_RWclose(file);
    return 1;
  }
  return 0;
}

PREFIX int spReadOneLine( SDL_RWops *file , char* buffer, int buffer_len)
{
	return spReadUntil(file,buffer,buffer_len,'\n',1);
}

PREFIX int spReadUntil( SDL_RWops *file , char* buffer, int buffer_len, char end_sign,char ignore_windows_return)
{
	int pos = 0;
	if (ignore_windows_return)
		while (pos < buffer_len)
		{
			if (SDL_RWread( file, &(buffer[pos]), 1, 1 ) <= 0)
				return 1; //EOF
			if ( buffer[pos] == end_sign )
				break;
			if (buffer[pos] != '\r') //fucking windows line break
				pos++;
		}
	else
		while (pos < buffer_len)
		{
			if (SDL_RWread( file, &(buffer[pos]), 1, 1 ) <= 0)
				return 1; //EOF
			if ( buffer[pos] == end_sign )
				break;
		}	
	buffer[pos] = 0;
	return 0; //not EOF
}

