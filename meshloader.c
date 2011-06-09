#include "meshloader.h"

tpoint readOBJVString(char* string)
{
  int i;
  float number[3];
  int pos=0;
  for (i=0;i<3;i++)
  {
    //Vorzeichen?
    float sign=1;
    if (string[pos]=='-')
    {
      sign=-1;
      pos++;
    }
    //while (string[pos]==' ')
    //  pos++;
    number[i]=0;
    //bis zum Punkt
    while (string[pos]!='.')
    {
      number[i]=number[i]*10.0+(float)(string[pos]-'0');
      pos++;
    }
    pos++;
    //bis zum Leerzeichen oder e
    float aftercomma=0.0;
    float dividend=1;
    while (string[pos]!=' ' && string[pos]!='e' && string[pos]!=0)
    {
      aftercomma=aftercomma*10.0+(float)(string[pos]-'0');
      dividend*=10.0;
      pos++;
    }
    float exponent=0;
    if (string[pos]=='e')
    {
      pos++;
      float sign;
      if (string[pos]=='-')
      {
        sign=-1;
        pos++;
      }
      else
        sign=1;
      while (string[pos]!=' ' && string[pos]!=0)
      {
        exponent=exponent*10.0+(float)(string[pos]-'0');
        pos++;
      }
      exponent*=sign;
    }
    if (exponent>-2.0)
      number[i]=sign*(number[i]+aftercomma/dividend)*pow(10,exponent);
    else
      number[i]=0;
    while (string[pos]!=' ' && string[pos]!=0)
      pos++;
    pos++;
  }
  tpoint result;
  result.x=(Sint32)(number[0]*ACCURACY_FACTOR);
  result.y=(Sint32)(number[1]*ACCURACY_FACTOR);
  result.z=(Sint32)(number[2]*ACCURACY_FACTOR);
  return result;
}

tquad readOBJQString(char* string)
{
  int i;
  tquad result;
  int pos=0;
  for (i=0;i<4;i++)
  {
    result.p[i]=0;
    //bis zum ersten Slash
    while (string[pos]!='/')
    {
      result.p[i]=result.p[i]*10+(float)(string[pos]-'0');
      pos++;
    }
    result.p[i]--;
    pos++;
    //bis zum nächsten Leerzeichen springen
    while (string[pos]!=' ' && string[pos]!=0)
      pos++;
    pos++;
  }
  return result;
}

ttriangle readOBJTString(char* string)
{
  int i;
  ttriangle result;
  int pos=0;
  for (i=0;i<3;i++)
  {
    result.p[i]=0;
    //bis zum ersten Slash
    while (string[pos]!='/')
    {
      result.p[i]=result.p[i]*10+(float)(string[pos]-'0');
      pos++;
    }
    result.p[i]--;
    pos++;
    //bis zum nächsten Leerzeichen springen
    while (string[pos]!=' ' && string[pos]!=0)
      pos++;
    pos++;
  }
  return result;
}

pmesh loadMesh(char* filename)
{
  SDL_RWops *file=SDL_RWFromFile(filename,"rb");
  //"v" am Zeilenanfang zählen
  char sign;
  char oldsign=0;
  pmesh mesh = (pmesh)malloc(sizeof(tmesh));
  mesh->vcount=0;
  while (SDL_RWread(file,&sign,1,1)>0)
  {
    if (sign=='v' && oldsign=='\n')
    { 
      SDL_RWread(file,&sign,1,1);
      if (sign==' ')
        mesh->vcount++;
    }
    oldsign=sign;
  }
  mesh->verticies=(ppoint)malloc(sizeof(tpoint)*mesh->vcount);
  
  //"Quads" und "Triangles" zählen
  SDL_RWseek(file,0,SEEK_SET);
  oldsign=0;
  mesh->qcount=0;
  mesh->tcount=0;
  while (SDL_RWread(file,&sign,1,1)>0)
  {
    if (sign=='f' && oldsign=='\n')
    { 
      SDL_RWread(file,&sign,1,1);
      if (sign==' ')
      { 
        //Solange die / zählen bis ein Zeilenumbruch kommt.
        int c=0;
        while (SDL_RWread(file,&sign,1,1)>0)
        {
          if (sign=='/')
            c++;
          oldsign=sign;
          if (sign=='\n')
            break;
        }
        if ((c>>1)==4) //Quads
          mesh->qcount++;
        if ((c>>1)==3) //Triangles
          mesh->tcount++;
      }
        mesh->vcount++;
    }
    oldsign=sign;
  }
  mesh->verticies=(ppoint)malloc(sizeof(tpoint)*mesh->vcount);
  if (mesh->tcount==0)
    mesh->triangles=NULL;
  else
    mesh->triangles=(ptriangle)malloc(sizeof(ttriangle)*mesh->tcount);
  if (mesh->qcount==0)
    mesh->quads=NULL;
  else
    mesh->quads=(pquad)malloc(sizeof(tquad)*mesh->qcount);

  //"verticies" auslesen
  SDL_RWseek(file,0,SEEK_SET);
  oldsign=0;
  int count=0;
  while (SDL_RWread(file,&sign,1,1)>0)
  {
    if (sign=='v' && oldsign=='\n')
    { 
      SDL_RWread(file,&sign,1,1);
      if (sign==' ')
      {
        char buffer[256];
        int pos=0;
        //Reading String
        while (SDL_RWread(file,&sign,1,1)>0)
        {
          if (sign!='\n')
          {
            buffer[pos]=sign;
            pos++; 
          }
          oldsign=sign;
          if (sign=='\n')
          {
            buffer[pos]=0;
            break;
          }
        }
        //Using String
        mesh->verticies[count]=readOBJVString(buffer);
        count++;
      }
    }
    oldsign=sign;
  }

  //"quads" and "triangles" auslesen
  SDL_RWseek(file,0,SEEK_SET);
  oldsign=0;
  int qcount=0;
  int tcount=0;
  while (SDL_RWread(file,&sign,1,1)>0)
  {
    if (sign=='f' && oldsign=='\n')
    { 
      SDL_RWread(file,&sign,1,1);
      if (sign==' ')
      {
        char buffer[256];
        int pos=0;
        int c=0;
        //Reading String
        while (SDL_RWread(file,&sign,1,1)>0)
        {
          if (sign=='/')
            c++;
          if (sign!='\n')
          {
            buffer[pos]=sign;
            pos++; 
          }
          oldsign=sign;
          if (sign=='\n')
          {
            buffer[pos]=0;
            break;
          }
        }
        //Using String
        if ((c>>1)==4) //Quad 
        {
          mesh->quads[qcount]=readOBJQString(buffer);
          qcount++;
        }
        if ((c>>1)==3) //Triangle
        {
          mesh->triangles[tcount]=readOBJTString(buffer);
          tcount++;
        }
      }
    }
    oldsign=sign;
  }

  SDL_RWclose(file);
  return mesh;
}

void drawMesh(pmesh mesh,Uint16 color)
{
  engineList(mesh->verticies,mesh->vcount,mesh->quads,mesh->qcount,mesh->triangles,mesh->tcount,color);
}

void drawMeshXYZ(Sint32 x,Sint32 y,Sint32 z,pmesh mesh,Uint16 color)
{
  engineListXYZ(x,y,z,mesh->verticies,mesh->vcount,mesh->quads,mesh->qcount,mesh->triangles,mesh->tcount,color);
}

void drawMeshXYZS(Sint32 x,Sint32 y,Sint32 z,Sint32 s,pmesh mesh,Uint16 color)
{
  engineListXYZS(x,y,z,s,mesh->verticies,mesh->vcount,mesh->quads,mesh->qcount,mesh->triangles,mesh->tcount,color);
}

void freeMesh(pmesh mesh)
{
  free(mesh->triangles);
  free(mesh->quads);
  free(mesh->verticies);
  free(mesh);
}

