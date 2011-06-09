#ifndef _MESHLOADER_H
#define _MESHLOADER_H
#include "3dengine.h"

typedef struct smesh *pmesh;
typedef struct smesh {
  int vcount;
  ppoint verticies;
  int qcount;
  pquad quads;
  int tcount;
  ptriangle triangles;
} tmesh;

tpoint readOBJVString(char* string);

tquad readOBJQString(char* string);

ttriangle readOBJTString(char* string);

pmesh loadMesh(char* filename);

void drawMesh(pmesh mesh,Uint16 color);

void drawMeshXYZ(Sint32 x,Sint32 y,Sint32 z,pmesh mesh,Uint16 color);

void drawMeshXYZS(Sint32 x,Sint32 y,Sint32 z,Sint32 s,pmesh mesh,Uint16 color);

void freeMesh(pmesh mesh);

#endif
