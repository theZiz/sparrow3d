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
#include "sparrowRenderer.h"
#include "sparrowCore.h"
#include "sparrowPrimitives.h"
#include "sparrowMath.h"
#define M_PI 3.14159265358979323846
#include <math.h>
#include <string.h>

Sint32 spModelView[16];
Sint32 spProjection[16];
Sint32 spX_to_Y;

/*inline Sint32 fpdiv(Sint32 a,Sint32 b)
{
  #ifdef FAST_BUT_UGLY_2
  if (b>=0 && b<(1<<SP_PRIM_ACCURACY))
    return a*spGetOne_over_x_pointer()[b]>>SP_PRIM_ACCURACY-SP_ACCURACY;
  if (b <0 && b>(-1<<SP_PRIM_ACCURACY))
    return -a*spGetOne_over_x_pointer()[-b]>>SP_PRIM_ACCURACY-SP_ACCURACY;
  #endif
  return ((a<<SP_HALF_ACCURACY)/b)<<SP_HALF_ACCURACY;
}*/

#ifdef FAST_BUT_UGLY_2
  #define fpdiv(a,b) (b>=0 && b<(1<<SP_PRIM_ACCURACY))? \
                       (a*spGetOne_over_x_pointer()[b]>>SP_PRIM_ACCURACY-SP_ACCURACY): \
                       ( \
                         (b <0 && b>(-1<<SP_PRIM_ACCURACY))? \
                         (-a*spGetOne_over_x_pointer()[-b]>>SP_PRIM_ACCURACY-SP_ACCURACY): \
                         (((a<<SP_HALF_ACCURACY)/b)<<SP_HALF_ACCURACY) \
                       )
#else
  #define fpdiv(a,b) (((a<<SP_HALF_ACCURACY)/b)<<SP_HALF_ACCURACY)
#endif

inline void spSetFrustumf2(Sint32 *matrix, Sint32 left, Sint32 right, Sint32 bottom, Sint32 top,
                           Sint32 znear, Sint32 zfar)
{
    Sint32 temp, temp2, temp3, temp4;
    temp = 2 * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    matrix[0] = fpdiv(temp,temp2);
    matrix[1] = 0<<SP_ACCURACY;
    matrix[2] = 0<<SP_ACCURACY;
    matrix[3] = 0<<SP_ACCURACY;
    matrix[4] = 0<<SP_ACCURACY;
    matrix[5] = fpdiv(temp,temp3);
    matrix[6] = 0<<SP_ACCURACY;
    matrix[7] = 0<<SP_ACCURACY;
    matrix[8] = fpdiv(right + left,temp2);
    matrix[9] = fpdiv(top + bottom,temp3);
    matrix[10] = fpdiv(-zfar - znear,temp4);
    matrix[11] = -1<<SP_ACCURACY;
    matrix[12] = 0<<SP_ACCURACY;
    matrix[13] = 0<<SP_ACCURACY;
    matrix[14] = fpdiv((-temp >> SP_HALF_ACCURACY) * (zfar >> SP_HALF_ACCURACY),temp4);
    matrix[15] = 0<<SP_ACCURACY;
}

PREFIX void spSetPerspective(float fovyInDegrees, float aspectRatio,
                             float znear, float zfar)
{
    float ymax, xmax;
    ymax = znear * tanf(fovyInDegrees * M_PI / 360.0);
    //ymin = -ymax;
    //xmin = -ymax * aspectRatio;
    xmax = ymax * aspectRatio;
    spSetFrustumf2(spProjection,(Sint32)(-xmax*SP_ACCURACY_FACTOR),
                                (Sint32)(xmax*SP_ACCURACY_FACTOR),
                                (Sint32)(-ymax*SP_ACCURACY_FACTOR),
                                (Sint32)(ymax*SP_ACCURACY_FACTOR),
                                (Sint32)(znear*SP_ACCURACY_FACTOR),
                                (Sint32)(zfar*SP_ACCURACY_FACTOR));
    Sint32 spRatio = (Sint32)(aspectRatio*SP_ACCURACY_FACTOR);
    spX_to_Y = (spGetWindowSurface()->w<<SP_ACCURACY)/spGetWindowSurface()->h;
}

PREFIX void spIdentity()
{
  spModelView[ 0] = 1<<SP_ACCURACY;
  spModelView[ 1] = 0<<SP_ACCURACY;
  spModelView[ 2] = 0<<SP_ACCURACY;
  spModelView[ 3] = 0<<SP_ACCURACY;
  spModelView[ 4] = 0<<SP_ACCURACY;
  spModelView[ 5] = 1<<SP_ACCURACY;
  spModelView[ 6] = 0<<SP_ACCURACY;
  spModelView[ 7] = 0<<SP_ACCURACY;
  spModelView[ 8] = 0<<SP_ACCURACY;
  spModelView[ 9] = 0<<SP_ACCURACY;
  spModelView[10] = 1<<SP_ACCURACY;
  spModelView[11] = 0<<SP_ACCURACY;
  spModelView[12] = 0<<SP_ACCURACY;
  spModelView[13] = 0<<SP_ACCURACY;
  spModelView[14] = 0<<SP_ACCURACY;
  spModelView[15] = 1<<SP_ACCURACY;
}

PREFIX void spScale(Sint32 x,Sint32 y,Sint32 z)
{
  spModelView[ 0]=(spModelView[ 0]>>SP_HALF_ACCURACY)*(x>>SP_HALF_ACCURACY);
	spModelView[ 5]=(spModelView[ 5]>>SP_HALF_ACCURACY)*(y>>SP_HALF_ACCURACY);
	spModelView[10]=(spModelView[10]>>SP_HALF_ACCURACY)*(z>>SP_HALF_ACCURACY);
}  

/*inline Sint32 fimul(Sint32 a,Sint32 b)
{
  return (a>>SP_HALF_ACCURACY)*(b>>SP_HALF_ACCURACY);
}*/

#define fimul(a,b) (a>>SP_HALF_ACCURACY)*(b>>SP_HALF_ACCURACY)

PREFIX void spRotate(Sint32 x,Sint32 y,Sint32 z,Sint32 rad)
{
	//Rotation matrix:
	Sint32 s=spSin(rad);
	Sint32 c=spCos(rad);
  Sint32 l = spSqrt((x>>SP_HALF_ACCURACY)*(x>>SP_HALF_ACCURACY)
                   +(y>>SP_HALF_ACCURACY)*(y>>SP_HALF_ACCURACY)
                   +(z>>SP_HALF_ACCURACY)*(z>>SP_HALF_ACCURACY));
	if (l==0)
	  return;
  x = fpdiv(x,l);
  y = fpdiv(y,l);
  z = fpdiv(z,l);
	Sint32 rotate[16];
  rotate[ 0]= c+fimul(fimul(x,x),(1<<SP_ACCURACY)-c);
  rotate[ 4]=   fimul(fimul(x,y),(1<<SP_ACCURACY)-c)-fimul(z,s);
  rotate[ 8]=   fimul(fimul(x,z),(1<<SP_ACCURACY)-c)+fimul(y,s);
  rotate[12]= 0;
  rotate[ 1]=   fimul(fimul(y,x),(1<<SP_ACCURACY)-c)+fimul(z,s);
  rotate[ 5]= c+fimul(fimul(y,y),(1<<SP_ACCURACY)-c);
  rotate[ 9]=   fimul(fimul(y,z),(1<<SP_ACCURACY)-c)-fimul(x,s);
  rotate[13]= 0;
  rotate[ 2]=   fimul(fimul(z,x),(1<<SP_ACCURACY)-c)-fimul(y,s);
  rotate[ 6]=   fimul(fimul(z,y),(1<<SP_ACCURACY)-c)+fimul(x,s);
  rotate[10]= c+fimul(fimul(z,z),(1<<SP_ACCURACY)-c);
  rotate[14]= 0;
  rotate[ 3]= 0;
  rotate[ 7]= 0;
  rotate[11]= 0;
  rotate[15]= 1<<SP_ACCURACY;

	Sint32 result[12];
	result[ 0]=(spModelView[ 0]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[ 4]>>SP_HALF_ACCURACY)*(rotate[ 1]>>SP_HALF_ACCURACY)+(spModelView[ 8]>>SP_HALF_ACCURACY)*(rotate[ 2]>>SP_HALF_ACCURACY);
	result[ 1]=(spModelView[ 1]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[ 5]>>SP_HALF_ACCURACY)*(rotate[ 1]>>SP_HALF_ACCURACY)+(spModelView[ 9]>>SP_HALF_ACCURACY)*(rotate[ 2]>>SP_HALF_ACCURACY);
	result[ 2]=(spModelView[ 2]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[ 6]>>SP_HALF_ACCURACY)*(rotate[ 1]>>SP_HALF_ACCURACY)+(spModelView[10]>>SP_HALF_ACCURACY)*(rotate[ 2]>>SP_HALF_ACCURACY);
	result[ 3]=(spModelView[ 3]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[ 7]>>SP_HALF_ACCURACY)*(rotate[ 1]>>SP_HALF_ACCURACY)+(spModelView[11]>>SP_HALF_ACCURACY)*(rotate[ 2]>>SP_HALF_ACCURACY);

	result[ 4]=(spModelView[ 0]>>SP_HALF_ACCURACY)*(rotate[ 4]>>SP_HALF_ACCURACY)+(spModelView[ 4]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY)+(spModelView[ 8]>>SP_HALF_ACCURACY)*(rotate[ 6]>>SP_HALF_ACCURACY);
	result[ 5]=(spModelView[ 1]>>SP_HALF_ACCURACY)*(rotate[ 4]>>SP_HALF_ACCURACY)+(spModelView[ 5]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY)+(spModelView[ 9]>>SP_HALF_ACCURACY)*(rotate[ 6]>>SP_HALF_ACCURACY);
	result[ 6]=(spModelView[ 2]>>SP_HALF_ACCURACY)*(rotate[ 4]>>SP_HALF_ACCURACY)+(spModelView[ 6]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY)+(spModelView[10]>>SP_HALF_ACCURACY)*(rotate[ 6]>>SP_HALF_ACCURACY);
	result[ 7]=(spModelView[ 3]>>SP_HALF_ACCURACY)*(rotate[ 4]>>SP_HALF_ACCURACY)+(spModelView[ 7]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY)+(spModelView[11]>>SP_HALF_ACCURACY)*(rotate[ 6]>>SP_HALF_ACCURACY);

	result[ 8]=(spModelView[ 0]>>SP_HALF_ACCURACY)*(rotate[ 8]>>SP_HALF_ACCURACY)+(spModelView[ 4]>>SP_HALF_ACCURACY)*(rotate[ 9]>>SP_HALF_ACCURACY)+(spModelView[ 8]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);
	result[ 9]=(spModelView[ 1]>>SP_HALF_ACCURACY)*(rotate[ 8]>>SP_HALF_ACCURACY)+(spModelView[ 5]>>SP_HALF_ACCURACY)*(rotate[ 9]>>SP_HALF_ACCURACY)+(spModelView[ 9]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);
	result[10]=(spModelView[ 2]>>SP_HALF_ACCURACY)*(rotate[ 8]>>SP_HALF_ACCURACY)+(spModelView[ 6]>>SP_HALF_ACCURACY)*(rotate[ 9]>>SP_HALF_ACCURACY)+(spModelView[10]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);
	result[11]=(spModelView[ 3]>>SP_HALF_ACCURACY)*(rotate[ 8]>>SP_HALF_ACCURACY)+(spModelView[ 7]>>SP_HALF_ACCURACY)*(rotate[ 9]>>SP_HALF_ACCURACY)+(spModelView[11]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);

	memcpy(spModelView,result,sizeof(Sint32)*12);
}

PREFIX void spRotateX(Sint32 rad)
{
	//Rotation matrix:
	Sint32 s=spSin(rad);
	Sint32 c=spCos(rad);

	Sint32 rotate[16];
  rotate[ 0]= 1<<SP_ACCURACY;
  rotate[ 4]= 0;
  rotate[ 8]= 0;
  rotate[12]= 0;
  rotate[ 1]= 0;
  rotate[ 5]= c;
  rotate[ 9]= -s;
  rotate[13]= 0;
  rotate[ 2]= 0;
  rotate[ 6]= s;
  rotate[10]= c;
  rotate[14]= 0;
  rotate[ 3]= 0;
  rotate[ 7]= 0;
  rotate[11]= 0;
  rotate[15]= 1<<SP_ACCURACY;

	Sint32 result[8];
  //+4
	result[ 0]=(spModelView[ 4]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY)+(spModelView[ 8]>>SP_HALF_ACCURACY)*(rotate[ 6]>>SP_HALF_ACCURACY);
	result[ 1]=(spModelView[ 5]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY)+(spModelView[ 9]>>SP_HALF_ACCURACY)*(rotate[ 6]>>SP_HALF_ACCURACY);
	result[ 2]=(spModelView[ 6]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY)+(spModelView[10]>>SP_HALF_ACCURACY)*(rotate[ 6]>>SP_HALF_ACCURACY);
	result[ 3]=(spModelView[ 7]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY)+(spModelView[11]>>SP_HALF_ACCURACY)*(rotate[ 6]>>SP_HALF_ACCURACY);

	result[ 4]=(spModelView[ 4]>>SP_HALF_ACCURACY)*(rotate[ 9]>>SP_HALF_ACCURACY)+(spModelView[ 8]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);
	result[ 5]=(spModelView[ 5]>>SP_HALF_ACCURACY)*(rotate[ 9]>>SP_HALF_ACCURACY)+(spModelView[ 9]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);
	result[ 6]=(spModelView[ 6]>>SP_HALF_ACCURACY)*(rotate[ 9]>>SP_HALF_ACCURACY)+(spModelView[10]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);
	result[ 7]=(spModelView[ 7]>>SP_HALF_ACCURACY)*(rotate[ 9]>>SP_HALF_ACCURACY)+(spModelView[11]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);

	memcpy(&(spModelView[4]),result,sizeof(Sint32)*8);
}

PREFIX void spRotateY(Sint32 rad)
{
	//Rotation matrix:
	Sint32 s=spSin(rad);
	Sint32 c=spCos(rad);

	Sint32 rotate[16];
  rotate[ 0]= c;
  rotate[ 4]= 0;
  rotate[ 8]= s;
  rotate[12]= 0;
  rotate[ 1]= 0;
  rotate[ 5]= 1<<SP_ACCURACY;
  rotate[ 9]= 0;
  rotate[13]= 0;
  rotate[ 2]= -s;
  rotate[ 6]= 0;
  rotate[10]= c;
  rotate[14]= 0;
  rotate[ 3]= 0;
  rotate[ 7]= 0;
  rotate[11]= 0;
  rotate[15]= 1<<SP_ACCURACY;

	Sint32 result[4];
	Sint32 result_8[4];
	result[ 0]=(spModelView[ 0]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[ 8]>>SP_HALF_ACCURACY)*(rotate[ 2]>>SP_HALF_ACCURACY);
	result[ 1]=(spModelView[ 1]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[ 9]>>SP_HALF_ACCURACY)*(rotate[ 2]>>SP_HALF_ACCURACY);
	result[ 2]=(spModelView[ 2]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[10]>>SP_HALF_ACCURACY)*(rotate[ 2]>>SP_HALF_ACCURACY);
	result[ 3]=(spModelView[ 3]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[11]>>SP_HALF_ACCURACY)*(rotate[ 2]>>SP_HALF_ACCURACY);
  //+8!
	result_8[ 0]=(spModelView[ 0]>>SP_HALF_ACCURACY)*(rotate[ 8]>>SP_HALF_ACCURACY)+(spModelView[ 8]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);
	result_8[ 1]=(spModelView[ 1]>>SP_HALF_ACCURACY)*(rotate[ 8]>>SP_HALF_ACCURACY)+(spModelView[ 9]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);
	result_8[ 2]=(spModelView[ 2]>>SP_HALF_ACCURACY)*(rotate[ 8]>>SP_HALF_ACCURACY)+(spModelView[10]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);
	result_8[ 3]=(spModelView[ 3]>>SP_HALF_ACCURACY)*(rotate[ 8]>>SP_HALF_ACCURACY)+(spModelView[11]>>SP_HALF_ACCURACY)*(rotate[10]>>SP_HALF_ACCURACY);
	memcpy(spModelView,result,sizeof(Sint32)*4);
	memcpy(&(spModelView[8]),result_8,sizeof(Sint32)*4);
}

PREFIX void spRotateZ(Sint32 rad)
{
	//Rotation matrix:
	Sint32 s=spSin(rad);
	Sint32 c=spCos(rad);

	Sint32 rotate[16];
  rotate[ 0]= c;
  rotate[ 4]= -s;
  rotate[ 8]= 0;
  rotate[12]= 0;
  rotate[ 1]= s;
  rotate[ 5]= c;
  rotate[ 9]= 0;
  rotate[13]= 0;
  rotate[ 2]= 0;
  rotate[ 6]= 0;
  rotate[10]= 1<<SP_ACCURACY;
  rotate[14]= 0;
  rotate[ 3]= 0;
  rotate[ 7]= 0;
  rotate[11]= 0;
  rotate[15]= 1<<SP_ACCURACY;

	Sint32 result[8];
	result[ 0]=(spModelView[ 0]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[ 4]>>SP_HALF_ACCURACY)*(rotate[ 1]>>SP_HALF_ACCURACY);
	result[ 1]=(spModelView[ 1]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[ 5]>>SP_HALF_ACCURACY)*(rotate[ 1]>>SP_HALF_ACCURACY);
	result[ 2]=(spModelView[ 2]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[ 6]>>SP_HALF_ACCURACY)*(rotate[ 1]>>SP_HALF_ACCURACY);
	result[ 3]=(spModelView[ 3]>>SP_HALF_ACCURACY)*(rotate[ 0]>>SP_HALF_ACCURACY)+(spModelView[ 7]>>SP_HALF_ACCURACY)*(rotate[ 1]>>SP_HALF_ACCURACY);

	result[ 4]=(spModelView[ 0]>>SP_HALF_ACCURACY)*(rotate[ 4]>>SP_HALF_ACCURACY)+(spModelView[ 4]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY);
	result[ 5]=(spModelView[ 1]>>SP_HALF_ACCURACY)*(rotate[ 4]>>SP_HALF_ACCURACY)+(spModelView[ 5]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY);
	result[ 6]=(spModelView[ 2]>>SP_HALF_ACCURACY)*(rotate[ 4]>>SP_HALF_ACCURACY)+(spModelView[ 6]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY);
	result[ 7]=(spModelView[ 3]>>SP_HALF_ACCURACY)*(rotate[ 4]>>SP_HALF_ACCURACY)+(spModelView[ 7]>>SP_HALF_ACCURACY)*(rotate[ 5]>>SP_HALF_ACCURACY);

	memcpy(spModelView,result,sizeof(Sint32)*8);
}

PREFIX void spTranslate(Sint32 x,Sint32 y,Sint32 z)
{
  spModelView[12]=(spModelView[ 0]>>SP_HALF_ACCURACY)*(x>>SP_HALF_ACCURACY)+(spModelView[ 4]>>SP_HALF_ACCURACY)*(y>>SP_HALF_ACCURACY)+(spModelView[ 8]>>SP_HALF_ACCURACY)*(z>>SP_HALF_ACCURACY)+spModelView[12];
	spModelView[13]=(spModelView[ 1]>>SP_HALF_ACCURACY)*(x>>SP_HALF_ACCURACY)+(spModelView[ 5]>>SP_HALF_ACCURACY)*(y>>SP_HALF_ACCURACY)+(spModelView[ 9]>>SP_HALF_ACCURACY)*(z>>SP_HALF_ACCURACY)+spModelView[13];
	spModelView[14]=(spModelView[ 2]>>SP_HALF_ACCURACY)*(x>>SP_HALF_ACCURACY)+(spModelView[ 6]>>SP_HALF_ACCURACY)*(y>>SP_HALF_ACCURACY)+(spModelView[10]>>SP_HALF_ACCURACY)*(z>>SP_HALF_ACCURACY)+spModelView[14];
	spModelView[15]=(spModelView[ 3]>>SP_HALF_ACCURACY)*(x>>SP_HALF_ACCURACY)+(spModelView[ 7]>>SP_HALF_ACCURACY)*(y>>SP_HALF_ACCURACY)+(spModelView[11]>>SP_HALF_ACCURACY)*(z>>SP_HALF_ACCURACY)+spModelView[15];
}

PREFIX Sint32* spGetMatrix()
{
  return spModelView;
}

PREFIX void spSetMatrix(Sint32* matrix)
{
  memcpy(spModelView,matrix,16*sizeof(Sint32));
}

inline void spCalcNormal(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 x2,Sint32 y2,Sint32 z2,
                         Sint32 x3,Sint32 y3,Sint32 z3,Sint32* normale)
{
  normale[0]=((y1-y2)>>SP_HALF_ACCURACY)*((z2-z3)>>SP_HALF_ACCURACY)
            -((z1-z2)>>SP_HALF_ACCURACY)*((y2-y3)>>SP_HALF_ACCURACY);
  normale[1]=((z1-z2)>>SP_HALF_ACCURACY)*((x2-x3)>>SP_HALF_ACCURACY)
            -((x1-x2)>>SP_HALF_ACCURACY)*((z2-z3)>>SP_HALF_ACCURACY);
  normale[2]=((x1-x2)>>SP_HALF_ACCURACY)*((y2-y3)>>SP_HALF_ACCURACY)
            -((y1-y2)>>SP_HALF_ACCURACY)*((x2-x3)>>SP_HALF_ACCURACY);
}

inline void spMulModellView(Sint32 x,Sint32 y,Sint32 z,Sint32 *tx,Sint32 *ty,Sint32 *tz,Sint32 *tw)
{
  (*tx) = (spModelView[ 0] >> SP_HALF_ACCURACY)*(x >> SP_HALF_ACCURACY)
        + (spModelView[ 4] >> SP_HALF_ACCURACY)*(y >> SP_HALF_ACCURACY)
        + (spModelView[ 8] >> SP_HALF_ACCURACY)*(z >> SP_HALF_ACCURACY)
        + (spModelView[12]);// >> SP_HALF_ACCURACY)*( 1 << SP_HALF_ACCURACY);
  (*ty) = (spModelView[ 1] >> SP_HALF_ACCURACY)*(x >> SP_HALF_ACCURACY)
        + (spModelView[ 5] >> SP_HALF_ACCURACY)*(y >> SP_HALF_ACCURACY)
        + (spModelView[ 9] >> SP_HALF_ACCURACY)*(z >> SP_HALF_ACCURACY)
        + (spModelView[13]);// >> SP_HALF_ACCURACY)*( 1 << SP_HALF_ACCURACY);
  (*tz) = (spModelView[ 2] >> SP_HALF_ACCURACY)*(x >> SP_HALF_ACCURACY)
        + (spModelView[ 6] >> SP_HALF_ACCURACY)*(y >> SP_HALF_ACCURACY)
        + (spModelView[10] >> SP_HALF_ACCURACY)*(z >> SP_HALF_ACCURACY)
        + (spModelView[14]);// >> SP_HALF_ACCURACY)*( 1 << SP_HALF_ACCURACY);
  (*tw) = (spModelView[ 3] >> SP_HALF_ACCURACY)*(x >> SP_HALF_ACCURACY)
        + (spModelView[ 7] >> SP_HALF_ACCURACY)*(y >> SP_HALF_ACCURACY)
        + (spModelView[11] >> SP_HALF_ACCURACY)*(z >> SP_HALF_ACCURACY)
        + (spModelView[15]);// >> SP_HALF_ACCURACY)*( 1 << SP_HALF_ACCURACY);
}

PREFIX int spTriangle3D(Sint32 x1,Sint32 y1,Sint32 z1,
  Sint32 x2,Sint32 y2,Sint32 z2,
  Sint32 x3,Sint32 y3,Sint32 z3,Uint16 color)
{
  int windowX = spGetWindowSurface()->w;
  int windowY = spGetWindowSurface()->h;
  int viewPortX = (windowX >> 1);
  int viewPortY = (windowY >> 1);

  Sint32 tx1,ty1,tz1,tw1;
  Sint32 tx2,ty2,tz2,tw2;
  Sint32 tx3,ty3,tz3,tw3;
  spMulModellView(x1,y1,z1,&tx1,&ty1,&tz1,&tw1);
  spMulModellView(x2,y2,z2,&tx2,&ty2,&tz2,&tw2);
  spMulModellView(x3,y3,z3,&tx3,&ty3,&tz3,&tw3);

         x1 = fimul(spProjection[ 0],tx1);// + fimul(spProjection[ 8],tz1);
         y1 = fimul(spProjection[ 5],ty1);// + fimul(spProjection[ 9],tz1);
//         z1 = fimul(spProjection[10],tz1) + fimul(spProjection[14],tw1);
  Sint32 w1 = fimul(spProjection[11],tz1);
  if (w1 == 0)
    w1 = 1;  

  Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
  Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
//  Sint32 nz1 = fpdiv(z1,w1)>>SP_HALF_ACCURACY;

         x2 = fimul(spProjection[ 0],tx2);// + fimul(spProjection[ 8],tz2);
         y2 = fimul(spProjection[ 5],ty2);// + fimul(spProjection[ 9],tz2);
//         z2 = fimul(spProjection[10],tz2) + fimul(spProjection[14],tw2);
  Sint32 w2 = fimul(spProjection[11],tz2);
  if (w2 == 0)
    w2 = 1;
  Sint32 nx2 = fpdiv(x2,w2)>>SP_HALF_ACCURACY;
  Sint32 ny2 = fpdiv(y2,w2)>>SP_HALF_ACCURACY;
//  Sint32 nz2 = fpdiv(z2,w2)>>SP_HALF_ACCURACY;

         x3 = fimul(spProjection[ 0],tx3);// + fimul(spProjection[ 8],tz3);
         y3 = fimul(spProjection[ 5],ty3);// + fimul(spProjection[ 9],tz3);
//         z3 = fimul(spProjection[10],tz3) + fimul(spProjection[14],tw3);
  Sint32 w3 = fimul(spProjection[11],tz3);
  if (w3 == 0)
    w3 = 1;  
  Sint32 nx3 = fpdiv(x3,w3)>>SP_HALF_ACCURACY;
  Sint32 ny3 = fpdiv(y3,w3)>>SP_HALF_ACCURACY;
//  Sint32 nz3 = fpdiv(z3,w3)>>SP_HALF_ACCURACY;

  spTriangle(viewPortX+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             viewPortY-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz1,
             viewPortX+((nx2*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             viewPortY-((ny2*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz2,
             viewPortX+((nx3*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             viewPortY-((ny3*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz3,color);
  return 1;
}

PREFIX int spQuad3D(Sint32 x1,Sint32 y1,Sint32 z1,
  Sint32 x2,Sint32 y2,Sint32 z2,
  Sint32 x3,Sint32 y3,Sint32 z3,
  Sint32 x4,Sint32 y4,Sint32 z4,Uint16 color)
{
  int windowX = spGetWindowSurface()->w;
  int windowY = spGetWindowSurface()->h;
  int viewPortX = (windowX >> 1);
  int viewPortY = (windowY >> 1);

  Sint32 tx1,ty1,tz1,tw1;
  Sint32 tx2,ty2,tz2,tw2;
  Sint32 tx3,ty3,tz3,tw3;
  spMulModellView(x1,y1,z1,&tx1,&ty1,&tz1,&tw1);
  spMulModellView(x2,y2,z2,&tx2,&ty2,&tz2,&tw2);
  spMulModellView(x3,y3,z3,&tx3,&ty3,&tz3,&tw3);

  Sint32 tx4,ty4,tz4,tw4;
  spMulModellView(x4,y4,z4,&tx4,&ty4,&tz4,&tw4);
  
         x1 = fimul(spProjection[ 0],tx1);// + fimul(spProjection[ 8],tz1);
         y1 = fimul(spProjection[ 5],ty1);// + fimul(spProjection[ 9],tz1);
//         z1 = fimul(spProjection[10],tz1) + fimul(spProjection[14],tw1);
  Sint32 w1 = fimul(spProjection[11],tz1);
  if (w1 == 0)
    w1 = 1;  

  Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
  Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
//  Sint32 nz1 = fpdiv(z1,w1)>>SP_HALF_ACCURACY;

         x2 = fimul(spProjection[ 0],tx2);// + fimul(spProjection[ 8],tz2);
         y2 = fimul(spProjection[ 5],ty2);// + fimul(spProjection[ 9],tz2);
//         z2 = fimul(spProjection[10],tz2) + fimul(spProjection[14],tw2);
  Sint32 w2 = fimul(spProjection[11],tz2);
  if (w2 == 0)
    w2 = 1;
  Sint32 nx2 = fpdiv(x2,w2)>>SP_HALF_ACCURACY;
  Sint32 ny2 = fpdiv(y2,w2)>>SP_HALF_ACCURACY;
//  Sint32 nz2 = fpdiv(z2,w2)>>SP_HALF_ACCURACY;

         x3 = fimul(spProjection[ 0],tx3);// + fimul(spProjection[ 8],tz3);
         y3 = fimul(spProjection[ 5],ty3);// + fimul(spProjection[ 9],tz3);
//         z3 = fimul(spProjection[10],tz3) + fimul(spProjection[14],tw3);
  Sint32 w3 = fimul(spProjection[11],tz3);
  if (w3 == 0)
    w3 = 1;  
  Sint32 nx3 = fpdiv(x3,w3)>>SP_HALF_ACCURACY;
  Sint32 ny3 = fpdiv(y3,w3)>>SP_HALF_ACCURACY;
//  Sint32 nz3 = fpdiv(z3,w3)>>SP_HALF_ACCURACY;
  
         x4 = fimul(spProjection[ 0],tx4);// + fimul(spProjection[ 8],tz4);
         y4 = fimul(spProjection[ 5],ty4);// + fimul(spProjection[ 9],tz4);
//         z4 = fimul(spProjection[10],tz4) + fimul(spProjection[14],tw4);
  Sint32 w4 = fimul(spProjection[11],tz4);
  if (w4 == 0)
    w4 = 1;  
  Sint32 nx4 = fpdiv(x4,w4)>>SP_HALF_ACCURACY;
  Sint32 ny4 = fpdiv(y4,w4)>>SP_HALF_ACCURACY;
//  Sint32 nz4 = fpdiv(z4,w4)>>SP_HALF_ACCURACY;
  
  spQuad(viewPortX+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
         viewPortY-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz1,
         viewPortX+((nx2*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
         viewPortY-((ny2*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz2,
         viewPortX+((nx3*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
         viewPortY-((ny3*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz3,
         viewPortX+((nx4*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
         viewPortY-((ny4*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz4,color);
  return 1;
}

PREFIX int spTriangleTex3D(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 u1,Sint32 v1,
  Sint32 x2,Sint32 y2,Sint32 z2,Sint32 u2,Sint32 v2,
  Sint32 x3,Sint32 y3,Sint32 z3,Sint32 u3,Sint32 v3,Uint16 color)
{
  int windowX = spGetWindowSurface()->w;
  int windowY = spGetWindowSurface()->h;
  int viewPortX = (windowX >> 1);
  int viewPortY = (windowY >> 1);

  Sint32 tx1,ty1,tz1,tw1;
  Sint32 tx2,ty2,tz2,tw2;
  Sint32 tx3,ty3,tz3,tw3;
  spMulModellView(x1,y1,z1,&tx1,&ty1,&tz1,&tw1);
  spMulModellView(x2,y2,z2,&tx2,&ty2,&tz2,&tw2);
  spMulModellView(x3,y3,z3,&tx3,&ty3,&tz3,&tw3);

         x1 = fimul(spProjection[ 0],tx1);// + fimul(spProjection[ 8],tz1);
         y1 = fimul(spProjection[ 5],ty1);// + fimul(spProjection[ 9],tz1);
//         z1 = fimul(spProjection[10],tz1) + fimul(spProjection[14],tw1);
  Sint32 w1 = fimul(spProjection[11],tz1);
  if (w1 == 0)
    w1 = 1;  

  Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
  Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
//  Sint32 nz1 = fpdiv(z1,w1)>>SP_HALF_ACCURACY;

         x2 = fimul(spProjection[ 0],tx2);// + fimul(spProjection[ 8],tz2);
         y2 = fimul(spProjection[ 5],ty2);// + fimul(spProjection[ 9],tz2);
//         z2 = fimul(spProjection[10],tz2) + fimul(spProjection[14],tw2);
  Sint32 w2 = fimul(spProjection[11],tz2);
  if (w2 == 0)
    w2 = 1;
  Sint32 nx2 = fpdiv(x2,w2)>>SP_HALF_ACCURACY;
  Sint32 ny2 = fpdiv(y2,w2)>>SP_HALF_ACCURACY;
//  Sint32 nz2 = fpdiv(z2,w2)>>SP_HALF_ACCURACY;

         x3 = fimul(spProjection[ 0],tx3);// + fimul(spProjection[ 8],tz3);
         y3 = fimul(spProjection[ 5],ty3);// + fimul(spProjection[ 9],tz3);
//         z3 = fimul(spProjection[10],tz3) + fimul(spProjection[14],tw3);
  Sint32 w3 = fimul(spProjection[11],tz3);
  if (w3 == 0)
    w3 = 1;  
  Sint32 nx3 = fpdiv(x3,w3)>>SP_HALF_ACCURACY;
  Sint32 ny3 = fpdiv(y3,w3)>>SP_HALF_ACCURACY;
//  Sint32 nz3 = fpdiv(z3,w3)>>SP_HALF_ACCURACY;
  
  spTriangle_tex(viewPortX+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
                 viewPortY-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz1,u1,v1,
                 viewPortX+((nx2*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
                 viewPortY-((ny2*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz2,u2,v2,
                 viewPortX+((nx3*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
                 viewPortY-((ny3*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz3,u3,v3,color);
  return 1;
}


PREFIX int spQuadTex3D(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 u1,Sint32 v1,
  Sint32 x2,Sint32 y2,Sint32 z2,Sint32 u2,Sint32 v2,
  Sint32 x3,Sint32 y3,Sint32 z3,Sint32 u3,Sint32 v3,
  Sint32 x4,Sint32 y4,Sint32 z4,Sint32 u4,Sint32 v4,Uint16 color)
{
  int windowX = spGetWindowSurface()->w;
  int windowY = spGetWindowSurface()->h;
  int viewPortX = (windowX >> 1);
  int viewPortY = (windowY >> 1);

  Sint32 tx1,ty1,tz1,tw1;
  Sint32 tx2,ty2,tz2,tw2;
  Sint32 tx3,ty3,tz3,tw3;
  spMulModellView(x1,y1,z1,&tx1,&ty1,&tz1,&tw1);
  spMulModellView(x2,y2,z2,&tx2,&ty2,&tz2,&tw2);
  spMulModellView(x3,y3,z3,&tx3,&ty3,&tz3,&tw3);

  
  Sint32 tx4,ty4,tz4,tw4;
  spMulModellView(x4,y4,z4,&tx4,&ty4,&tz4,&tw4);
  
         x1 = fimul(spProjection[ 0],tx1);// + fimul(spProjection[ 8],tz1);
         y1 = fimul(spProjection[ 5],ty1);// + fimul(spProjection[ 9],tz1);
//         z1 = fimul(spProjection[10],tz1) + fimul(spProjection[14],tw1);
  Sint32 w1 = fimul(spProjection[11],tz1);
  if (w1 == 0)
    w1 = 1;  

  Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
  Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
//  Sint32 nz1 = fpdiv(z1,w1)>>SP_HALF_ACCURACY;

         x2 = fimul(spProjection[ 0],tx2);// + fimul(spProjection[ 8],tz2);
         y2 = fimul(spProjection[ 5],ty2);// + fimul(spProjection[ 9],tz2);
//         z2 = fimul(spProjection[10],tz2) + fimul(spProjection[14],tw2);
  Sint32 w2 = fimul(spProjection[11],tz2);
  if (w2 == 0)
    w2 = 1;
  Sint32 nx2 = fpdiv(x2,w2)>>SP_HALF_ACCURACY;
  Sint32 ny2 = fpdiv(y2,w2)>>SP_HALF_ACCURACY;
//  Sint32 nz2 = fpdiv(z2,w2)>>SP_HALF_ACCURACY;

         x3 = fimul(spProjection[ 0],tx3);// + fimul(spProjection[ 8],tz3);
         y3 = fimul(spProjection[ 5],ty3);// + fimul(spProjection[ 9],tz3);
//         z3 = fimul(spProjection[10],tz3) + fimul(spProjection[14],tw3);
  Sint32 w3 = fimul(spProjection[11],tz3);
  if (w3 == 0)
    w3 = 1;  
  Sint32 nx3 = fpdiv(x3,w3)>>SP_HALF_ACCURACY;
  Sint32 ny3 = fpdiv(y3,w3)>>SP_HALF_ACCURACY;
//  Sint32 nz3 = fpdiv(z3,w3)>>SP_HALF_ACCURACY;
  
         x4 = fimul(spProjection[ 0],tx4);// + fimul(spProjection[ 8],tz4);
         y4 = fimul(spProjection[ 5],ty4);// + fimul(spProjection[ 9],tz4);
//         z4 = fimul(spProjection[10],tz4) + fimul(spProjection[14],tw4);
  Sint32 w4 = fimul(spProjection[11],tz4);
  if (w4 == 0)
    w4 = 1;  
  Sint32 nx4 = fpdiv(x4,w4)>>SP_HALF_ACCURACY;
  Sint32 ny4 = fpdiv(y4,w4)>>SP_HALF_ACCURACY;
//  Sint32 nz4 = fpdiv(z4,w4)>>SP_HALF_ACCURACY;
  
  spQuad_tex(viewPortX+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             viewPortY-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz1,u1,v1,
             viewPortX+((nx2*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             viewPortY-((ny2*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz2,u2,v2,
             viewPortX+((nx3*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             viewPortY-((ny3*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz3,u3,v3,
             viewPortX+((nx4*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             viewPortY-((ny4*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz4,u4,v4,color);
  return 1;
}

PREFIX void spBlit3D(Sint32 x1,Sint32 y1,Sint32 z1,SDL_Surface* surface)
{
  int windowX = spGetWindowSurface()->w;
  int windowY = spGetWindowSurface()->h;
  int viewPortX = (windowX >> 1);
  int viewPortY = (windowY >> 1);
  Sint32 tx1,ty1,tz1,tw1;
  spMulModellView(x1,y1,z1,&tx1,&ty1,&tz1,&tw1);

         x1 = fimul(spProjection[ 0],tx1);// + fimul(spProjection[ 8],tz1);
         y1 = fimul(spProjection[ 5],ty1);// + fimul(spProjection[ 9],tz1);
//         z1 = fimul(spProjection[10],tz1) + fimul(spProjection[14],tw1);
  Sint32 w1 = fimul(spProjection[11],tz1);
  if (w1 == 0)
    w1 = 1;  

  Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
  Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
//  Sint32 nz1 = fpdiv(z1,w1)>>SP_HALF_ACCURACY;

  spBlitSurface(viewPortX+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
                viewPortY-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
                tz1,
                surface);
  
}

PREFIX int spMesh3D(spModelPointer mesh,int updateEdgeList)
{
  int count = 0;
  Sint32 windowX = spGetWindowSurface()->w;
  Sint32 windowY = spGetWindowSurface()->h;
  Sint32 viewPortX = (windowX >> 1);
  Sint32 viewPortY = (windowY >> 1);
  //Project Points
  int i;
  for (i = 0; i < mesh->pointCount; i++)
  {
    Sint32 tx1,ty1,tz1,tw1;
    spMulModellView(mesh->point[i].x,mesh->point[i].y,mesh->point[i].z,&tx1,&ty1,&tz1,&tw1);
    Sint32 x1 = fimul(spProjection[ 0],tx1);
    Sint32 y1 = fimul(spProjection[ 5],ty1);
    Sint32 w1 = fimul(spProjection[11],tz1);
    if (w1 == 0)
      w1 = 1;  
    Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
    Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
    mesh->point[i].px = viewPortX+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY);
    mesh->point[i].py = viewPortY-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY);
    mesh->point[i].pz = tz1;    
  }
  for (i = 0; i < mesh->texPointCount; i++)
  {
    Sint32 tx1,ty1,tz1,tw1;
    spMulModellView(mesh->texPoint[i].x,mesh->texPoint[i].y,mesh->texPoint[i].z,&tx1,&ty1,&tz1,&tw1);
    Sint32 x1 = fimul(spProjection[ 0],tx1);
    Sint32 y1 = fimul(spProjection[ 5],ty1);
    Sint32 w1 = fimul(spProjection[11],tz1);
    if (w1 == 0)
      w1 = 1;  
    Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
    Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
    mesh->texPoint[i].px = viewPortX+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY);
    mesh->texPoint[i].py = viewPortY-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY);
    mesh->texPoint[i].pz = tz1;    
  }
  //Draw Faces, if seeable
  for (i = 0; i < mesh->triangleCount; i++)
  {
    count+= mesh->triangle[i].was_drawn = 
    spTriangle(mesh->point[mesh->triangle[i].point[0]].px,
               mesh->point[mesh->triangle[i].point[0]].py,
               mesh->point[mesh->triangle[i].point[0]].pz,
               mesh->point[mesh->triangle[i].point[1]].px,
               mesh->point[mesh->triangle[i].point[1]].py,
               mesh->point[mesh->triangle[i].point[1]].pz,
               mesh->point[mesh->triangle[i].point[2]].px,
               mesh->point[mesh->triangle[i].point[2]].py,
               mesh->point[mesh->triangle[i].point[2]].pz,mesh->color);
    
  }
  for (i = 0; i < mesh->quadCount; i++)
  {
    count+= mesh->quad[i].was_drawn = 
    spQuad(mesh->point[mesh->quad[i].point[0]].px,
           mesh->point[mesh->quad[i].point[0]].py,
           mesh->point[mesh->quad[i].point[0]].pz,
           mesh->point[mesh->quad[i].point[1]].px,
           mesh->point[mesh->quad[i].point[1]].py,
           mesh->point[mesh->quad[i].point[1]].pz,
           mesh->point[mesh->quad[i].point[2]].px,
           mesh->point[mesh->quad[i].point[2]].py,
           mesh->point[mesh->quad[i].point[2]].pz,
           mesh->point[mesh->quad[i].point[3]].px,
           mesh->point[mesh->quad[i].point[3]].py,
           mesh->point[mesh->quad[i].point[3]].pz,mesh->color);    
  }
  if (mesh->texQuadCount + mesh->texTriangleCount > 0)
    spBindTexture(mesh->texture);
  for (i = 0; i < mesh->texTriangleCount; i++)
  {
    count+= mesh->texTriangle[i].was_drawn =
    spTriangle_tex(mesh->texPoint[mesh->texTriangle[i].point[0]].px,
                   mesh->texPoint[mesh->texTriangle[i].point[0]].py,
                   mesh->texPoint[mesh->texTriangle[i].point[0]].pz,
                   mesh->texPoint[mesh->texTriangle[i].point[0]].u,
                   mesh->texPoint[mesh->texTriangle[i].point[0]].v,
                   mesh->texPoint[mesh->texTriangle[i].point[1]].px,
                   mesh->texPoint[mesh->texTriangle[i].point[1]].py,
                   mesh->texPoint[mesh->texTriangle[i].point[1]].pz,
                   mesh->texPoint[mesh->texTriangle[i].point[1]].u,
                   mesh->texPoint[mesh->texTriangle[i].point[1]].v,
                   mesh->texPoint[mesh->texTriangle[i].point[2]].px,
                   mesh->texPoint[mesh->texTriangle[i].point[2]].py,
                   mesh->texPoint[mesh->texTriangle[i].point[2]].pz,
                   mesh->texPoint[mesh->texTriangle[i].point[2]].u,
                   mesh->texPoint[mesh->texTriangle[i].point[2]].v,mesh->color);
  }
  for (i = 0; i < mesh->texQuadCount; i++)
  {
    count+= mesh->texQuad[i].was_drawn =
    spQuad_tex(mesh->texPoint[mesh->texQuad[i].point[0]].px,
               mesh->texPoint[mesh->texQuad[i].point[0]].py,
               mesh->texPoint[mesh->texQuad[i].point[0]].pz,
               mesh->texPoint[mesh->texQuad[i].point[0]].u,
               mesh->texPoint[mesh->texQuad[i].point[0]].v,
               mesh->texPoint[mesh->texQuad[i].point[1]].px,
               mesh->texPoint[mesh->texQuad[i].point[1]].py,
               mesh->texPoint[mesh->texQuad[i].point[1]].pz,
               mesh->texPoint[mesh->texQuad[i].point[1]].u,
               mesh->texPoint[mesh->texQuad[i].point[1]].v,
               mesh->texPoint[mesh->texQuad[i].point[2]].px,
               mesh->texPoint[mesh->texQuad[i].point[2]].py,
               mesh->texPoint[mesh->texQuad[i].point[2]].pz,
               mesh->texPoint[mesh->texQuad[i].point[2]].u,
               mesh->texPoint[mesh->texQuad[i].point[2]].v,
               mesh->texPoint[mesh->texQuad[i].point[3]].px,
               mesh->texPoint[mesh->texQuad[i].point[3]].py,
               mesh->texPoint[mesh->texQuad[i].point[3]].pz,
               mesh->texPoint[mesh->texQuad[i].point[3]].u,
               mesh->texPoint[mesh->texQuad[i].point[3]].v,mesh->color);
  }
  return count;
}
