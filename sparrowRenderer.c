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
#include "sparrowCore.h"
#include "sparrowRenderer.h"
#include "sparrowPrimitives.h"
#include "sparrowMath.h"
#define M_PI 3.14159265358979323846
#include <math.h>
#include <string.h>

Sint32 spModelView[16] = {SP_ONE,0,0,0,0,SP_ONE,0,0,0,0,SP_ONE,0,0,0,0,SP_ONE};
Sint32 spProjection[16];
int spLightOn = -1;
spLight spLightDiffuse[SP_MAX_LIGHTS];
Uint32 spLightAmbient[3] = {1 << SP_ACCURACY-2,1 << SP_ACCURACY-2,1 << SP_ACCURACY-2};
int spPreNormal = 0;
int spUsePerspective = 0;

PREFIX void spSetFrustumf2( Sint32 *matrix, Sint32 left, Sint32 right, Sint32 bottom, Sint32 top,
							Sint32 znear, Sint32 zfar )
{
	printf("Created Projection frustum with:\n");
	printf("  Left:   %.3f\n",spFixedToFloat(left));
	printf("  Right:  %.3f\n",spFixedToFloat(right));
	printf("  Top:    %.3f\n",spFixedToFloat(top));
	printf("  Bottom: %.3f\n",spFixedToFloat(bottom));
	printf("  Z-near: %.3f\n",spFixedToFloat(znear));
	printf("  Z-far:  %.3f\n",spFixedToFloat(zfar));
	Sint32 znear2 = znear << 1;
	Sint32 width = right - left;
	Sint32 height = top - bottom;
	Sint32 zRange = znear - zfar;
	matrix[ 0] = spDivHigh( znear2, width );
	matrix[ 1] = 0 << SP_ACCURACY;
	matrix[ 2] = 0 << SP_ACCURACY;
	matrix[ 3] = 0 << SP_ACCURACY;
	matrix[ 4] = 0 << SP_ACCURACY;
	matrix[ 5] = spDivHigh( znear2, height );
	matrix[ 6] = 0 << SP_ACCURACY;
	matrix[ 7] = 0 << SP_ACCURACY;
	matrix[ 8] = spDivHigh( right + left, width ); //0 if right == -left
	matrix[ 9] = spDivHigh( top + bottom, height ); //0 if top == -bottom
	matrix[10] = -SP_ONE;//spDivHigh( zfar + znear, zRange ); //nearly minus one
	matrix[11] = -SP_ONE;
	matrix[12] = 0 << SP_ACCURACY;
	matrix[13] = 0 << SP_ACCURACY;
	matrix[14] = -znear;//spDivHigh( spMulHigh( znear2, zfar ), zRange );
	matrix[15] = 0 << SP_ACCURACY;
}

PREFIX void spSetPerspective( float fovyInDegrees, float aspectRatio,
							  float znear, float zfar )
{
	float ymax, xmax;
	ymax = znear * tanf( fovyInDegrees * M_PI / 360.0f );
	xmax = ymax * aspectRatio;
	spSetFrustumf2( spProjection, ( Sint32 )( -xmax  * SP_ACCURACY_FACTOR ),
	                              ( Sint32 )(  xmax  * SP_ACCURACY_FACTOR ),
	                              ( Sint32 )( -ymax  * SP_ACCURACY_FACTOR ),
	                              ( Sint32 )(  ymax  * SP_ACCURACY_FACTOR ),
	                              ( Sint32 )(  znear * SP_ACCURACY_FACTOR ),
	                              ( Sint32 )(  zfar  * SP_ACCURACY_FACTOR ));
	spSetZFar((Sint32)(zfar * SP_ACCURACY_FACTOR));
	spSetZNear((Sint32)(znear * SP_ACCURACY_FACTOR));
	printf("  Matrix:\n");
	int i;
	for (i = 0; i < 4; i++)
		printf("    | % 3.3f | % 3.3f | % 3.3f | % 3.3f | \n",spFixedToFloat(spProjection[0+i]),spFixedToFloat(spProjection[4+i]),spFixedToFloat(spProjection[8+i]),spFixedToFloat(spProjection[12+i]));	
}

PREFIX void spSetPerspectiveStereoscopic( Sint32* projectionMatrix, float fovyInDegrees, float aspectRatio,
							  float znear, float zfar , float z0,float distance)
{
	float ymax, xmax, ymin, xmin;
	xmin = -znear * tanf( fovyInDegrees * M_PI / 360.0f ) + distance/2.0f*znear/z0;
	xmax =  znear * tanf( fovyInDegrees * M_PI / 360.0f ) + distance/2.0f*znear/z0;
	ymin = -1.0f/aspectRatio * znear * tanf( fovyInDegrees * M_PI / 360.0f );
	ymax =  1.0f/aspectRatio * znear * tanf( fovyInDegrees * M_PI / 360.0f );
	spSetFrustumf2( projectionMatrix, ( Sint32 )( xmin  * SP_ACCURACY_FACTOR ),
	                                  ( Sint32 )( xmax  * SP_ACCURACY_FACTOR ),
	                                  ( Sint32 )( ymin  * SP_ACCURACY_FACTOR ),
	                                  ( Sint32 )( ymax  * SP_ACCURACY_FACTOR ),
	                                  ( Sint32 )( znear * SP_ACCURACY_FACTOR ),
	                                  ( Sint32 )( zfar  * SP_ACCURACY_FACTOR ));
  projectionMatrix[12] = spFloatToFixed(distance);
	spSetZFar((Sint32)(zfar * SP_ACCURACY_FACTOR));
	spSetZNear((Sint32)(znear * SP_ACCURACY_FACTOR));
	printf("  Matrix:\n");
	int i;
	for (i = 0; i < 4; i++)
		printf("    | % 3.3f | % 3.3f | % 3.3f | % 3.3f | \n",spFixedToFloat(projectionMatrix[0+i]),spFixedToFloat(projectionMatrix[4+i]),spFixedToFloat(projectionMatrix[8+i]),spFixedToFloat(projectionMatrix[12+i]));	
}

PREFIX void spStereoCreateProjectionMatrixes(Sint32* left_matrix,Sint32* right_matrix,float fovyInDegrees, float aspectRatio,
							  float znear, float zfar , float z0,float distance,int crossed)
{
	spSetPerspectiveStereoscopic(  left_matrix, fovyInDegrees, aspectRatio/(crossed?2.0f:1.0f), znear, zfar, z0,  distance );
	spSetPerspectiveStereoscopic( right_matrix, fovyInDegrees, aspectRatio/(crossed?2.0f:1.0f), znear, zfar, z0, -distance );	
}

PREFIX Sint32* spGetProjectionMatrix()
{
	return spProjection;
}

PREFIX void spIdentity()
{
	spModelView[ 0] = SP_ONE;
	spModelView[ 1] = 0 << SP_ACCURACY;
	spModelView[ 2] = 0 << SP_ACCURACY;
	spModelView[ 3] = 0 << SP_ACCURACY;
	spModelView[ 4] = 0 << SP_ACCURACY;
	spModelView[ 5] = SP_ONE;
	spModelView[ 6] = 0 << SP_ACCURACY;
	spModelView[ 7] = 0 << SP_ACCURACY;
	spModelView[ 8] = 0 << SP_ACCURACY;
	spModelView[ 9] = 0 << SP_ACCURACY;
	spModelView[10] = SP_ONE;
	spModelView[11] = 0 << SP_ACCURACY;
	spModelView[12] = 0 << SP_ACCURACY;
	spModelView[13] = 0 << SP_ACCURACY;
	spModelView[14] = 0 << SP_ACCURACY;
	spModelView[15] = SP_ONE;
}

PREFIX void spScale( Sint32 x, Sint32 y, Sint32 z )
{
	spModelView[ 0] = spMul( spModelView[ 0], x );
	spModelView[ 1] = spMul( spModelView[ 1], x );
	spModelView[ 2] = spMul( spModelView[ 2], x );
	spModelView[ 3] = spMul( spModelView[ 3], x );
	
	spModelView[ 4] = spMul( spModelView[ 4], y );
	spModelView[ 5] = spMul( spModelView[ 5], y );
	spModelView[ 6] = spMul( spModelView[ 6], y );
	spModelView[ 7] = spMul( spModelView[ 7], y );
	
	spModelView[ 8] = spMul( spModelView[ 8], z );
	spModelView[ 9] = spMul( spModelView[ 9], z );
	spModelView[10] = spMul( spModelView[10], z );
	spModelView[11] = spMul( spModelView[11], z );
}

PREFIX void spRotate( Sint32 x, Sint32 y, Sint32 z, Sint32 rad )
{
	//Rotation matrix:
	Sint32 s = spSin( rad );
	Sint32 c = spCos( rad );
	Sint32 l = spSqrt( spMul( x, x )
					   + spMul( y, y )
					   + spMul( z, z ) );
	if ( l == 0 )
		return;
	x = spDiv( x, l );
	y = spDiv( y, l );
	z = spDiv( z, l );
	Sint32 rotate[16];
	rotate[ 0] = c + spMul( spMul( x, x ), ( SP_ONE ) - c );
	rotate[ 4] =   spMul( spMul( x, y ), ( SP_ONE ) - c ) - spMul( z, s );
	rotate[ 8] =   spMul( spMul( x, z ), ( SP_ONE ) - c ) + spMul( y, s );
	rotate[12] = 0;
	rotate[ 1] =   spMul( spMul( y, x ), ( SP_ONE ) - c ) + spMul( z, s );
	rotate[ 5] = c + spMul( spMul( y, y ), ( SP_ONE ) - c );
	rotate[ 9] =   spMul( spMul( y, z ), ( SP_ONE ) - c ) - spMul( x, s );
	rotate[13] = 0;
	rotate[ 2] =   spMul( spMul( z, x ), ( SP_ONE ) - c ) - spMul( y, s );
	rotate[ 6] =   spMul( spMul( z, y ), ( SP_ONE ) - c ) + spMul( x, s );
	rotate[10] = c + spMul( spMul( z, z ), ( SP_ONE ) - c );
	rotate[14] = 0;
	rotate[ 3] = 0;
	rotate[ 7] = 0;
	rotate[11] = 0;
	rotate[15] = SP_ONE;

	Sint32 result[12];
	result[ 0] = ( spModelView[ 0] >> SP_HALF_ACCURACY ) * ( rotate[ 0] >> SP_HALF_ACCURACY ) + ( spModelView[ 4] >> SP_HALF_ACCURACY ) * ( rotate[ 1] >> SP_HALF_ACCURACY ) + ( spModelView[ 8] >> SP_HALF_ACCURACY ) * ( rotate[ 2] >> SP_HALF_ACCURACY );
	result[ 1] = ( spModelView[ 1] >> SP_HALF_ACCURACY ) * ( rotate[ 0] >> SP_HALF_ACCURACY ) + ( spModelView[ 5] >> SP_HALF_ACCURACY ) * ( rotate[ 1] >> SP_HALF_ACCURACY ) + ( spModelView[ 9] >> SP_HALF_ACCURACY ) * ( rotate[ 2] >> SP_HALF_ACCURACY );
	result[ 2] = ( spModelView[ 2] >> SP_HALF_ACCURACY ) * ( rotate[ 0] >> SP_HALF_ACCURACY ) + ( spModelView[ 6] >> SP_HALF_ACCURACY ) * ( rotate[ 1] >> SP_HALF_ACCURACY ) + ( spModelView[10] >> SP_HALF_ACCURACY ) * ( rotate[ 2] >> SP_HALF_ACCURACY );
	result[ 3] = ( spModelView[ 3] >> SP_HALF_ACCURACY ) * ( rotate[ 0] >> SP_HALF_ACCURACY ) + ( spModelView[ 7] >> SP_HALF_ACCURACY ) * ( rotate[ 1] >> SP_HALF_ACCURACY ) + ( spModelView[11] >> SP_HALF_ACCURACY ) * ( rotate[ 2] >> SP_HALF_ACCURACY );

	result[ 4] = ( spModelView[ 0] >> SP_HALF_ACCURACY ) * ( rotate[ 4] >> SP_HALF_ACCURACY ) + ( spModelView[ 4] >> SP_HALF_ACCURACY ) * ( rotate[ 5] >> SP_HALF_ACCURACY ) + ( spModelView[ 8] >> SP_HALF_ACCURACY ) * ( rotate[ 6] >> SP_HALF_ACCURACY );
	result[ 5] = ( spModelView[ 1] >> SP_HALF_ACCURACY ) * ( rotate[ 4] >> SP_HALF_ACCURACY ) + ( spModelView[ 5] >> SP_HALF_ACCURACY ) * ( rotate[ 5] >> SP_HALF_ACCURACY ) + ( spModelView[ 9] >> SP_HALF_ACCURACY ) * ( rotate[ 6] >> SP_HALF_ACCURACY );
	result[ 6] = ( spModelView[ 2] >> SP_HALF_ACCURACY ) * ( rotate[ 4] >> SP_HALF_ACCURACY ) + ( spModelView[ 6] >> SP_HALF_ACCURACY ) * ( rotate[ 5] >> SP_HALF_ACCURACY ) + ( spModelView[10] >> SP_HALF_ACCURACY ) * ( rotate[ 6] >> SP_HALF_ACCURACY );
	result[ 7] = ( spModelView[ 3] >> SP_HALF_ACCURACY ) * ( rotate[ 4] >> SP_HALF_ACCURACY ) + ( spModelView[ 7] >> SP_HALF_ACCURACY ) * ( rotate[ 5] >> SP_HALF_ACCURACY ) + ( spModelView[11] >> SP_HALF_ACCURACY ) * ( rotate[ 6] >> SP_HALF_ACCURACY );

	result[ 8] = ( spModelView[ 0] >> SP_HALF_ACCURACY ) * ( rotate[ 8] >> SP_HALF_ACCURACY ) + ( spModelView[ 4] >> SP_HALF_ACCURACY ) * ( rotate[ 9] >> SP_HALF_ACCURACY ) + ( spModelView[ 8] >> SP_HALF_ACCURACY ) * ( rotate[10] >> SP_HALF_ACCURACY );
	result[ 9] = ( spModelView[ 1] >> SP_HALF_ACCURACY ) * ( rotate[ 8] >> SP_HALF_ACCURACY ) + ( spModelView[ 5] >> SP_HALF_ACCURACY ) * ( rotate[ 9] >> SP_HALF_ACCURACY ) + ( spModelView[ 9] >> SP_HALF_ACCURACY ) * ( rotate[10] >> SP_HALF_ACCURACY );
	result[10] = ( spModelView[ 2] >> SP_HALF_ACCURACY ) * ( rotate[ 8] >> SP_HALF_ACCURACY ) + ( spModelView[ 6] >> SP_HALF_ACCURACY ) * ( rotate[ 9] >> SP_HALF_ACCURACY ) + ( spModelView[10] >> SP_HALF_ACCURACY ) * ( rotate[10] >> SP_HALF_ACCURACY );
	result[11] = ( spModelView[ 3] >> SP_HALF_ACCURACY ) * ( rotate[ 8] >> SP_HALF_ACCURACY ) + ( spModelView[ 7] >> SP_HALF_ACCURACY ) * ( rotate[ 9] >> SP_HALF_ACCURACY ) + ( spModelView[11] >> SP_HALF_ACCURACY ) * ( rotate[10] >> SP_HALF_ACCURACY );

	memcpy( spModelView, result, sizeof( Sint32 ) * 12 );
}

PREFIX void spRotateX( Sint32 rad )
{
	//Rotation matrix:
	Sint32 s = spSin( rad );
	Sint32 c = spCos( rad );

	Sint32 rotate[16];
	rotate[ 0] = SP_ONE;
	rotate[ 4] = 0;
	rotate[ 8] = 0;
	rotate[12] = 0;
	rotate[ 1] = 0;
	rotate[ 5] = c;
	rotate[ 9] = -s;
	rotate[13] = 0;
	rotate[ 2] = 0;
	rotate[ 6] = s;
	rotate[10] = c;
	rotate[14] = 0;
	rotate[ 3] = 0;
	rotate[ 7] = 0;
	rotate[11] = 0;
	rotate[15] = SP_ONE;

	Sint32 result[8];
	//+4
	result[ 0] = spMul( spModelView[ 4], rotate[ 5] ) + spMul( spModelView[ 8], rotate[ 6] );
	result[ 1] = spMul( spModelView[ 5], rotate[ 5] ) + spMul( spModelView[ 9], rotate[ 6] );
	result[ 2] = spMul( spModelView[ 6], rotate[ 5] ) + spMul( spModelView[10], rotate[ 6] );
	result[ 3] = spMul( spModelView[ 7], rotate[ 5] ) + spMul( spModelView[11], rotate[ 6] );

	result[ 4] = spMul( spModelView[ 4], rotate[ 9] ) + spMul( spModelView[ 8], rotate[10] );
	result[ 5] = spMul( spModelView[ 5], rotate[ 9] ) + spMul( spModelView[ 9], rotate[10] );
	result[ 6] = spMul( spModelView[ 6], rotate[ 9] ) + spMul( spModelView[10], rotate[10] );
	result[ 7] = spMul( spModelView[ 7], rotate[ 9] ) + spMul( spModelView[11], rotate[10] );

	memcpy( &( spModelView[4] ), result, sizeof( Sint32 ) * 8 );
}

PREFIX void spRotateY( Sint32 rad )
{
	//Rotation matrix:
	Sint32 s = spSin( rad );
	Sint32 c = spCos( rad );

	Sint32 rotate[16];
	rotate[ 0] = c;
	rotate[ 4] = 0;
	rotate[ 8] = s;
	rotate[12] = 0;
	rotate[ 1] = 0;
	rotate[ 5] = SP_ONE;
	rotate[ 9] = 0;
	rotate[13] = 0;
	rotate[ 2] = -s;
	rotate[ 6] = 0;
	rotate[10] = c;
	rotate[14] = 0;
	rotate[ 3] = 0;
	rotate[ 7] = 0;
	rotate[11] = 0;
	rotate[15] = SP_ONE;

	Sint32 result[4];
	Sint32 result_8[4];
	result[ 0] = spMul( spModelView[ 0], rotate[ 0] ) + spMul( spModelView[ 8], rotate[ 2] );
	result[ 1] = spMul( spModelView[ 1], rotate[ 0] ) + spMul( spModelView[ 9], rotate[ 2] );
	result[ 2] = spMul( spModelView[ 2], rotate[ 0] ) + spMul( spModelView[10], rotate[ 2] );
	result[ 3] = spMul( spModelView[ 3], rotate[ 0] ) + spMul( spModelView[11], rotate[ 2] );
	//+8!
	result_8[ 0] = spMul( spModelView[ 0], rotate[ 8] ) + spMul( spModelView[ 8], rotate[10] );
	result_8[ 1] = spMul( spModelView[ 1], rotate[ 8] ) + spMul( spModelView[ 9], rotate[10] );
	result_8[ 2] = spMul( spModelView[ 2], rotate[ 8] ) + spMul( spModelView[10], rotate[10] );
	result_8[ 3] = spMul( spModelView[ 3], rotate[ 8] ) + spMul( spModelView[11], rotate[10] );
	memcpy( spModelView, result, sizeof( Sint32 ) * 4 );
	memcpy( &( spModelView[8] ), result_8, sizeof( Sint32 ) * 4 );
}

PREFIX void spRotateZ( Sint32 rad )
{
	//Rotation matrix:
	Sint32 s = spSin( rad );
	Sint32 c = spCos( rad );

	Sint32 rotate[16];
	rotate[ 0] = c;
	rotate[ 4] = -s;
	rotate[ 8] = 0;
	rotate[12] = 0;
	rotate[ 1] = s;
	rotate[ 5] = c;
	rotate[ 9] = 0;
	rotate[13] = 0;
	rotate[ 2] = 0;
	rotate[ 6] = 0;
	rotate[10] = SP_ONE;
	rotate[14] = 0;
	rotate[ 3] = 0;
	rotate[ 7] = 0;
	rotate[11] = 0;
	rotate[15] = SP_ONE;

	Sint32 result[8];
	result[ 0] = spMul( spModelView[ 0], rotate[ 0] ) + spMul( spModelView[ 4], rotate[ 1] );
	result[ 1] = spMul( spModelView[ 1], rotate[ 0] ) + spMul( spModelView[ 5], rotate[ 1] );
	result[ 2] = spMul( spModelView[ 2], rotate[ 0] ) + spMul( spModelView[ 6], rotate[ 1] );
	result[ 3] = spMul( spModelView[ 3], rotate[ 0] ) + spMul( spModelView[ 7], rotate[ 1] );

	result[ 4] = spMul( spModelView[ 0], rotate[ 4] ) + spMul( spModelView[ 4], rotate[ 5] );
	result[ 5] = spMul( spModelView[ 1], rotate[ 4] ) + spMul( spModelView[ 5], rotate[ 5] );
	result[ 6] = spMul( spModelView[ 2], rotate[ 4] ) + spMul( spModelView[ 6], rotate[ 5] );
	result[ 7] = spMul( spModelView[ 3], rotate[ 4] ) + spMul( spModelView[ 7], rotate[ 5] );

	memcpy( spModelView, result, sizeof( Sint32 ) * 8 );
}

PREFIX void spTranslate( Sint32 x, Sint32 y, Sint32 z )
{
	spModelView[12] = spMul( spModelView[ 0], x ) + spMul( spModelView[ 4], y ) + spMul( spModelView[ 8], z ) + spModelView[12];
	spModelView[13] = spMul( spModelView[ 1], x ) + spMul( spModelView[ 5], y ) + spMul( spModelView[ 9], z ) + spModelView[13];
	spModelView[14] = spMul( spModelView[ 2], x ) + spMul( spModelView[ 6], y ) + spMul( spModelView[10], z ) + spModelView[14];
	spModelView[15] = spMul( spModelView[ 3], x ) + spMul( spModelView[ 7], y ) + spMul( spModelView[11], z ) + spModelView[15];
}

PREFIX Sint32* spGetMatrix()
{
	return spModelView;
}

PREFIX void spSetMatrix( Sint32* matrix )
{
	memcpy( spModelView, matrix, 16 * sizeof( Sint32 ) );
}

inline void spCalcNormal( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2,
						  Sint32 x3, Sint32 y3, Sint32 z3, Sint32* normale )
{
	normale[0] = spMul( y1 - y2, z2 - z3 )
				 - spMul( z1 - z2, y2 - y3 );
	normale[1] = spMul( z1 - z2, x2 - x3 )
				 - spMul( x1 - x2, z2 - z3 );
	normale[2] = spMul( x1 - x2, y2 - y3 )
				 - spMul( y1 - y2, x2 - x3 );
}


inline void spMulModellView( Sint32 x, Sint32 y, Sint32 z, Sint32 *tx, Sint32 *ty, Sint32 *tz, Sint32 *tw )
{
	( *tx ) = spMul( spModelView[ 0], x )
			  + spMul( spModelView[ 4], y )
			  + spMul( spModelView[ 8], z )
			  + spModelView[12];
	( *ty ) = spMul( spModelView[ 1], x )
			  + spMul( spModelView[ 5], y )
			  + spMul( spModelView[ 9], z )
			  + spModelView[13];
	( *tz ) = spMul( spModelView[ 2], x )
			  + spMul( spModelView[ 6], y )
			  + spMul( spModelView[10], z )
			  + spModelView[14];
	( *tw ) = spMul( spModelView[ 3], x )
			  + spMul( spModelView[ 7], y )
			  + spMul( spModelView[11], z )
			  + spModelView[15];
}

inline void spMulModellView3x3( Sint32* p, Sint32* t)
{
	t[0] = spMul( spModelView[ 0], p[0] )
	     + spMul( spModelView[ 4], p[1] )
	     + spMul( spModelView[ 8], p[2] );
	t[1] = spMul( spModelView[ 1], p[0] )
	     + spMul( spModelView[ 5], p[1] )
	     + spMul( spModelView[ 9], p[2] );
	t[2] = spMul( spModelView[ 2], p[0] )
	     + spMul( spModelView[ 6], p[1] )
	     + spMul( spModelView[10], p[2] );
}

//senquack - credit for this fast sqrt goes to Wilco Dijkstra http://www.finesse.demon.co.uk/steven/sqrt.html
#define iter1(N) \
	tryv = root + (1 << (N));\
	if (n >= tryv << (N))\
	{\
		n -= tryv << (N);\
		root |= 2 << (N);\
	}

#ifdef FAST_BUT_UGLY
SP_LIGHT_TYPE lightSqrt (SP_LIGHT_TYPE n)
{
	if (n <= 0)
		return 0;
	n >>= SP_LIGHT_ACCURACY - SP_ACCURACY; //Accuracy lost to SP_ACCURACY
	if (n <= ((SP_LIGHT_TYPE)1 << SP_SQRT_ACCURACY)+1)
		return (SP_LIGHT_TYPE)(spUnsave_Small_Sqrt(n)) << SP_LIGHT_ACCURACY - SP_ACCURACY;
	int bit_count = SP_LIGHT_TYPE_SIZE - 2 - SP_LIGHT_ACCURACY + SP_ACCURACY;
	SP_LIGHT_TYPE x = (SP_LIGHT_TYPE)1 << bit_count;
	while (x > n)
	{
		bit_count-=2;
		x >>= 2;
	}
	SP_LIGHT_TYPE result = (SP_LIGHT_TYPE)(spUnsave_Small_Sqrt(n >> bit_count - SP_SQRT_ACCURACY + 2 )) << (bit_count - SP_SQRT_ACCURACY >> 1) + 1 + SP_LIGHT_ACCURACY - SP_ACCURACY;
	return result;
}
#else
SP_LIGHT_TYPE lightSqrt ( SP_LIGHT_TYPE n )
{
	SP_LIGHT_TYPE root = 0, tryv;
	iter1 ( 30 );
	iter1 ( 29 );
	iter1 ( 28 );
	iter1 ( 27 );
	iter1 ( 26 );
	iter1 ( 25 );
	iter1 ( 24 );
	iter1 ( 23 );
	iter1 ( 22 );
	iter1 ( 21 );
	iter1 ( 20 );
	iter1 ( 19 );
	iter1 ( 18 );
	iter1 ( 17 );
	iter1 ( 16 );
	iter1 ( 15 );
	iter1 ( 14 );
	iter1 ( 13 );
	iter1 ( 12 );
	iter1 ( 11 );
	iter1 ( 10 );
	iter1 ( 9 );
	iter1 ( 8 );
	iter1 ( 7 );
	iter1 ( 6 );
	iter1 ( 5 );
	iter1 ( 4 );
	iter1 ( 3 );
	iter1 ( 2 );
	iter1 ( 1 );
	iter1 ( 0 );
	return root << ( SP_LIGHT_HALF_ACCURACY - 1 );
}
#endif

#define spLightMul(a,b) (((a)>>SP_LIGHT_HALF_ACCURACY) * ((b)>>SP_LIGHT_HALF_ACCURACY))
#define spLightDiv(a,b) (((a)<<SP_LIGHT_HALF_ACCURACY)/(b)<<SP_LIGHT_HALF_ACCURACY)

/*SP_LIGHT_TYPE lightSqrt ( SP_LIGHT_TYPE n )
{
	SP_LIGHT_TYPE root = 1 << 31;
	int i;
	for (i = 31; i > 0; i--)
	{
		if (spLightMul(root,root) >= n)
			root -= 1 << i;
		else
			root += 1 << i;
	}
	return root;
}*/


//#define lightSqrt(n) (SP_LIGHT_TYPE)(sqrt((float)(n)/SP_LIGHT_ACCURACY_FACTOR)*SP_LIGHT_ACCURACY_FACTOR)


inline void spCalcLightNormal( SP_LIGHT_TYPE x1, SP_LIGHT_TYPE y1, SP_LIGHT_TYPE z1, SP_LIGHT_TYPE x2, SP_LIGHT_TYPE y2, SP_LIGHT_TYPE z2,
                               SP_LIGHT_TYPE x3, SP_LIGHT_TYPE y3, SP_LIGHT_TYPE z3, SP_LIGHT_TYPE* normale )
{
	normale[0] = spLightMul( y1 - y2, z2 - z3 )
	           - spLightMul( z1 - z2, y2 - y3 );
	normale[1] = spLightMul( z1 - z2, x2 - x3 )
             - spLightMul( x1 - x2, z2 - z3 );
	normale[2] = spLightMul( x1 - x2, y2 - y3 )
             - spLightMul( y1 - y2, x2 - x3 );
}

inline Uint16 rendererLightCalculation( Uint16 color, Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3 )
{
	if ( spLightOn <= 0)
		return color;
	Sint32 or = ( color >> 11 ); //0..31
	Sint32 og = ( ( color & 2047 ) >> 5 ); //0..63
	Sint32 ob = ( color & 31 ); //0..31
	//globale light:

	SP_LIGHT_TYPE r = spLightAmbient[0] * or << SP_LIGHT_ACCURACY - SP_ACCURACY;
	SP_LIGHT_TYPE g = spLightAmbient[1] * og << SP_LIGHT_ACCURACY - SP_ACCURACY;
	SP_LIGHT_TYPE b = spLightAmbient[2] * ob << SP_LIGHT_ACCURACY - SP_ACCURACY;

	//the other lights
	int i;
	for ( i = 0; i < SP_MAX_LIGHTS; i++ )
	{
		if ( !spLightDiffuse[i].active )
			continue;
		SP_LIGHT_TYPE normale[3];
		spCalcLightNormal( x1 << SP_LIGHT_ACCURACY - SP_ACCURACY, y1 << SP_LIGHT_ACCURACY - SP_ACCURACY, z1 << SP_LIGHT_ACCURACY - SP_ACCURACY,
		                   x2 << SP_LIGHT_ACCURACY - SP_ACCURACY, y2 << SP_LIGHT_ACCURACY - SP_ACCURACY, z2 << SP_LIGHT_ACCURACY - SP_ACCURACY,
		                   x3 << SP_LIGHT_ACCURACY - SP_ACCURACY, y3 << SP_LIGHT_ACCURACY - SP_ACCURACY, z3 << SP_LIGHT_ACCURACY - SP_ACCURACY, normale );
		SP_LIGHT_TYPE normale_length = lightSqrt( spLightMul( normale[0], normale[0] ) +
																			 spLightMul( normale[1], normale[1] ) +
																			 spLightMul( normale[2], normale[2] ) );
		SP_LIGHT_TYPE direction[3];
		direction[0] = (SP_LIGHT_TYPE)(spLightDiffuse[i].tx - ( x1 + x2 >> 1 )) << SP_LIGHT_ACCURACY - SP_ACCURACY;
		direction[1] = (SP_LIGHT_TYPE)(spLightDiffuse[i].ty - ( y1 + y2 >> 1 )) << SP_LIGHT_ACCURACY - SP_ACCURACY;
		direction[2] = (SP_LIGHT_TYPE)(spLightDiffuse[i].tz - ( z1 + z2 >> 1 )) << SP_LIGHT_ACCURACY - SP_ACCURACY;
		SP_LIGHT_TYPE direction_length = lightSqrt( spLightMul( direction[0], direction[0] ) +
																								spLightMul( direction[1], direction[1] ) +
																								spLightMul( direction[2], direction[2] ) );
		SP_LIGHT_TYPE div = spLightMul( direction_length, normale_length );
		if ( div == 0 )
			div = 1;
		SP_LIGHT_TYPE ac = spLightDiv( spLightMul( direction[0], normale[0] ) +
		                               spLightMul( direction[1], normale[1] ) +
		                               spLightMul( direction[2], normale[2] ) , div );
		if ( ac < 0 )
			ac = 0;
		if ( ac > ( (SP_LIGHT_TYPE)1 << SP_LIGHT_ACCURACY ) )
			ac = (SP_LIGHT_TYPE)1 << SP_LIGHT_ACCURACY;
		r += spLightMul( ac, spLightDiffuse[i].r << SP_LIGHT_ACCURACY - SP_ACCURACY ) * or;
		g += spLightMul( ac, spLightDiffuse[i].g << SP_LIGHT_ACCURACY - SP_ACCURACY ) * og;
		b += spLightMul( ac, spLightDiffuse[i].b << SP_LIGHT_ACCURACY - SP_ACCURACY ) * ob;
	}

	r = r >> SP_LIGHT_ACCURACY;
	if ( r > 31 )
		r = 31;
	if ( r < 0 )
		r = 0;
	g = g >> SP_LIGHT_ACCURACY;
	if ( g > 63 )
		g = 63;
	if ( g < 0 )
		g = 0;
	b = b >> SP_LIGHT_ACCURACY;
	if ( b > 31 )
		b = 31;
	if ( b < 0 )
		b = 0;
	color = ( r << 11 ) + ( g << 5 ) + b;
	return color;
}

inline Uint16 rendererLightCalculationKnowNormal( Uint16 color, Sint32 x1, Sint32 y1, Sint32 z1, Sint32 x2, Sint32 y2, Sint32 z2, Sint32 x3, Sint32 y3, Sint32 z3 , Sint32* normal)
{
	if ( spLightOn <= 0)
		return color;
	Sint32 or = ( color >> 11 ); //0..31
	Sint32 og = ( ( color & 2047 ) >> 5 ); //0..63
	Sint32 ob = ( color & 31 ); //0..31
	//globale light:

	SP_LIGHT_TYPE r = spLightAmbient[0] * or;
	SP_LIGHT_TYPE g = spLightAmbient[1] * og;
	SP_LIGHT_TYPE b = spLightAmbient[2] * ob;

	//the other lights
	int i;
	for ( i = 0; i < SP_MAX_LIGHTS; i++ )
	{
		if ( !spLightDiffuse[i].active )
			continue;
		Sint32 direction[3];
		direction[0] = spLightDiffuse[i].tx - ( x1 + x2 >> 1 );
		direction[1] = spLightDiffuse[i].ty - ( y1 + y2 >> 1 );
		direction[2] = spLightDiffuse[i].tz - ( z1 + z2 >> 1 );
		Sint32 direction_length = spSqrt( spMulHigh( direction[0], direction[0] ) +
		                                  spMulHigh( direction[1], direction[1] ) +
		                                  spMulHigh( direction[2], direction[2] ) );
		if ( direction_length == 0 )
			direction_length = 1;
		Sint32 ac = spDivHigh( spMulHigh( direction[0], normal[0] ) +
		                       spMulHigh( direction[1], normal[1] ) +
		                       spMulHigh( direction[2], normal[2] ) , direction_length );
		if ( ac < 0 )
			ac = 0;
		if ( ac > SP_ONE )
			ac = SP_ONE;
		r += spMulHigh( ac, spLightDiffuse[i].r ) * or;
		g += spMulHigh( ac, spLightDiffuse[i].g ) * og;
		b += spMulHigh( ac, spLightDiffuse[i].b ) * ob;
	}

	r = r >> SP_ACCURACY;
	if ( r > 31 )
		r = 31;
	if ( r < 0 )
		r = 0;
	g = g >> SP_ACCURACY;
	if ( g > 63 )
		g = 63;
	if ( g < 0 )
		g = 0;
	b = b >> SP_ACCURACY;
	if ( b > 31 )
		b = 31;
	if ( b < 0 )
		b = 0;
	color = ( r << 11 ) + ( g << 5 ) + b;
	return color;
}

PREFIX int spTriangle3D( Sint32 x1, Sint32 y1, Sint32 z1,
						 Sint32 x2, Sint32 y2, Sint32 z2,
						 Sint32 x3, Sint32 y3, Sint32 z3, Uint16 color )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	Sint32 tx2, ty2, tz2, tw2;
	Sint32 tx3, ty3, tz3, tw3;
	spMulModellView( x1, y1, z1, &tx1, &ty1, &tz1, &tw1 );
	spMulModellView( x2, y2, z2, &tx2, &ty2, &tz2, &tw2 );
	spMulModellView( x3, y3, z3, &tx3, &ty3, &tz3, &tw3 );

	x1 = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8],tz1) + spProjection[12];
	y1 = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9],tz1);
	z1 = spMul( spProjection[10], tz1 ) + spProjection[14];
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;

	Sint32 nx1 = spDiv( x1, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y1, w1 ) >> SP_HALF_ACCURACY;

	x2 = spMul( spProjection[ 0], tx2 ) + spMul(spProjection[ 8],tz2) + spProjection[12];
	y2 = spMul( spProjection[ 5], ty2 ) + spMul(spProjection[ 9],tz2);
	z2 = spMul( spProjection[10], tz2 ) + spProjection[14];
	Sint32 w2 = spMul( spProjection[11], tz2 );
	if ( w2 == 0 )
		w2 = 1;
	Sint32 nx2 = spDiv( x2, w2 ) >> SP_HALF_ACCURACY;
	Sint32 ny2 = spDiv( y2, w2 ) >> SP_HALF_ACCURACY;

	x3 = spMul( spProjection[ 0], tx3 ) + spMul(spProjection[ 8],tz3) + spProjection[12];
	y3 = spMul( spProjection[ 5], ty3 ) + spMul(spProjection[ 9],tz3);
	z3 = spMul( spProjection[10], tz3 ) + spProjection[14];
	Sint32 w3 = spMul( spProjection[11], tz3 );
	if ( w3 == 0 )
		w3 = 1;
	Sint32 nx3 = spDiv( x3, w3 ) >> SP_HALF_ACCURACY;
	Sint32 ny3 = spDiv( y3, w3 ) >> SP_HALF_ACCURACY;
	return
		spTriangle( viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
					viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z1,
					viewPortX + ( ( nx2 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
					viewPortY - ( ( ny2 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z2,
					viewPortX + ( ( nx3 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
					viewPortY - ( ( ny3 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z3, rendererLightCalculation( color, tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3 ) );
}

PREFIX int spQuad3D( Sint32 x1, Sint32 y1, Sint32 z1,
					 Sint32 x2, Sint32 y2, Sint32 z2,
					 Sint32 x3, Sint32 y3, Sint32 z3,
					 Sint32 x4, Sint32 y4, Sint32 z4, Uint16 color )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	Sint32 tx2, ty2, tz2, tw2;
	Sint32 tx3, ty3, tz3, tw3;
	Sint32 tx4, ty4, tz4, tw4;
	spMulModellView( x1, y1, z1, &tx1, &ty1, &tz1, &tw1 );
	spMulModellView( x2, y2, z2, &tx2, &ty2, &tz2, &tw2 );
	spMulModellView( x3, y3, z3, &tx3, &ty3, &tz3, &tw3 );
	spMulModellView( x4, y4, z4, &tx4, &ty4, &tz4, &tw4 );

	x1 = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8],tz1) + spProjection[12];
	y1 = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9],tz1);
	z1 = spMul( spProjection[10], tz1 ) + spProjection[14];
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;

	Sint32 nx1 = spDiv( x1, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y1, w1 ) >> SP_HALF_ACCURACY;

	x2 = spMul( spProjection[ 0], tx2 ) + spMul(spProjection[ 8],tz2) + spProjection[12];
	y2 = spMul( spProjection[ 5], ty2 ) + spMul(spProjection[ 9],tz2);
	z2 = spMul( spProjection[10], tz2 ) + spProjection[14];
	Sint32 w2 = spMul( spProjection[11], tz2 );
	if ( w2 == 0 )
		w2 = 1;
	Sint32 nx2 = spDiv( x2, w2 ) >> SP_HALF_ACCURACY;
	Sint32 ny2 = spDiv( y2, w2 ) >> SP_HALF_ACCURACY;

	x3 = spMul( spProjection[ 0], tx3 ) + spMul(spProjection[ 8],tz3) + spProjection[12];
	y3 = spMul( spProjection[ 5], ty3 ) + spMul(spProjection[ 9],tz3);
	z3 = spMul( spProjection[10], tz3 ) + spProjection[14];
	Sint32 w3 = spMul( spProjection[11], tz3 );
	if ( w3 == 0 )
		w3 = 1;
	Sint32 nx3 = spDiv( x3, w3 ) >> SP_HALF_ACCURACY;
	Sint32 ny3 = spDiv( y3, w3 ) >> SP_HALF_ACCURACY;

	x4 = spMul( spProjection[ 0], tx4 ) + spMul(spProjection[ 8],tz4) + spProjection[12];
	y4 = spMul( spProjection[ 5], ty4 ) + spMul(spProjection[ 9],tz4);
	z4 = spMul( spProjection[10], tz4 ) + spProjection[14];
	Sint32 w4 = spMul( spProjection[11], tz4 );
	if ( w4 == 0 )
		w4 = 1;
	Sint32 nx4 = spDiv( x4, w4 ) >> SP_HALF_ACCURACY;
	Sint32 ny4 = spDiv( y4, w4 ) >> SP_HALF_ACCURACY;

	return
		spQuad( viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
				viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z1,
				viewPortX + ( ( nx2 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
				viewPortY - ( ( ny2 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z2,
				viewPortX + ( ( nx3 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
				viewPortY - ( ( ny3 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z3,
				viewPortX + ( ( nx4 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
				viewPortY - ( ( ny4 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z4, rendererLightCalculation( color, tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3 ) );
}

PREFIX int spTriangleTex3D( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1,
							Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2,
							Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3, Uint16 color )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	Sint32 tx2, ty2, tz2, tw2;
	Sint32 tx3, ty3, tz3, tw3;
	spMulModellView( x1, y1, z1, &tx1, &ty1, &tz1, &tw1 );
	spMulModellView( x2, y2, z2, &tx2, &ty2, &tz2, &tw2 );
	spMulModellView( x3, y3, z3, &tx3, &ty3, &tz3, &tw3 );

	x1 = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8],tz1) + spProjection[12];
	y1 = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9],tz1);
	z1 = spMul( spProjection[10], tz1 ) + spProjection[14];
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;

	Sint32 nx1 = spDiv( x1, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y1, w1 ) >> SP_HALF_ACCURACY;

	x2 = spMul( spProjection[ 0], tx2 ) + spMul(spProjection[ 8],tz2) + spProjection[12];
	y2 = spMul( spProjection[ 5], ty2 ) + spMul(spProjection[ 9],tz2);
	z2 = spMul( spProjection[10], tz2 ) + spProjection[14];
	Sint32 w2 = spMul( spProjection[11], tz2 );
	if ( w2 == 0 )
		w2 = 1;
	Sint32 nx2 = spDiv( x2, w2 ) >> SP_HALF_ACCURACY;
	Sint32 ny2 = spDiv( y2, w2 ) >> SP_HALF_ACCURACY;

	x3 = spMul( spProjection[ 0], tx3 ) + spMul(spProjection[ 8],tz3) + spProjection[12];
	y3 = spMul( spProjection[ 5], ty3 ) + spMul(spProjection[ 9],tz3);
	z3 = spMul( spProjection[10], tz3 ) + spProjection[14];
	Sint32 w3 = spMul( spProjection[11], tz3 );
	if ( w3 == 0 )
		w3 = 1;
	Sint32 nx3 = spDiv( x3, w3 ) >> SP_HALF_ACCURACY;
	Sint32 ny3 = spDiv( y3, w3 ) >> SP_HALF_ACCURACY;

	if (spUsePerspective)
		return
			spPerspectiveTriangle_tex( viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
							viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z1, u1, v1, w1,
							viewPortX + ( ( nx2 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
							viewPortY - ( ( ny2 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z2, u2, v2, w2,
							viewPortX + ( ( nx3 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
							viewPortY - ( ( ny3 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z3, u3, v3, w3, rendererLightCalculation( color, tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3 ) );
	return
		spTriangle_tex( viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
						viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z1, u1, v1,
						viewPortX + ( ( nx2 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
						viewPortY - ( ( ny2 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z2, u2, v2,
						viewPortX + ( ( nx3 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
						viewPortY - ( ( ny3 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z3, u3, v3, rendererLightCalculation( color, tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3 ) );
}


PREFIX int spQuadTex3D( Sint32 x1, Sint32 y1, Sint32 z1, Sint32 u1, Sint32 v1,
						Sint32 x2, Sint32 y2, Sint32 z2, Sint32 u2, Sint32 v2,
						Sint32 x3, Sint32 y3, Sint32 z3, Sint32 u3, Sint32 v3,
						Sint32 x4, Sint32 y4, Sint32 z4, Sint32 u4, Sint32 v4, Uint16 color )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	Sint32 tx2, ty2, tz2, tw2;
	Sint32 tx3, ty3, tz3, tw3;
	Sint32 tx4, ty4, tz4, tw4;
	
	//object space ===> homogenous world space
	spMulModellView( x1, y1, z1, &tx1, &ty1, &tz1, &tw1 );
	spMulModellView( x2, y2, z2, &tx2, &ty2, &tz2, &tw2 );
	spMulModellView( x3, y3, z3, &tx3, &ty3, &tz3, &tw3 );
	spMulModellView( x4, y4, z4, &tx4, &ty4, &tz4, &tw4 );

	//homogenous world space ===> w space ==(w-clip)=> device space
	       x1 = spMul( spProjection[ 0], tx1 ) + spMul( spProjection[ 8], tz1 ) + spProjection[12];
	       y1 = spMul( spProjection[ 5], ty1 ) + spMul( spProjection[ 9], tz1 );
	       z1 = spMul( spProjection[10], tz1 ) + spProjection[14];
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;
	Sint32 nx1 = spDiv( x1, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y1, w1 ) >> SP_HALF_ACCURACY;
	
	       x2 = spMul( spProjection[ 0], tx2 ) + spMul( spProjection[ 8], tz2 ) + spProjection[12];
	       y2 = spMul( spProjection[ 5], ty2 ) + spMul( spProjection[ 9], tz2 );
	       z2 = spMul( spProjection[10], tz2 ) + spProjection[14];
	Sint32 w2 = spMul( spProjection[11], tz2 );
	if ( w2 == 0 )
		w2 = 1;
	Sint32 nx2 = spDiv( x2, w2 ) >> SP_HALF_ACCURACY;
	Sint32 ny2 = spDiv( y2, w2 ) >> SP_HALF_ACCURACY;

	       x3 = spMul( spProjection[ 0], tx3 ) + spMul( spProjection[ 8], tz3 ) + spProjection[12];
	       y3 = spMul( spProjection[ 5], ty3 ) + spMul( spProjection[ 9], tz3 );
	       z3 = spMul( spProjection[10], tz3 ) + spProjection[14];
	Sint32 w3 = spMul( spProjection[11], tz3 );
	if ( w3 == 0 )
		w3 = 1;
	Sint32 nx3 = spDiv( x3, w3 ) >> SP_HALF_ACCURACY;
	Sint32 ny3 = spDiv( y3, w3 ) >> SP_HALF_ACCURACY;

	       x4 = spMul( spProjection[ 0], tx4 ) + spMul( spProjection[ 8], tz4 ) + spProjection[12];
	       y4 = spMul( spProjection[ 5], ty4 ) + spMul( spProjection[ 9], tz4 );
	       z4 = spMul( spProjection[10], tz4 ) + spProjection[14];
	Sint32 w4 = spMul( spProjection[11], tz4 );
	if ( w4 == 0 )
		w4 = 1;
	Sint32 nx4 = spDiv( x4, w4 ) >> SP_HALF_ACCURACY;
	Sint32 ny4 = spDiv( y4, w4 ) >> SP_HALF_ACCURACY;
	
	if (spUsePerspective)
		return
			spPerspectiveQuad_tex( viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
														 viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z1, u1, v1, w1,
														 viewPortX + ( ( nx2 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
														 viewPortY - ( ( ny2 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z2, u2, v2, w2, 
														 viewPortX + ( ( nx3 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
														 viewPortY - ( ( ny3 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z3, u3, v3, w3, 
														 viewPortX + ( ( nx4 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
														 viewPortY - ( ( ny4 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z4, u4, v4, w4, rendererLightCalculation( color, tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3 ) );
	return
		spQuad_tex( viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
					viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z1, u1, v1,
					viewPortX + ( ( nx2 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
					viewPortY - ( ( ny2 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z2, u2, v2,
					viewPortX + ( ( nx3 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
					viewPortY - ( ( ny3 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z3, u3, v3,
					viewPortX + ( ( nx4 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
					viewPortY - ( ( ny4 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z4, u4, v4, rendererLightCalculation( color, tx1, ty1, tz1, tx2, ty2, tz2, tx3, ty3, tz3 ) );
}

PREFIX void spBlit3D( Sint32 x1, Sint32 y1, Sint32 z1, SDL_Surface* surface )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );
	Sint32 tx1, ty1, tz1, tw1;
	spMulModellView( x1, y1, z1, &tx1, &ty1, &tz1, &tw1 );

	x1 = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8],tz1) + spProjection[12];
	y1 = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9],tz1);
	z1 = spMul( spProjection[10], tz1 ) + spProjection[14];
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;

	Sint32 nx1 = spDiv( x1, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y1, w1 ) >> SP_HALF_ACCURACY;

	spBlitSurface( viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
				   viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
				   z1,surface );

}

PREFIX int spMesh3D( spModelPointer mesh, int updateEdgeList )
{
	int count = 0;
	Sint32 windowX = spGetRenderTarget()->w;
	Sint32 windowY = spGetRenderTarget()->h;
	Sint32 viewPortX = ( windowX >> 1 );
	Sint32 viewPortY = ( windowY >> 1 );
	//Project Points
	int i;
	for ( i = 0; i < mesh->pointCount; i++ )
	{
		Sint32 tw;
		spMulModellView( mesh->point[i].x, mesh->point[i].y, mesh->point[i].z, &( mesh->point[i].tx ), &( mesh->point[i].ty ), &( mesh->point[i].tz ), &tw );
		Sint32 x1 = spMul( spProjection[ 0], mesh->point[i].tx ) + spMul(spProjection[ 8],mesh->point[i].tz) + spProjection[12];
		Sint32 y1 = spMul( spProjection[ 5], mesh->point[i].ty ) + spMul(spProjection[ 9],mesh->point[i].tz);
		Sint32 z1 = spMul( spProjection[10], mesh->point[i].tz ) + spProjection[14];
		Sint32 w1 = spMul( spProjection[11], mesh->point[i].tz );
		if ( w1 == 0 )
			w1 = 1;
		Sint32 nx1 = spDivHigh( x1, w1 );
		Sint32 ny1 = spDivHigh( y1, w1 );
		mesh->point[i].px = viewPortX + ( ( nx1 * windowX ) >> SP_ACCURACY + 1 );
		mesh->point[i].py = viewPortY - ( ( ny1 * windowY ) >> SP_ACCURACY + 1 );
		mesh->point[i].pz = z1;
	}
	for ( i = 0; i < mesh->texPointCount; i++ )
	{
		Sint32 tw;
		spMulModellView( mesh->texPoint[i].x, mesh->texPoint[i].y, mesh->texPoint[i].z, &( mesh->texPoint[i].tx ), &( mesh->texPoint[i].ty ), &( mesh->texPoint[i].tz ), &tw );
		Sint32 x1 = spMul( spProjection[ 0], mesh->texPoint[i].tx ) + spMul(spProjection[ 8],mesh->texPoint[i].tz) + spProjection[12];
		Sint32 y1 = spMul( spProjection[ 5], mesh->texPoint[i].ty ) + spMul(spProjection[ 9],mesh->texPoint[i].tz);
		Sint32 z1 = spMul( spProjection[10], mesh->texPoint[i].tz ) + spProjection[14];
		mesh->texPoint[i].w = spMul( spProjection[11], mesh->texPoint[i].tz );
		if ( mesh->texPoint[i].w == 0 )
			mesh->texPoint[i].w = 1;
		Sint32 nx1 = spDivHigh( x1, mesh->texPoint[i].w );
		Sint32 ny1 = spDivHigh( y1, mesh->texPoint[i].w );
		mesh->texPoint[i].px = viewPortX + ( ( nx1 * windowX ) >> SP_ACCURACY + 1 );
		mesh->texPoint[i].py = viewPortY - ( ( ny1 * windowY ) >> SP_ACCURACY + 1 );
		mesh->texPoint[i].pz = z1;
	}
	//Project Normals
	if (spPreNormal)
	{
		for ( i = 0; i < mesh->triangleCount; i++ )
			spMulModellView3x3( mesh->triangle[i].normal, mesh->triangle[i].pNormal );
		for ( i = 0; i < mesh->quadCount; i++ )
			spMulModellView3x3( mesh->quad[i].normal, mesh->quad[i].pNormal );
		for ( i = 0; i < mesh->texTriangleCount; i++ )
			spMulModellView3x3( mesh->texTriangle[i].normal, mesh->texTriangle[i].pNormal );
		for ( i = 0; i < mesh->texQuadCount; i++ )
			spMulModellView3x3( mesh->texQuad[i].normal, mesh->texQuad[i].pNormal );
	}
	
	Uint16 lightColor;
	//Draw Faces, if seeable
	for ( i = 0; i < mesh->triangleCount; i++ )
	{
		if (spPreNormal)
			lightColor = rendererLightCalculationKnowNormal( mesh->color,
				mesh->point[mesh->triangle[i].point[0]].tx,
				mesh->point[mesh->triangle[i].point[0]].ty,
				mesh->point[mesh->triangle[i].point[0]].tz,
				mesh->point[mesh->triangle[i].point[1]].tx,
				mesh->point[mesh->triangle[i].point[1]].ty,
				mesh->point[mesh->triangle[i].point[1]].tz,
				mesh->point[mesh->triangle[i].point[2]].tx,
				mesh->point[mesh->triangle[i].point[2]].ty,
				mesh->point[mesh->triangle[i].point[2]].tz,
				mesh->triangle[i].pNormal );
		else
			lightColor = rendererLightCalculation( mesh->color,
				mesh->point[mesh->triangle[i].point[0]].tx,
				mesh->point[mesh->triangle[i].point[0]].ty,
				mesh->point[mesh->triangle[i].point[0]].tz,
				mesh->point[mesh->triangle[i].point[1]].tx,
				mesh->point[mesh->triangle[i].point[1]].ty,
				mesh->point[mesh->triangle[i].point[1]].tz,
				mesh->point[mesh->triangle[i].point[2]].tx,
				mesh->point[mesh->triangle[i].point[2]].ty,
				mesh->point[mesh->triangle[i].point[2]].tz );		
		count += ( 0 < ( mesh->triangle[i].was_drawn =
							 spTriangle( mesh->point[mesh->triangle[i].point[0]].px,
										 mesh->point[mesh->triangle[i].point[0]].py,
										 mesh->point[mesh->triangle[i].point[0]].pz,
										 mesh->point[mesh->triangle[i].point[1]].px,
										 mesh->point[mesh->triangle[i].point[1]].py,
										 mesh->point[mesh->triangle[i].point[1]].pz,
										 mesh->point[mesh->triangle[i].point[2]].px,
										 mesh->point[mesh->triangle[i].point[2]].py,
										 mesh->point[mesh->triangle[i].point[2]].pz,
										 lightColor ) ) );
	}
	for ( i = 0; i < mesh->quadCount; i++ )
	{
		if (spPreNormal)
			lightColor = rendererLightCalculationKnowNormal( mesh->color,
				mesh->point[mesh->quad[i].point[0]].tx,
				mesh->point[mesh->quad[i].point[0]].ty,
				mesh->point[mesh->quad[i].point[0]].tz,
				mesh->point[mesh->quad[i].point[1]].tx,
				mesh->point[mesh->quad[i].point[1]].ty,
				mesh->point[mesh->quad[i].point[1]].tz,
				mesh->point[mesh->quad[i].point[2]].tx,
				mesh->point[mesh->quad[i].point[2]].ty,
				mesh->point[mesh->quad[i].point[2]].tz,
				mesh->quad[i].pNormal );
		else
			lightColor = rendererLightCalculation( mesh->color,
				mesh->point[mesh->quad[i].point[0]].tx,
				mesh->point[mesh->quad[i].point[0]].ty,
				mesh->point[mesh->quad[i].point[0]].tz,
				mesh->point[mesh->quad[i].point[1]].tx,
				mesh->point[mesh->quad[i].point[1]].ty,
				mesh->point[mesh->quad[i].point[1]].tz,
				mesh->point[mesh->quad[i].point[2]].tx,
				mesh->point[mesh->quad[i].point[2]].ty,
				mesh->point[mesh->quad[i].point[2]].tz );
		count += ( 0 < ( mesh->quad[i].was_drawn =
							 spQuad( mesh->point[mesh->quad[i].point[0]].px,
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
									 mesh->point[mesh->quad[i].point[3]].pz,
									 lightColor ) ) );
	}
	if ( mesh->texQuadCount + mesh->texTriangleCount > 0 )
		spBindTexture( mesh->texture );
	for ( i = 0; i < mesh->texTriangleCount; i++ )
	{
		if (spPreNormal)
			lightColor = rendererLightCalculationKnowNormal( mesh->color,
				mesh->texPoint[mesh->texTriangle[i].point[0]].tx,
				mesh->texPoint[mesh->texTriangle[i].point[0]].ty,
				mesh->texPoint[mesh->texTriangle[i].point[0]].tz,
				mesh->texPoint[mesh->texTriangle[i].point[1]].tx,
				mesh->texPoint[mesh->texTriangle[i].point[1]].ty,
				mesh->texPoint[mesh->texTriangle[i].point[1]].tz,
				mesh->texPoint[mesh->texTriangle[i].point[2]].tx,
				mesh->texPoint[mesh->texTriangle[i].point[2]].ty,
				mesh->texPoint[mesh->texTriangle[i].point[2]].tz,
				mesh->texTriangle[i].pNormal );
		else
			lightColor = rendererLightCalculation( mesh->color,
				mesh->texPoint[mesh->texTriangle[i].point[0]].tx,
				mesh->texPoint[mesh->texTriangle[i].point[0]].ty,
				mesh->texPoint[mesh->texTriangle[i].point[0]].tz,
				mesh->texPoint[mesh->texTriangle[i].point[1]].tx,
				mesh->texPoint[mesh->texTriangle[i].point[1]].ty,
				mesh->texPoint[mesh->texTriangle[i].point[1]].tz,
				mesh->texPoint[mesh->texTriangle[i].point[2]].tx,
				mesh->texPoint[mesh->texTriangle[i].point[2]].ty,
				mesh->texPoint[mesh->texTriangle[i].point[2]].tz );
		if (spUsePerspective)
			count += ( 0 < ( mesh->texTriangle[i].was_drawn =
								 spPerspectiveTriangle_tex( mesh->texPoint[mesh->texTriangle[i].point[0]].px,
												 mesh->texPoint[mesh->texTriangle[i].point[0]].py,
												 mesh->texPoint[mesh->texTriangle[i].point[0]].pz,
												 mesh->texPoint[mesh->texTriangle[i].point[0]].u,
												 mesh->texPoint[mesh->texTriangle[i].point[0]].v,
												 mesh->texPoint[mesh->texTriangle[i].point[0]].w,
												 mesh->texPoint[mesh->texTriangle[i].point[1]].px,
												 mesh->texPoint[mesh->texTriangle[i].point[1]].py,
												 mesh->texPoint[mesh->texTriangle[i].point[1]].pz,
												 mesh->texPoint[mesh->texTriangle[i].point[1]].u,
												 mesh->texPoint[mesh->texTriangle[i].point[1]].v,
												 mesh->texPoint[mesh->texTriangle[i].point[1]].w,
												 mesh->texPoint[mesh->texTriangle[i].point[2]].px,
												 mesh->texPoint[mesh->texTriangle[i].point[2]].py,
												 mesh->texPoint[mesh->texTriangle[i].point[2]].pz,
												 mesh->texPoint[mesh->texTriangle[i].point[2]].u,
												 mesh->texPoint[mesh->texTriangle[i].point[2]].v,
												 mesh->texPoint[mesh->texTriangle[i].point[2]].w,
												 lightColor ) ) );
		else
			count += ( 0 < ( mesh->texTriangle[i].was_drawn =
								 spTriangle_tex( mesh->texPoint[mesh->texTriangle[i].point[0]].px,
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
												 mesh->texPoint[mesh->texTriangle[i].point[2]].v,
												 lightColor ) ) );		
	}
	for ( i = 0; i < mesh->texQuadCount; i++ )
	{
		if (spPreNormal)
			lightColor = rendererLightCalculationKnowNormal( mesh->color,
				mesh->texPoint[mesh->texQuad[i].point[0]].tx,
				mesh->texPoint[mesh->texQuad[i].point[0]].ty,
				mesh->texPoint[mesh->texQuad[i].point[0]].tz,
				mesh->texPoint[mesh->texQuad[i].point[1]].tx,
				mesh->texPoint[mesh->texQuad[i].point[1]].ty,
				mesh->texPoint[mesh->texQuad[i].point[1]].tz,
				mesh->texPoint[mesh->texQuad[i].point[2]].tx,
				mesh->texPoint[mesh->texQuad[i].point[2]].ty,
				mesh->texPoint[mesh->texQuad[i].point[2]].tz,
				mesh->texQuad[i].pNormal );
		else
			lightColor = rendererLightCalculation( mesh->color,
				mesh->texPoint[mesh->texQuad[i].point[0]].tx,
				mesh->texPoint[mesh->texQuad[i].point[0]].ty,
				mesh->texPoint[mesh->texQuad[i].point[0]].tz,
				mesh->texPoint[mesh->texQuad[i].point[1]].tx,
				mesh->texPoint[mesh->texQuad[i].point[1]].ty,
				mesh->texPoint[mesh->texQuad[i].point[1]].tz,
				mesh->texPoint[mesh->texQuad[i].point[2]].tx,
				mesh->texPoint[mesh->texQuad[i].point[2]].ty,
				mesh->texPoint[mesh->texQuad[i].point[2]].tz );
		if (spUsePerspective)
			count += ( 0 < ( mesh->texQuad[i].was_drawn =
								 spPerspectiveQuad_tex( mesh->texPoint[mesh->texQuad[i].point[0]].px,
											 mesh->texPoint[mesh->texQuad[i].point[0]].py,
											 mesh->texPoint[mesh->texQuad[i].point[0]].pz,
											 mesh->texPoint[mesh->texQuad[i].point[0]].u,
											 mesh->texPoint[mesh->texQuad[i].point[0]].v,
											 mesh->texPoint[mesh->texQuad[i].point[0]].w,
											 mesh->texPoint[mesh->texQuad[i].point[1]].px,
											 mesh->texPoint[mesh->texQuad[i].point[1]].py,
											 mesh->texPoint[mesh->texQuad[i].point[1]].pz,
											 mesh->texPoint[mesh->texQuad[i].point[1]].u,
											 mesh->texPoint[mesh->texQuad[i].point[1]].v,
											 mesh->texPoint[mesh->texQuad[i].point[1]].w,
											 mesh->texPoint[mesh->texQuad[i].point[2]].px,
											 mesh->texPoint[mesh->texQuad[i].point[2]].py,
											 mesh->texPoint[mesh->texQuad[i].point[2]].pz,
											 mesh->texPoint[mesh->texQuad[i].point[2]].u,
											 mesh->texPoint[mesh->texQuad[i].point[2]].v,
											 mesh->texPoint[mesh->texQuad[i].point[2]].w,
											 mesh->texPoint[mesh->texQuad[i].point[3]].px,
											 mesh->texPoint[mesh->texQuad[i].point[3]].py,
											 mesh->texPoint[mesh->texQuad[i].point[3]].pz,
											 mesh->texPoint[mesh->texQuad[i].point[3]].u,
											 mesh->texPoint[mesh->texQuad[i].point[3]].v,
											 mesh->texPoint[mesh->texQuad[i].point[3]].w,
											 lightColor ) ) );
		else
			count += ( 0 < ( mesh->texQuad[i].was_drawn =
								 spQuad_tex( mesh->texPoint[mesh->texQuad[i].point[0]].px,
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
											 mesh->texPoint[mesh->texQuad[i].point[3]].v,
											 lightColor ) ) );		
	}
	if ( updateEdgeList )
	{
		for ( i = 0; i < mesh->edgeCount; i++ )
			mesh->edge[i].status = -1;
		for ( i = 0; i < mesh->texEdgeCount; i++ )
			mesh->texEdge[i].status = -1;
		for ( i = 0; i < mesh->triangleCount; i++ )
		{
			mesh->edge[mesh->triangle[i].edge[0]].status += mesh->triangle[i].was_drawn;
			mesh->edge[mesh->triangle[i].edge[1]].status += mesh->triangle[i].was_drawn;
			mesh->edge[mesh->triangle[i].edge[2]].status += mesh->triangle[i].was_drawn;
		}
		for ( i = 0; i < mesh->texTriangleCount; i++ )
		{
			mesh->texEdge[mesh->texTriangle[i].edge[0]].status += mesh->texTriangle[i].was_drawn;
			mesh->texEdge[mesh->texTriangle[i].edge[1]].status += mesh->texTriangle[i].was_drawn;
			mesh->texEdge[mesh->texTriangle[i].edge[2]].status += mesh->texTriangle[i].was_drawn;
		}
		for ( i = 0; i < mesh->quadCount; i++ )
		{
			mesh->edge[mesh->quad[i].edge[0]].status += mesh->quad[i].was_drawn;
			mesh->edge[mesh->quad[i].edge[1]].status += mesh->quad[i].was_drawn;
			mesh->edge[mesh->quad[i].edge[2]].status += mesh->quad[i].was_drawn;
			mesh->edge[mesh->quad[i].edge[3]].status += mesh->quad[i].was_drawn;
		}
		for ( i = 0; i < mesh->texQuadCount; i++ )
		{
			mesh->texEdge[mesh->texQuad[i].edge[0]].status += mesh->texQuad[i].was_drawn;
			mesh->texEdge[mesh->texQuad[i].edge[1]].status += mesh->texQuad[i].was_drawn;
			mesh->texEdge[mesh->texQuad[i].edge[2]].status += mesh->texQuad[i].was_drawn;
			mesh->texEdge[mesh->texQuad[i].edge[3]].status += mesh->texQuad[i].was_drawn;
		}
	}
	return count;
}

PREFIX int spMesh3DwithPos(Sint32 x,Sint32 y,Sint32 z, spModelPointer mesh, int updateEdgeList )
{
	spTranslate(x,y,z);
	int count = spMesh3D(mesh,updateEdgeList);
	spTranslate(-x,-y,-z);
	return count;
}

PREFIX void spLine3D( Sint32 x1, Sint32 y1, Sint32 z1,
					  Sint32 x2, Sint32 y2, Sint32 z2, Uint16 color )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	Sint32 tx2, ty2, tz2, tw2;
	spMulModellView( x1, y1, z1, &tx1, &ty1, &tz1, &tw1 );
	spMulModellView( x2, y2, z2, &tx2, &ty2, &tz2, &tw2 );

	x1 = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8], tz1) + spProjection[12];
	y1 = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9], tz1);
	z1 = spMul( spProjection[10], tz1 ) + spProjection[14];
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;

	Sint32 nx1 = spDiv( x1, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y1, w1 ) >> SP_HALF_ACCURACY;

	x2 = spMul( spProjection[ 0], tx2 ) + spMul(spProjection[ 8], tz2) + spProjection[12];
	y2 = spMul( spProjection[ 5], ty2 ) + spMul(spProjection[ 9], tz2);
	z2 = spMul( spProjection[10], tz2 ) + spProjection[14];
	Sint32 w2 = spMul( spProjection[11], tz2 );
	if ( w2 == 0 )
		w2 = 1;

	Sint32 nx2 = spDiv( x2, w2 ) >> SP_HALF_ACCURACY;
	Sint32 ny2 = spDiv( y2, w2 ) >> SP_HALF_ACCURACY;

	spLine( viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
			viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z1,
			viewPortX + ( ( nx2 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ),
			viewPortY - ( ( ny2 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY ), z2, color );
}

PREFIX void spSetLight( int value )
{
	if ( spLightOn == -1 )
	{
		//One default light is on
		spLightDiffuse[0].active = 1;
		spLightDiffuse[0].r = 7 << SP_ACCURACY - 3;
		spLightDiffuse[0].g = 7 << SP_ACCURACY - 3;
		spLightDiffuse[0].b = 7 << SP_ACCURACY - 3;
		spLightDiffuse[0].x = 0 << SP_ACCURACY;
		spLightDiffuse[0].y = 0 << SP_ACCURACY;
		spLightDiffuse[0].z = 0 << SP_ACCURACY;
		spLightDiffuse[0].tx = 0 << SP_ACCURACY;
		spLightDiffuse[0].ty = 0 << SP_ACCURACY;
		spLightDiffuse[0].tz = 0 << SP_ACCURACY;
		memset( &( spLightDiffuse[1] ), 0, sizeof( spLight ) * ( SP_MAX_LIGHTS - 1 ) );
	}
	spLightOn = value != 0;
}

PREFIX void spEnableLight( int number, Sint32 active )
{
	if ( number < 0 || number >= SP_MAX_LIGHTS )
		return;
	spLightDiffuse[number].active = active;
}

PREFIX void spSetLightColor( int number, Uint32 r, Uint32 g, Uint32 b )
{
	if ( number < 0 || number >= SP_MAX_LIGHTS )
		return;
	spLightDiffuse[number].r = r;
	spLightDiffuse[number].g = g;
	spLightDiffuse[number].b = b;
}

PREFIX void spSetLightPosition( int number, Sint32 x, Sint32 y, Sint32 z )
{
	if ( number < 0 || number >= SP_MAX_LIGHTS )
		return;
	spLightDiffuse[number].x = x;
	spLightDiffuse[number].y = y;
	spLightDiffuse[number].z = z;
	spUpdateLight(number);
}

PREFIX void spSetAmbientLightColor( Uint32 r, Uint32 g, Uint32 b )
{
	spLightAmbient[0] = r;
	spLightAmbient[1] = g;
	spLightAmbient[2] = b;
}

PREFIX void spRectangle3D( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Uint16 color )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	spMulModellView( x, y, z, &tx1, &ty1, &tz1, &tw1 );
	x = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8], tz1) + spProjection[12];
	y = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9], tz1);
	z = spMul( spProjection[10], tz1 ) + spProjection[14];
	w = spMul( spProjection[ 0], w );
	h = spMul( spProjection[ 5], h );
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;
	Sint32 nx1 = spDiv( x, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nw1 = spDiv( w, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nh1 = spDiv( h, w1 ) >> SP_HALF_ACCURACY;

	x = viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	y = viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	w = ( nw1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;
	h = ( nh1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;

	spRectangle( x, y, z, w, h, color );
}

PREFIX void spEllipse3D( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Uint16 color )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	spMulModellView( x, y, z, &tx1, &ty1, &tz1, &tw1 );
	x = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8], tz1) + spProjection[12];
	y = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9], tz1);
	z = spMul( spProjection[10], tz1 ) + spProjection[14];
	rx = spMul( spProjection[ 0], rx );
	ry = spMul( spProjection[ 5], ry );
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;
	Sint32 nx1 = spDiv( x, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nrx1 = spDiv( rx, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nry1 = spDiv( ry, w1 ) >> SP_HALF_ACCURACY;

	x = viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	y = viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	rx = ( nrx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;
	ry = ( nry1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;

	spEllipse( x, y, z, rx, ry, color );
}

PREFIX void spRectangleBorder3D( Sint32 x, Sint32 y, Sint32 z, Sint32 w, Sint32 h, Sint32 bx, Sint32 by, Uint16 color )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	spMulModellView( x, y, z, &tx1, &ty1, &tz1, &tw1 );
	x = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8], tz1) + spProjection[12];
	y = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9], tz1);
	z = spMul( spProjection[10], tz1 ) + spProjection[14];
	w = spMul( spProjection[ 0], w );
	h = spMul( spProjection[ 5], h );
	bx = spMul( spProjection[ 0], bx );
	by = spMul( spProjection[ 5], by );
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;
	Sint32 nx1 = spDiv( x, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nw1 = spDiv( w, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nh1 = spDiv( h, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nbx1 = spDiv( bx, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nby1 = spDiv( by, w1 ) >> SP_HALF_ACCURACY;

	x = viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	y = viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	w = ( nw1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;
	h = ( nh1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;
	bx = ( nbx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;
	by = ( nby1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;

	spRectangleBorder( x, y, z, w, h, bx, by, color );
}

PREFIX void spEllipseBorder3D( Sint32 x, Sint32 y, Sint32 z, Sint32 rx, Sint32 ry, Sint32 bx, Sint32 by, Uint16 color )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	spMulModellView( x, y, z, &tx1, &ty1, &tz1, &tw1 );
	x = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8], tz1) + spProjection[12];
	y = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9], tz1);
	z = spMul( spProjection[10], tz1 ) + spProjection[14];
	rx = spMul( spProjection[ 0], rx );
	ry = spMul( spProjection[ 5], ry );
	bx = spMul( spProjection[ 0], bx );
	by = spMul( spProjection[ 5], by );
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;
	Sint32 nx1 = spDiv( x, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nrx1 = spDiv( rx, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nry1 = spDiv( ry, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nbx1 = spDiv( bx, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nby1 = spDiv( by, w1 ) >> SP_HALF_ACCURACY;

	x = viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	y = viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	rx = ( nrx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;
	ry = ( nry1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;
	bx = ( nbx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;
	by = ( nby1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY;

	spEllipseBorder( x, y, z, rx, ry, bx, by, color );
}

PREFIX void spProjectPoint3D( Sint32 x, Sint32 y, Sint32 z, Sint32 *px, Sint32 *py, Sint32 *pz, Sint32 *w,Sint32 withModelview )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	spMulModellView( x, y, z, &tx1, &ty1, &tz1, &tw1 );
	if ( withModelview )
	{
		x = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8], tz1) + spProjection[12];
		y = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9], tz1);
		z = spMul( spProjection[10], tz1 ) + spProjection[14];
	}
	else
	{
		x = spMul( spProjection[ 0], x ) + spMul(spProjection[ 8], tz1) + spProjection[12];
		y = spMul( spProjection[ 5], y ) + spMul(spProjection[ 9], tz1);
		z = spMul( spProjection[10], tz1 ) + spProjection[14];
	}
	*w = spMul( spProjection[11], tz1 );
	if ( *w == 0 )
		*w = 1;
	Sint32 nx1 = spDiv( x, *w ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y, *w ) >> SP_HALF_ACCURACY;


	if ( withModelview )
	{
		*px = viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
		*py = viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
		*pz = z;
	}
	else
	{
		*px = ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
		*py = ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
		*pz = z;
	}
}

PREFIX void spRotozoomSurface3D( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 zoomX, Sint32 zoomY, Sint32 angle )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	spMulModellView( x, y, z, &tx1, &ty1, &tz1, &tw1 );
	x = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8], tz1) + spProjection[12];
	y = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9], tz1);
	z = spMul( spProjection[10], tz1 ) + spProjection[14];
	zoomX = spMul( spProjection[ 0], zoomX ) + spMul(spProjection[ 8], tz1) + spProjection[12];
	zoomY = spMul( spProjection[ 5], zoomY ) + spMul(spProjection[ 9], tz1);
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;
	Sint32 nx1 = spDiv( x, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nzoomX1 = spDiv( zoomX, w1 );
	Sint32 nzoomY1 = spDiv( zoomY, w1 );

	x = viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	y = viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	zoomX = nzoomX1 * ( windowX >> 1 ) / surface->w;
	zoomY = nzoomY1 * ( windowY >> 1 ) / surface->h;
	spRotozoomSurface( x, y, z, surface, zoomX, zoomY, angle );
}

PREFIX void spRotozoomSurfacePart3D( Sint32 x, Sint32 y, Sint32 z, SDL_Surface* surface, Sint32 sx, Sint32 sy, Sint32 w, Sint32 h, Sint32 zoomX, Sint32 zoomY, Sint32 angle )
{
	int windowX = spGetRenderTarget()->w;
	int windowY = spGetRenderTarget()->h;
	int viewPortX = ( windowX >> 1 );
	int viewPortY = ( windowY >> 1 );

	Sint32 tx1, ty1, tz1, tw1;
	spMulModellView( x, y, z, &tx1, &ty1, &tz1, &tw1 );
	x = spMul( spProjection[ 0], tx1 ) + spMul(spProjection[ 8], tz1) + spProjection[12];
	y = spMul( spProjection[ 5], ty1 ) + spMul(spProjection[ 9], tz1);
	z = spMul( spProjection[10], tz1 ) + spProjection[14];
	zoomX = spMul( spProjection[ 0], zoomX ) + spMul(spProjection[ 8], tz1) + spProjection[12];
	zoomY = spMul( spProjection[ 5], zoomY ) + spMul(spProjection[ 9], tz1);
	Sint32 w1 = spMul( spProjection[11], tz1 );
	if ( w1 == 0 )
		w1 = 1;
	Sint32 nx1 = spDiv( x, w1 ) >> SP_HALF_ACCURACY;
	Sint32 ny1 = spDiv( y, w1 ) >> SP_HALF_ACCURACY;
	Sint32 nzoomX1 = spDiv( zoomX, w1 );
	Sint32 nzoomY1 = spDiv( zoomY, w1 );

	x = viewPortX + ( ( nx1 * ( windowX << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	y = viewPortY - ( ( ny1 * ( windowY << SP_HALF_ACCURACY - 1 ) ) >> SP_ACCURACY );
	zoomX = nzoomX1 * ( windowX >> 1 ) / surface->w;
	zoomY = nzoomY1 * ( windowY >> 1 ) / surface->h;
	spRotozoomSurfacePart( x, y, z, surface, sx, sy, w, h, zoomX, zoomY, angle );
}

PREFIX void spUpdateLight(int number)
{
	if ( number < 0 || number >= SP_MAX_LIGHTS )
		return;
	Sint32 tw_dummy;
	spMulModellView( spLightDiffuse[number].x, spLightDiffuse[number].y, spLightDiffuse[number].z, &(spLightDiffuse[number].tx), &(spLightDiffuse[number].ty), &(spLightDiffuse[number].tz), &tw_dummy );
}

PREFIX void spUsePrecalculatedNormals(int value)
{
	spPreNormal = value;
}

PREFIX void spSetPerspectiveTextureMapping(int value)
{
	spUsePerspective = value;
}
