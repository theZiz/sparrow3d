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
#include <stdio.h>

Sint32 spModelView[16];
Sint32 spProjection[16];

inline Sint32 fpdiv(Sint32 a,Sint32 b)
{
  if (b>=0 && b<(1<<SP_PRIM_ACCURACY))
    return a*spGetOne_over_x_pointer()[b]>>SP_PRIM_ACCURACY-SP_ACCURACY;
  if (b <0 && b>(-1<<SP_PRIM_ACCURACY))
    return -a*spGetOne_over_x_pointer()[-b]>>SP_PRIM_ACCURACY-SP_ACCURACY;
  return ((a<<SP_HALF_ACCURACY)/b)<<SP_HALF_ACCURACY;
}

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

inline Sint32 fimul(Sint32 a,Sint32 b)
{
  return (a>>SP_HALF_ACCURACY)*(b>>SP_HALF_ACCURACY);
}

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

  /*Sint32 result[16];
  
  result[ 0] = fimul(spModelView[ 0],rotate[ 0]) + fimul(spModelView[ 4],rotate[ 1]) + fimul(spModelView[ 8],rotate[ 2]) + fimul(spModelView[12],rotate[ 3]);
  result[ 1] = fimul(spModelView[ 1],rotate[ 0]) + fimul(spModelView[ 5],rotate[ 1]) + fimul(spModelView[ 9],rotate[ 2]) + fimul(spModelView[13],rotate[ 3]);
  result[ 2] = fimul(spModelView[ 2],rotate[ 0]) + fimul(spModelView[ 6],rotate[ 1]) + fimul(spModelView[10],rotate[ 2]) + fimul(spModelView[14],rotate[ 3]);
  result[ 3] = fimul(spModelView[ 3],rotate[ 0]) + fimul(spModelView[ 7],rotate[ 1]) + fimul(spModelView[11],rotate[ 2]) + fimul(spModelView[15],rotate[ 3]);

  result[ 4] = fimul(spModelView[ 0],rotate[ 4]) + fimul(spModelView[ 4],rotate[ 5]) + fimul(spModelView[ 8],rotate[ 6]) + fimul(spModelView[12],rotate[ 7]);
  result[ 5] = fimul(spModelView[ 1],rotate[ 4]) + fimul(spModelView[ 5],rotate[ 5]) + fimul(spModelView[ 9],rotate[ 6]) + fimul(spModelView[13],rotate[ 7]);
  result[ 6] = fimul(spModelView[ 2],rotate[ 4]) + fimul(spModelView[ 6],rotate[ 5]) + fimul(spModelView[10],rotate[ 6]) + fimul(spModelView[14],rotate[ 7]);
  result[ 7] = fimul(spModelView[ 3],rotate[ 4]) + fimul(spModelView[ 7],rotate[ 5]) + fimul(spModelView[11],rotate[ 6]) + fimul(spModelView[15],rotate[ 7]);

  result[ 8] = fimul(spModelView[ 0],rotate[ 8]) + fimul(spModelView[ 4],rotate[ 9]) + fimul(spModelView[ 8],rotate[10]) + fimul(spModelView[12],rotate[11]);
  result[ 9] = fimul(spModelView[ 1],rotate[ 8]) + fimul(spModelView[ 5],rotate[ 9]) + fimul(spModelView[ 9],rotate[10]) + fimul(spModelView[13],rotate[11]);
  result[10] = fimul(spModelView[ 2],rotate[ 8]) + fimul(spModelView[ 6],rotate[ 9]) + fimul(spModelView[10],rotate[10]) + fimul(spModelView[14],rotate[11]);
  result[11] = fimul(spModelView[ 3],rotate[ 8]) + fimul(spModelView[ 7],rotate[ 9]) + fimul(spModelView[11],rotate[10]) + fimul(spModelView[15],rotate[11]);

  result[12] = fimul(spModelView[ 0],rotate[12]) + fimul(spModelView[ 4],rotate[13]) + fimul(spModelView[ 8],rotate[14]) + fimul(spModelView[12],rotate[15]);
  result[13] = fimul(spModelView[ 1],rotate[12]) + fimul(spModelView[ 5],rotate[13]) + fimul(spModelView[ 9],rotate[14]) + fimul(spModelView[13],rotate[15]);
  result[14] = fimul(spModelView[ 2],rotate[12]) + fimul(spModelView[ 6],rotate[13]) + fimul(spModelView[10],rotate[14]) + fimul(spModelView[14],rotate[15]);
  result[15] = fimul(spModelView[ 3],rotate[12]) + fimul(spModelView[ 7],rotate[13]) + fimul(spModelView[11],rotate[14]) + fimul(spModelView[15],rotate[15]);

  memcpy(spModelView,result,sizeof(Sint32)*16);*/
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

PREFIX void spTriangle3D(Sint32 x1,Sint32 y1,Sint32 z1,
  Sint32 x2,Sint32 y2,Sint32 z2,
  Sint32 x3,Sint32 y3,Sint32 z3,Uint16 color)
{
  int windowX=spGetWindowSurface()->w;
  int windowY=spGetWindowSurface()->h;

  Sint32 tx1,ty1,tz1,tw1;
  Sint32 tx2,ty2,tz2,tw2;
  Sint32 tx3,ty3,tz3,tw3;
  spMulModellView(x1,y1,z1,&tx1,&ty1,&tz1,&tw1);
  spMulModellView(x2,y2,z2,&tx2,&ty2,&tz2,&tw2);
  spMulModellView(x3,y3,z3,&tx3,&ty3,&tz3,&tw3);

  Sint32 normal[3];
  spCalcNormal(tx1,ty1,tz1,tx2,ty2,tz2,tx3,ty3,tz3,normal);

         x1 = fimul(spProjection[ 0],tx1) + fimul(spProjection[ 8],tz1);
         y1 = fimul(spProjection[ 5],ty1) + fimul(spProjection[ 9],tz1);
//         z1 = fimul(spProjection[10],tz1) + fimul(spProjection[14],tw1);
  Sint32 w1 = fimul(spProjection[11],tz1);
  if (w1 == 0)
    w1 = 1;  

  Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
  Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
//  Sint32 nz1 = fpdiv(z1,w1)>>SP_HALF_ACCURACY;

         x2 = fimul(spProjection[ 0],tx2) + fimul(spProjection[ 8],tz2);
         y2 = fimul(spProjection[ 5],ty2) + fimul(spProjection[ 9],tz2);
//         z2 = fimul(spProjection[10],tz2) + fimul(spProjection[14],tw2);
  Sint32 w2 = fimul(spProjection[11],tz2);
  if (w2 == 0)
    w2 = 1;
  Sint32 nx2 = fpdiv(x2,w2)>>SP_HALF_ACCURACY;
  Sint32 ny2 = fpdiv(y2,w2)>>SP_HALF_ACCURACY;
//  Sint32 nz2 = fpdiv(z2,w2)>>SP_HALF_ACCURACY;

         x3 = fimul(spProjection[ 0],tx3) + fimul(spProjection[ 8],tz3);
         y3 = fimul(spProjection[ 5],ty3) + fimul(spProjection[ 9],tz3);
//         z3 = fimul(spProjection[10],tz3) + fimul(spProjection[14],tw3);
  Sint32 w3 = fimul(spProjection[11],tz3);
  if (w3 == 0)
    w3 = 1;  
  Sint32 nx3 = fpdiv(x3,w3)>>SP_HALF_ACCURACY;
  Sint32 ny3 = fpdiv(y3,w3)>>SP_HALF_ACCURACY;
//  Sint32 nz3 = fpdiv(z3,w3)>>SP_HALF_ACCURACY;
  
  /*spCalcNormal(x1,y1,z1,x2,y2,z2,x3,y3,z3,normal);
  if (normal[2]<0)
    return;  */

  spTriangle((windowX >> 1)+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             (windowY >> 1)-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz1,
             (windowX >> 1)+((nx2*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             (windowY >> 1)-((ny2*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz2,
             (windowX >> 1)+((nx3*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             (windowY >> 1)-((ny3*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz3,color);
}

PREFIX void spQuad3D(Sint32 x1,Sint32 y1,Sint32 z1,
  Sint32 x2,Sint32 y2,Sint32 z2,
  Sint32 x3,Sint32 y3,Sint32 z3,
  Sint32 x4,Sint32 y4,Sint32 z4,Uint16 color)
{
  int windowX=spGetWindowSurface()->w;
  int windowY=spGetWindowSurface()->h;

  Sint32 tx1,ty1,tz1,tw1;
  Sint32 tx2,ty2,tz2,tw2;
  Sint32 tx3,ty3,tz3,tw3;
  Sint32 tx4,ty4,tz4,tw4;
  spMulModellView(x1,y1,z1,&tx1,&ty1,&tz1,&tw1);
  spMulModellView(x2,y2,z2,&tx2,&ty2,&tz2,&tw2);
  spMulModellView(x3,y3,z3,&tx3,&ty3,&tz3,&tw3);
  spMulModellView(x4,y4,z4,&tx4,&ty4,&tz4,&tw4);

  
  Sint32 normal[3];
  spCalcNormal(tx1,ty1,tz1,tx2,ty2,tz2,tx3,ty3,tz3,normal);

         x1 = fimul(spProjection[ 0],tx1) + fimul(spProjection[ 8],tz1);
         y1 = fimul(spProjection[ 5],ty1) + fimul(spProjection[ 9],tz1);
//         z1 = fimul(spProjection[10],tz1) + fimul(spProjection[14],tw1);
  Sint32 w1 = fimul(spProjection[11],tz1);
  if (w1 == 0)
    w1 = 1;  

  Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
  Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
//  Sint32 nz1 = fpdiv(z1,w1)>>SP_HALF_ACCURACY;

         x2 = fimul(spProjection[ 0],tx2) + fimul(spProjection[ 8],tz2);
         y2 = fimul(spProjection[ 5],ty2) + fimul(spProjection[ 9],tz2);
//         z2 = fimul(spProjection[10],tz2) + fimul(spProjection[14],tw2);
  Sint32 w2 = fimul(spProjection[11],tz2);
  if (w2 == 0)
    w2 = 1;
  Sint32 nx2 = fpdiv(x2,w2)>>SP_HALF_ACCURACY;
  Sint32 ny2 = fpdiv(y2,w2)>>SP_HALF_ACCURACY;
//  Sint32 nz2 = fpdiv(z2,w2)>>SP_HALF_ACCURACY;

         x3 = fimul(spProjection[ 0],tx3) + fimul(spProjection[ 8],tz3);
         y3 = fimul(spProjection[ 5],ty3) + fimul(spProjection[ 9],tz3);
//         z3 = fimul(spProjection[10],tz3) + fimul(spProjection[14],tw3);
  Sint32 w3 = fimul(spProjection[11],tz3);
  if (w3 == 0)
    w3 = 1;  
  Sint32 nx3 = fpdiv(x3,w3)>>SP_HALF_ACCURACY;
  Sint32 ny3 = fpdiv(y3,w3)>>SP_HALF_ACCURACY;
//  Sint32 nz3 = fpdiv(z3,w3)>>SP_HALF_ACCURACY;
  
  spCalcNormal(fpdiv(x1,w1),fpdiv(y1,w1),0,fpdiv(x2,w2),fpdiv(y2,w2),0,fpdiv(x3,w3),fpdiv(y3,w3),0,normal);
  if (normal[2]<0)
    return;

         x4 = fimul(spProjection[ 0],tx4) + fimul(spProjection[ 8],tz4);
         y4 = fimul(spProjection[ 5],ty4) + fimul(spProjection[ 9],tz4);
//         z4 = fimul(spProjection[10],tz4) + fimul(spProjection[14],tw4);
  Sint32 w4 = fimul(spProjection[11],tz4);
  if (w4 == 0)
    w4 = 1;  
  Sint32 nx4 = fpdiv(x4,w4)>>SP_HALF_ACCURACY;
  Sint32 ny4 = fpdiv(y4,w4)>>SP_HALF_ACCURACY;
//  Sint32 nz4 = fpdiv(z4,w4)>>SP_HALF_ACCURACY;
  
  spQuad((windowX >> 1)+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
         (windowY >> 1)-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz1,
         (windowX >> 1)+((nx2*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
         (windowY >> 1)-((ny2*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz2,
         (windowX >> 1)+((nx3*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
         (windowY >> 1)-((ny3*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz3,
         (windowX >> 1)+((nx4*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
         (windowY >> 1)-((ny4*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz4,color);

}

PREFIX void spTriangleTex3D(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 u1,Sint32 v1,
  Sint32 x2,Sint32 y2,Sint32 z2,Sint32 u2,Sint32 v2,
  Sint32 x3,Sint32 y3,Sint32 z3,Sint32 u3,Sint32 v3,Uint16 color)
{
  int windowX=spGetWindowSurface()->w;
  int windowY=spGetWindowSurface()->h;

  Sint32 tx1,ty1,tz1,tw1;
  Sint32 tx2,ty2,tz2,tw2;
  Sint32 tx3,ty3,tz3,tw3;
  spMulModellView(x1,y1,z1,&tx1,&ty1,&tz1,&tw1);
  spMulModellView(x2,y2,z2,&tx2,&ty2,&tz2,&tw2);
  spMulModellView(x3,y3,z3,&tx3,&ty3,&tz3,&tw3);

  Sint32 normal[3];
  spCalcNormal(tx1,ty1,tz1,tx2,ty2,tz2,tx3,ty3,tz3,normal);

         x1 = fimul(spProjection[ 0],tx1) + fimul(spProjection[ 8],tz1);
         y1 = fimul(spProjection[ 5],ty1) + fimul(spProjection[ 9],tz1);
//         z1 = fimul(spProjection[10],tz1) + fimul(spProjection[14],tw1);
  Sint32 w1 = fimul(spProjection[11],tz1);
  if (w1 == 0)
    w1 = 1;  

  Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
  Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
//  Sint32 nz1 = fpdiv(z1,w1)>>SP_HALF_ACCURACY;

         x2 = fimul(spProjection[ 0],tx2) + fimul(spProjection[ 8],tz2);
         y2 = fimul(spProjection[ 5],ty2) + fimul(spProjection[ 9],tz2);
//         z2 = fimul(spProjection[10],tz2) + fimul(spProjection[14],tw2);
  Sint32 w2 = fimul(spProjection[11],tz2);
  if (w2 == 0)
    w2 = 1;
  Sint32 nx2 = fpdiv(x2,w2)>>SP_HALF_ACCURACY;
  Sint32 ny2 = fpdiv(y2,w2)>>SP_HALF_ACCURACY;
//  Sint32 nz2 = fpdiv(z2,w2)>>SP_HALF_ACCURACY;

         x3 = fimul(spProjection[ 0],tx3) + fimul(spProjection[ 8],tz3);
         y3 = fimul(spProjection[ 5],ty3) + fimul(spProjection[ 9],tz3);
//         z3 = fimul(spProjection[10],tz3) + fimul(spProjection[14],tw3);
  Sint32 w3 = fimul(spProjection[11],tz3);
  if (w3 == 0)
    w3 = 1;  
  Sint32 nx3 = fpdiv(x3,w3)>>SP_HALF_ACCURACY;
  Sint32 ny3 = fpdiv(y3,w3)>>SP_HALF_ACCURACY;
//  Sint32 nz3 = fpdiv(z3,w3)>>SP_HALF_ACCURACY;

  /*spCalcNormal(x1,y1,z1,x2,y2,z2,x3,y3,z3,normal);
  if (normal[2]<0)
    return;*/
  
  spTriangle_tex((windowX >> 1)+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
                 (windowY >> 1)-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz1,u1,v1,
                 (windowX >> 1)+((nx2*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
                 (windowY >> 1)-((ny2*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz2,u2,v2,
                 (windowX >> 1)+((nx3*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
                 (windowY >> 1)-((ny3*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz3,u3,v3,color);
}


PREFIX void spQuadTex3D(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 u1,Sint32 v1,
  Sint32 x2,Sint32 y2,Sint32 z2,Sint32 u2,Sint32 v2,
  Sint32 x3,Sint32 y3,Sint32 z3,Sint32 u3,Sint32 v3,
  Sint32 x4,Sint32 y4,Sint32 z4,Sint32 u4,Sint32 v4,Uint16 color)
{
  int windowX=spGetWindowSurface()->w;
  int windowY=spGetWindowSurface()->h;

  Sint32 tx1,ty1,tz1,tw1;
  Sint32 tx2,ty2,tz2,tw2;
  Sint32 tx3,ty3,tz3,tw3;
  spMulModellView(x1,y1,z1,&tx1,&ty1,&tz1,&tw1);
  spMulModellView(x2,y2,z2,&tx2,&ty2,&tz2,&tw2);
  spMulModellView(x3,y3,z3,&tx3,&ty3,&tz3,&tw3);

  Sint32 normal[3];
  spCalcNormal(tx1,ty1,tz1,tx2,ty2,tz2,tx3,ty3,tz3,normal);

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

  spCalcNormal(fpdiv(x1,w1),fpdiv(-y1,w1),0,fpdiv(x2,w2),fpdiv(-y2,w2),0,fpdiv(x3,w3),fpdiv(-y3,w3),0,normal);
  if (normal[2]<0)
    return;
  
         x4 = fimul(spProjection[ 0],tx4);// + fimul(spProjection[ 8],tz4);
         y4 = fimul(spProjection[ 5],ty4);// + fimul(spProjection[ 9],tz4);
//         z4 = fimul(spProjection[10],tz4) + fimul(spProjection[14],tw4);
  Sint32 w4 = fimul(spProjection[11],tz4);
  if (w4 == 0)
    w4 = 1;  
  Sint32 nx4 = fpdiv(x4,w4)>>SP_HALF_ACCURACY;
  Sint32 ny4 = fpdiv(y4,w4)>>SP_HALF_ACCURACY;
//  Sint32 nz4 = fpdiv(z4,w4)>>SP_HALF_ACCURACY;
  
  spQuad_tex((windowX >> 1)+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             (windowY >> 1)-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz1,u1,v1,
             (windowX >> 1)+((nx2*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             (windowY >> 1)-((ny2*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz2,u2,v2,
             (windowX >> 1)+((nx3*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             (windowY >> 1)-((ny3*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz3,u3,v3,
             (windowX >> 1)+((nx4*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             (windowY >> 1)-((ny4*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz4,u4,v4,color);
}
