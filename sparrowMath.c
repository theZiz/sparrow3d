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
#include "sparrowMath.h"
#include <stdlib.h>
#include <math.h>

Sint32 spCosvalue[(2*SP_PI)>>SP_COS_ACCURACY];

PREFIX void spInitMath(void)
{
  int a;
  for (a=0;a<((SP_PI<<1)>>SP_COS_ACCURACY);a++)
    spCosvalue[a]=(Sint32)(cos((float)(a<<SP_COS_ACCURACY)/SP_ACCURACY_FACTOR)*SP_ACCURACY_FACTOR);
}

PREFIX Sint32 spSin(Sint32 value)
{
  return spCos(value+((3*SP_PI)>>1));
}

PREFIX Sint32 spCos(Sint32 value)
{
  return spCosvalue[(abs(value)%(SP_PI<<1))>>SP_COS_ACCURACY];
}

//senquack - credit for this fast sqrt goes to Wilco Dijkstra http://www.finesse.demon.co.uk/steven/sqrt.html
#define iter1(N) \
    try_ = root + (1 << (N)); \
    if (n >= try_ << (N))   \
    {   n -= try_ << (N);   \
        root |= 2 << (N); \
    }

PREFIX Sint32 spSqrt (Sint32 n)
{
    Sint32 root = 0, try_;
    iter1 (15);    iter1 (14);    iter1 (13);    iter1 (12);
    iter1 (11);    iter1 (10);    iter1 ( 9);    iter1 ( 8);
    iter1 ( 7);    iter1 ( 6);    iter1 ( 5);    iter1 ( 4);
    iter1 ( 3);    iter1 ( 2);    iter1 ( 1);    iter1 ( 0);
    return root << (SP_HALF_ACCURACY-1);
}


PREFIX Sint32 spMin(Sint32 a,Sint32 b)
{
  if (a<b)
    return a;
  return b;
}

PREFIX Sint32 spMin3(Sint32 a,Sint32 b, Sint32 c)
{
  
}

PREFIX Sint32 spMin4(Sint32 a,Sint32 b, Sint32 c, Sint32 d)
{
  
}

PREFIX Sint32 spMax(Sint32 a,Sint32 b)
{
  
}

PREFIX Sint32 spMax3(Sint32 a,Sint32 b, Sint32 c)
{
  
}

PREFIX Sint32 spMax4(Sint32 a,Sint32 b, Sint32 c, Sint32 d)
{
  
}
