/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/

#include "test_mesh.h"
#include <sparrow3d.h>
#include <strings.h>
SDL_Surface *mesh_texture;
spModelPointer mesh;
int mesh_z = 1;
int mesh_light = 1;

void init_mesh(char* parameter1,char* parameter2)
{
	if (parameter2)
		mesh_texture = spLoadSurface( parameter2 );
	else
		mesh_texture = spLoadSurface( "./data/garfield.png" );
	if (parameter1)
		mesh = spMeshLoadObj( parameter1, mesh_texture, 65535 );
	else
		mesh = spMeshLoadObj( "./data/testmeshuv_tri.obj", mesh_texture, 65535 );
}

char* caption_mesh(char* caption)
{
	sprintf(caption,"Mesh Loading");
	return caption;
}

char* settings_mesh(char* caption,int button)
{
	switch (button)
	{
		case SP_BUTTON_A:
			if (mesh_z)
				sprintf(caption,"[A] Z Set & Test");
			else
				sprintf(caption,"[A] No Z Set & Test");
			break;
		case SP_BUTTON_B:
			if (mesh_light)
				sprintf(caption,"[B] Light on");
			else
				sprintf(caption,"[B] Light off");
			break;
	}
	return caption;
}

void draw_mesh(int rotation)
{
	spClearTarget(0);
	spSetZSet( mesh_z );
	spSetZTest( mesh_z );
	spSetLight( mesh_light );
	spSetAlphaTest( 0 );
	spTranslate( 0, 0, spFloatToFixed( -6.0f )+spSin(rotation)*6 );
	spRotateX( rotation );
	spRotateY( rotation );
	spRotateZ( rotation );
	
	int i;
	spSetLineWidth( 3 );
	spMesh3D( mesh, 2 );
	//spSetZSet( 0 );
	//spSetZTest( 0 );
	for (i = 0; i < mesh->edgeCount; i++)
	{
		if (mesh->edge[i].status == 0)
			spLine3D(mesh->point[mesh->edge[i].point[0]].x,
			         mesh->point[mesh->edge[i].point[0]].y,
			         mesh->point[mesh->edge[i].point[0]].z,
			         mesh->point[mesh->edge[i].point[1]].x,
			         mesh->point[mesh->edge[i].point[1]].y,
			         mesh->point[mesh->edge[i].point[1]].z,spGetFastRGB(255,255,0));
	}
	for (i = 0; i < mesh->texEdgeCount; i++)
	{
		if (mesh->texEdge[i].status == 0)
			spLine3D(mesh->texPoint[mesh->texEdge[i].point[0]].x,
			         mesh->texPoint[mesh->texEdge[i].point[0]].y,
			         mesh->texPoint[mesh->texEdge[i].point[0]].z,
			         mesh->texPoint[mesh->texEdge[i].point[1]].x,
			         mesh->texPoint[mesh->texEdge[i].point[1]].y,
			         mesh->texPoint[mesh->texEdge[i].point[1]].z,spGetFastRGB(255,255,0));
	}
	spSetLineWidth(1);
	spSetZSet( 1 );
	spSetZTest( 1 );
	spSetLight( 1 );
}

void calc_mesh()
{
	if ( spGetInput()->button[SP_BUTTON_A] )
	{
		spGetInput()->button[SP_BUTTON_A] = 0;
		mesh_z = 1-mesh_z;
	}
	if ( spGetInput()->button[SP_BUTTON_B] )
	{
		spGetInput()->button[SP_BUTTON_B] = 0;
		mesh_light = 1-mesh_light;
	}
}

void quit_mesh()
{
	spDeleteSurface(mesh_texture);
	spMeshDelete(mesh);
}
