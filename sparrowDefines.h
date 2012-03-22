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
#ifndef _SPARROW_DEFINES_H
#define _SPARROW_DEFINES_H

#ifdef WIN32
  #include <windows.h>
  #if BUILDING_DLL
    # define DLLIMPORT __declspec (dllexport)
  #else /* Not BUILDING_DLL */
    # define DLLIMPORT __declspec (dllimport)
  #endif /* Not BUILDING_DLL */
  #ifdef __cplusplus
    #define PREFIX extern "C" DLLIMPORT
  #else
    #define PREFIX DLLIMPORT
  #endif
#else
  #ifdef __cplusplus
    #define PREFIX extern "C"
  #else
    #define PREFIX  
  #endif
#endif

//If your application is "flickering" set the ACCURACY to a higher value, but
//keep in mind, that e.g. SP_ACCURACY 20 means, that you have only 12 bits
//(2048) before the comma! And you have to change every of the following values
/*
#define SP_ACCURACY 20
#define SP_HALF_ACCURACY 10
#define SP_ACCURACY_FACTOR 1048576.0f
#define SP_PI 3294199
#define SP_COS_ACCURACY 8
*/

#define SP_ACCURACY 18
#define SP_HALF_ACCURACY 9
#define SP_ACCURACY_FACTOR 262144.0f
#define SP_PI 823550
#define SP_COS_ACCURACY 6

/*
#define SP_ACCURACY 16
#define SP_HALF_ACCURACY 8
#define SP_ACCURACY_FACTOR 65536.0f
#define SP_PI 205888
#define SP_COS_ACCURACY 4
*/
#define SP_JOYSTICK_MIN -16384
#define SP_JOYSTICK_MAX  16383

#define SP_CENTER 0
#define SP_LEFT 1
#define SP_TOP 2
#define SP_RIGHT 3
#define SP_BOTTOM 4

#ifdef FAST_MULTIPLICATION
  #define spMul(a,b) (((a)>>SP_HALF_ACCURACY)*((b)>>SP_HALF_ACCURACY))
#else
  #define spMul(a,b) ((Sint64)(a)*(Sint64)(b)>>SP_ACCURACY)
#endif
#define spMulHigh(a,b) ((Sint64)(a)*(Sint64)(b)>>SP_ACCURACY)


#ifdef FAST_DIVISION
  #define spDiv(a,b) ((b>=0 && b<(1<<SP_PRIM_ACCURACY))? \
                       (a*spGetOne_over_x_pointer()[b]>>SP_PRIM_ACCURACY-SP_ACCURACY): \
                       ( \
                         (b <0 && b>(-1<<SP_PRIM_ACCURACY))? \
                         (-a*spGetOne_over_x_pointer()[-b]>>SP_PRIM_ACCURACY-SP_ACCURACY): \
                         (((a<<SP_HALF_ACCURACY)/b)<<SP_HALF_ACCURACY) \
                       ))
#else
  //#define spDiv(a,b) ((((a)<<SP_HALF_ACCURACY)/(b))<<SP_HALF_ACCURACY)
  #define spDiv(a,b) (((Sint64)(a)<<SP_ACCURACY)/(Sint64)(b))
#endif
#define spDivHigh(a,b) (((Sint64)(a)<<SP_ACCURACY)/(Sint64)(b))


#ifdef GP2X
  #define SP_AXIS_UP 0
  #define SP_AXIS_LEFTUP 1
  #define SP_AXIS_LEFT 2
  #define SP_AXIS_LEFTDOWN 3
  #define SP_AXIS_DOWN 4
  #define SP_AXIS_RIGHTDOWN 5
  #define SP_AXIS_RIGHT 6
  #define SP_AXIS_RIGHTUP 7
  #define SP_BUTTON_START 8
  #define SP_BUTTON_SELECT 9
  #define SP_BUTTON_L 10
  #define SP_BUTTON_R 11
  #define SP_BUTTON_A 12
  #define SP_BUTTON_B 13
  #define SP_BUTTON_X 14
  #define SP_BUTTON_Y 15
  #define SP_BUTTON_VOLPLUS 16
  #define SP_BUTTON_VOLMINUS 17
  #define SP_BUTTON_TIP 18
  //just for different SP_PRActices
  #define SP_PRACTICE_OK SP_BUTTON_B
#elif defined CAANOO
  #define SP_BUTTON_A 0
  #define SP_BUTTON_X 1
  #define SP_BUTTON_B 2
  #define SP_BUTTON_Y 3
  #define SP_BUTTON_L 4
  #define SP_BUTTON_R 5
  #define SP_BUTTON_START 6
  #define SP_BUTTON_TIP 7
  //SELECT == HELP I
  #define SP_BUTTON_SELECT 8
  //the Caanoo has a hardware volume control...
  #define SP_BUTTON_VOLPLUS 18
  #define SP_BUTTON_VOLMINUS 19
  #define SP_PRACTICE_OK SP_BUTTON_B
#else //PC and DINGUX and PANDORA
  #define SP_BUTTON_START 8
  #define SP_BUTTON_SELECT 9
  #define SP_BUTTON_L 10
  #define SP_BUTTON_R 11
  #define SP_BUTTON_A 12
  #define SP_BUTTON_B 13
  #define SP_BUTTON_X 14
  #define SP_BUTTON_Y 15
  #define SP_BUTTON_VOLPLUS 16
  #define SP_BUTTON_VOLMINUS 17
  #define SP_BUTTON_TIP 18
  #define SP_PRACTICE_OK SP_BUTTON_A
#endif

#ifdef F100
  #define SP_BUTTON_START_NAME "Start"
  #define SP_BUTTON_SELECT_NAME "Select"
  #define SP_BUTTON_L_NAME "L"
  #define SP_BUTTON_R_NAME "R"
  #define SP_BUTTON_A_NAME "A"
  #define SP_BUTTON_B_NAME "B"
  #define SP_BUTTON_X_NAME "X"
  #define SP_BUTTON_Y_NAME "Y"
  #define SP_PAD_NAME "D-Pad"
  #define SP_PRACTICE_OK_NAME SP_BUTTON_B_NAME
#elif defined WIZ
  #define SP_BUTTON_START_NAME "Menu"
  #define SP_BUTTON_SELECT_NAME "Select"
  #define SP_BUTTON_L_NAME "L"
  #define SP_BUTTON_R_NAME "R"
  #define SP_BUTTON_A_NAME "A"
  #define SP_BUTTON_B_NAME "B"
  #define SP_BUTTON_X_NAME "X"
  #define SP_BUTTON_Y_NAME "Y"
  #define SP_PAD_NAME "D-Pad"
  #define SP_PRACTICE_OK_NAME SP_BUTTON_B_NAME
#elif defined CAANOO
  #define SP_BUTTON_START_NAME "Home"
  #define SP_BUTTON_SELECT_NAME "Help I"
  #define SP_BUTTON_L_NAME "L"
  #define SP_BUTTON_R_NAME "R"
  #define SP_BUTTON_A_NAME "A"
  #define SP_BUTTON_B_NAME "B"
  #define SP_BUTTON_X_NAME "X"
  #define SP_BUTTON_Y_NAME "Y"
  #define SP_PAD_NAME "Stick"
  #define SP_PRACTICE_OK_NAME SP_BUTTON_B_NAME
#elif defined DINGUX
  #define SP_BUTTON_START_NAME "Start"
  #define SP_BUTTON_SELECT_NAME "Select"
  #define SP_BUTTON_L_NAME "L"
  #define SP_BUTTON_R_NAME "R"
  #define SP_BUTTON_A_NAME "A"
  #define SP_BUTTON_B_NAME "B"
  #define SP_BUTTON_X_NAME "X"
  #define SP_BUTTON_Y_NAME "Y"
  #define SP_PAD_NAME "D-Pad"
  #define SP_PRACTICE_OK_NAME SP_BUTTON_A_NAME  
#elif defined PANDORA
  #define SP_BUTTON_START_NAME "Start"
  #define SP_BUTTON_SELECT_NAME "Select"
  #define SP_BUTTON_L_NAME "L"
  #define SP_BUTTON_R_NAME "R"
  #define SP_BUTTON_A_NAME "A"
  #define SP_BUTTON_B_NAME "B"
  #define SP_BUTTON_X_NAME "X"
  #define SP_BUTTON_Y_NAME "Y"
  #define SP_PAD_NAME "D-Pad"
  #define SP_PRACTICE_OK_NAME SP_BUTTON_A_NAME  
#else
  #define SP_BUTTON_START_NAME "Return"
  #define SP_BUTTON_SELECT_NAME "Backspace"
  #define SP_BUTTON_L_NAME "q"
  #define SP_BUTTON_R_NAME "e"
  #define SP_BUTTON_A_NAME "a"
  #define SP_BUTTON_B_NAME "d"
  #define SP_BUTTON_X_NAME "s"
  #define SP_BUTTON_Y_NAME "w"
  #define SP_PAD_NAME "Arrow Keys"
  #define SP_PRACTICE_OK_NAME SP_BUTTON_A_NAME  
#endif

#endif
