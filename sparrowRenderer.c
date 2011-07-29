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
  rotate[ 0]= c+(((x>>SP_HALF_ACCURACY)*(x>>SP_HALF_ACCURACY))>>SP_HALF_ACCURACY)*(((1<<SP_ACCURACY)-c)>>SP_HALF_ACCURACY);
  rotate[ 4]=   (((x>>SP_HALF_ACCURACY)*(y>>SP_HALF_ACCURACY))>>SP_HALF_ACCURACY)*(((1<<SP_ACCURACY)-c)>>SP_HALF_ACCURACY)-(z>>SP_HALF_ACCURACY)*(s>>SP_HALF_ACCURACY);
  rotate[ 8]=   (((x>>SP_HALF_ACCURACY)*(z>>SP_HALF_ACCURACY))>>SP_HALF_ACCURACY)*(((1<<SP_ACCURACY)-c)>>SP_HALF_ACCURACY)+(y>>SP_HALF_ACCURACY)*(s>>SP_HALF_ACCURACY);
  //rotate[12]= 0<<ACCURACY;
  rotate[ 1]=   (((y>>SP_HALF_ACCURACY)*(x>>SP_HALF_ACCURACY))>>SP_HALF_ACCURACY)*(((1<<SP_ACCURACY)-c)>>SP_HALF_ACCURACY)+(z>>SP_HALF_ACCURACY)*(s>>SP_HALF_ACCURACY);
  rotate[ 5]= c+(((y>>SP_HALF_ACCURACY)*(y>>SP_HALF_ACCURACY))>>SP_HALF_ACCURACY)*(((1<<SP_ACCURACY)-c)>>SP_HALF_ACCURACY);
  rotate[ 9]=   (((y>>SP_HALF_ACCURACY)*(z>>SP_HALF_ACCURACY))>>SP_HALF_ACCURACY)*(((1<<SP_ACCURACY)-c)>>SP_HALF_ACCURACY)-(x>>SP_HALF_ACCURACY)*(s>>SP_HALF_ACCURACY);
  //rotate[13]= 0<<ACCURACY;
  rotate[ 2]=   (((z>>SP_HALF_ACCURACY)*(x>>SP_HALF_ACCURACY))>>SP_HALF_ACCURACY)*(((1<<SP_ACCURACY)-c)>>SP_HALF_ACCURACY)-(y>>SP_HALF_ACCURACY)*(s>>SP_HALF_ACCURACY);
  rotate[ 6]=   (((z>>SP_HALF_ACCURACY)*(y>>SP_HALF_ACCURACY))>>SP_HALF_ACCURACY)*(((1<<SP_ACCURACY)-c)>>SP_HALF_ACCURACY)+(x>>SP_HALF_ACCURACY)*(s>>SP_HALF_ACCURACY);
  rotate[10]= c+(((z>>SP_HALF_ACCURACY)*(z>>SP_HALF_ACCURACY))>>SP_HALF_ACCURACY)*(((1<<SP_ACCURACY)-c)>>SP_HALF_ACCURACY);
  //rotate[14]= 0<<ACCURACY;
  //rotate[ 3]= 0<<ACCURACY;
  //rotate[ 7]= 0<<ACCURACY;
  //rotate[11]= 0<<ACCURACY;
  //rotate[15]= 1<<ACCURACY;

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

inline void calcNormal(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 x2,Sint32 y2,Sint32 z2,
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
  
  //Normale berechnen
  Sint32 normal[3];
  calcNormal(tx1,ty1,tz1,tx2,ty2,tz2,tx3,ty3,tz3,normal);
  //Sint32 l=(Sint32)(sqrt((float)normal[0]*(float)normal[0]+(float)normal[1]*(float)normal[1]+(float)normal[2]*(float)normal[2]));
  Sint32 l = spSqrt((normal[0]>>SP_HALF_ACCURACY)*(normal[0]>>SP_HALF_ACCURACY)+
                    (normal[1]>>SP_HALF_ACCURACY)*(normal[1]>>SP_HALF_ACCURACY)+
                    (normal[2]>>SP_HALF_ACCURACY)*(normal[2]>>SP_HALF_ACCURACY));
	if (l==0)
	  return;
  //Vector von Betrachter zu Fläche
  Sint32 ox = fpdiv(tx1+tx2+tx3,3<<SP_ACCURACY); //TODO
  Sint32 oy = fpdiv(ty1+ty2+ty3,3<<SP_ACCURACY); //TODO
  Sint32 oz = fpdiv(tz1+tz2+tz3,3<<SP_ACCURACY); //TODO
  //Sint32 ol = (Sint32)(sqrt((float)ox*(float)ox+(float)oy*(float)oy+(float)oz*(float)oz));
  /*Sint32 ol = spSqrt((ox>>SP_HALF_ACCURACY)*(ox>>SP_HALF_ACCURACY)+
                     (oy>>SP_HALF_ACCURACY)*(oy>>SP_HALF_ACCURACY)+
                     (oz>>SP_HALF_ACCURACY)*(oz>>SP_HALF_ACCURACY));
  if (ol==0)
    return;
  
  ox=fpdiv(ox,ol);
  oy=fpdiv(oy,ol);
  oz=fpdiv(oz,ol);*/
  if (l==0)
    return;
  normal[0]=fpdiv(normal[0],l); //TODO
  normal[1]=fpdiv(normal[1],l); //TODO
  normal[2]=fpdiv(normal[2],l); //TODO
  
  
  Sint32 ac = (ox>>SP_HALF_ACCURACY)*(normal[0]>>SP_HALF_ACCURACY)
             +(oy>>SP_HALF_ACCURACY)*(normal[1]>>SP_HALF_ACCURACY)
             +(oz>>SP_HALF_ACCURACY)*(normal[2]>>SP_HALF_ACCURACY);
  
    
  if (ac>0)
    return;

         x1 = (spProjection[ 0] >> SP_HALF_ACCURACY)*(tx1 >> SP_HALF_ACCURACY);
         y1 = (spProjection[ 5] >> SP_HALF_ACCURACY)*(ty1 >> SP_HALF_ACCURACY);
/*         z1 = (spProjection[ 2] >> SP_HALF_ACCURACY)*(tx1 >> SP_HALF_ACCURACY)
            + (spProjection[ 6] >> SP_HALF_ACCURACY)*(ty1 >> SP_HALF_ACCURACY)
            + (spProjection[10] >> SP_HALF_ACCURACY)*(tz1 >> SP_HALF_ACCURACY)
            + (spProjection[14] >> SP_HALF_ACCURACY)*(tw1 >> SP_HALF_ACCURACY);
*/  Sint32 w1 = (spProjection[11] >> SP_HALF_ACCURACY)*(tz1 >> SP_HALF_ACCURACY);
  
  if (w1 == 0)
    w1 = 1;  
  Sint32 nx1 = fpdiv(x1,w1)>>SP_HALF_ACCURACY;
  Sint32 ny1 = fpdiv(y1,w1)>>SP_HALF_ACCURACY;
  //Sint32 nz1 = fpdiv(z1,w1)>>SP_HALF_ACCURACY;

         x2 = (spProjection[ 0] >> SP_HALF_ACCURACY)*(tx2 >> SP_HALF_ACCURACY);
         y2 = (spProjection[ 5] >> SP_HALF_ACCURACY)*(ty2 >> SP_HALF_ACCURACY);
/*         z2 = (spProjection[ 2] >> SP_HALF_ACCURACY)*(tx2 >> SP_HALF_ACCURACY)
            + (spProjection[ 6] >> SP_HALF_ACCURACY)*(ty2 >> SP_HALF_ACCURACY)
            + (spProjection[10] >> SP_HALF_ACCURACY)*(tz2 >> SP_HALF_ACCURACY)
            + (spProjection[14] >> SP_HALF_ACCURACY)*(tw2 >> SP_HALF_ACCURACY);
*/  Sint32 w2 = (spProjection[11] >> SP_HALF_ACCURACY)*(tz2 >> SP_HALF_ACCURACY);
      
  if (w2 == 0)
    w2 = 2;  
  Sint32 nx2 = fpdiv(x2,w2)>>SP_HALF_ACCURACY;
  Sint32 ny2 = fpdiv(y2,w2)>>SP_HALF_ACCURACY;
  //Sint32 nz2 = fpdiv(z2,w2)>>SP_HALF_ACCURACY;

         x3 = (spProjection[ 0] >> SP_HALF_ACCURACY)*(tx3 >> SP_HALF_ACCURACY);
         y3 = (spProjection[ 5] >> SP_HALF_ACCURACY)*(ty3 >> SP_HALF_ACCURACY);
/*         z3 = (spProjection[ 2] >> SP_HALF_ACCURACY)*(tx3 >> SP_HALF_ACCURACY)
            + (spProjection[ 6] >> SP_HALF_ACCURACY)*(ty3 >> SP_HALF_ACCURACY)
            + (spProjection[10] >> SP_HALF_ACCURACY)*(tz3 >> SP_HALF_ACCURACY)
            + (spProjection[14] >> SP_HALF_ACCURACY)*(tw3 >> SP_HALF_ACCURACY);
*/  Sint32 w3 = (spProjection[11] >> SP_HALF_ACCURACY)*(tz3 >> SP_HALF_ACCURACY);
      
  if (w3 == 0)
    w3 = 3;  
  Sint32 nx3 = fpdiv(x3,w3)>>SP_HALF_ACCURACY;
  Sint32 ny3 = fpdiv(y3,w3)>>SP_HALF_ACCURACY;
  //Sint32 nz3 = fpdiv(z3,w3)>>SP_HALF_ACCURACY;
  
  spTriangle((windowX >> 1)+((nx1*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             (windowY >> 1)-((ny1*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz1>>SP_ACCURACY,
             (windowX >> 1)+((nx2*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             (windowY >> 1)-((ny2*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz2>>SP_ACCURACY,
             (windowX >> 1)+((nx3*(windowX<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),
             (windowY >> 1)-((ny3*(windowY<<SP_HALF_ACCURACY-1)) >> SP_ACCURACY),tz3>>SP_ACCURACY,color);
}

PREFIX void spQuad3D(Sint32 x1,Sint32 y1,Sint32 z1,
  Sint32 x2,Sint32 y2,Sint32 z2,
  Sint32 x3,Sint32 y3,Sint32 z3,
  Sint32 x4,Sint32 y4,Sint32 z4,Uint16 color);

PREFIX void spTriangleTex3D(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 u1,Sint32 v1,
  Sint32 x2,Sint32 y2,Sint32 z2,Sint32 u2,Sint32 v2,
  Sint32 x3,Sint32 y3,Sint32 z3,Sint32 u3,Sint32 v3,Uint16 color);

PREFIX void spQuadTex3D(Sint32 x1,Sint32 y1,Sint32 z1,Sint32 u1,Sint32 v1,
  Sint32 x2,Sint32 y2,Sint32 z2,Sint32 u2,Sint32 v2,
  Sint32 x3,Sint32 y3,Sint32 z3,Sint32 u3,Sint32 v3,
  Sint32 x4,Sint32 y4,Sint32 z4,Sint32 u4,Sint32 v4,Uint16 color);
