/*
 The contents of this file are subject to the Mozilla Public License        
 Version 1.1 (the "License"); you may not use this file except in           
 compliance with the License. You may obtain a copy of the License at       
 http://www.mozilla.org/MPL/                                                
                                                                            
 Software distributed under the License is distributed on an "AS IS"        
 basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the    
 License for the specific language governing rights and limitations         
 under the License.                                                         
                                                                            
 Alternatively, the contents of this file may be used under the rms       
 of the GNU Lesser General Public license (the  "LGPL License"), in which case the  
 provisions of LGPL License are applicable instead of those                  
 above.                                                                     
                                                                            
 For feedback and questions about my Files and Projects please mail me,     
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com                         
*/
#include "sparrowMesh.h"
#include <math.h>

int meshReadLine(char* buffer,SDL_RWops *file,int max) //Returns 0 at EOF, otherwise amount of letters
{
  char sign;
  int pos = 0;
  while (SDL_RWread(file,&sign,1,1)>0 && pos<max)
  {
    buffer[pos] = sign;
    pos++;
    if (sign=='\n')
      break;
  }
  buffer[pos]=0;
  return pos;
}

typedef struct spMeshTempStruct *spMeshTempPointer;
typedef struct spMeshTempStruct {
  Uint32 point;
  Uint32 uv;
  Uint32 nr;
  spMeshTempPointer next;
} spMeshTemp;

Uint32 meshGetNumber(spMeshTempPointer* first,Uint32 point,Uint32 uv)
{
  spMeshTempPointer mom = *first;
  Uint32 nr = -1;
  while (mom != NULL)
  {
    if (mom->point == point && mom->uv == uv)
      return mom->nr; //found!
    mom = mom->next;
  }
  //not forund:
  mom = (spMeshTempPointer)malloc(sizeof(spMeshTemp));
  mom->point = point;
  mom->uv = uv;
  if (*first)
  {
    mom->nr = (*first)->nr+1;
    mom->next = (*first);
    (*first) = mom;
    return mom->nr;
  }
  (*first) = mom;
  mom->next = NULL;
  mom->nr = 0;
  return mom->nr;
}

float meshatof(char* buffer)
{
  double number = 0.0f;
  double sign = 1.0f;
  int pos = 0;
  //crap at the beginning
  while (buffer[pos]!=0 && buffer[pos]<'0' && buffer[pos]>'9' && buffer[pos]!='+' && buffer[pos]!='-')
    pos++;
  if (buffer[pos]==0)
    return number;
  if (buffer[pos]=='+')
    pos++;
  else
  if (buffer[pos]=='-')
  {
    pos++;
    sign = -1.0f;
  }
  //the number itself
  while (buffer[pos]>='0' && buffer[pos]<='9')
  {
    number = number*10.0f + (double)(buffer[pos]-'0');
    pos++;
  }
  if (buffer[pos]!='.')
    return sign*number;
  //after the comma
  pos++;
  double divisor = 1.0f;
  double aftercomma = 0.0f;
  while (buffer[pos]>='0' && buffer[pos]<='9')
  {
    divisor*=10.0f;
    aftercomma = aftercomma + (double)(buffer[pos]-'0')/divisor;
    pos++;
  }
  if (buffer[pos]!='e' && buffer[pos]!='E')
    return sign*(number+aftercomma);
  //after e
  pos++;
  double e = 1.0f;
  int esign = 1;
  if (buffer[pos]=='+')
    pos++;
  else
  if (buffer[pos]=='-')
  {
    pos++;
    esign = 0;
  }
  while (buffer[pos]>='0' && buffer[pos]<='9')
  {
    e = e*10.0f + (double)(buffer[pos]-'0');
    pos++;
  }
  if (esign)
    return sign*(number+aftercomma)*pow(10.0f,e);
  return sign*(number+aftercomma)/pow(10.0f,e);
}

void meshParseVertex(char* buffer,spPointPointer point,int max)
{
  point->x = 0;
  point->y = 0;
  point->z = 0;
  //search ' ' after 'v'
  int left = 0;
  while (left < max && buffer[left]!=' ')
    left++;
  //now buffer[left] is the ' ' after 'v' before the first number
  left++;
  int right=left+1;
  while (right < max && buffer[right]!=' ')
    right++; 
  //now buffer[right] is the ' ' after the number
  char oldc = buffer[right];
  buffer[right] = 0;
  float number = meshatof(&(buffer[left]));
  point->x = (int)(number*SP_ACCURACY_FACTOR);
  buffer[right]=oldc;
  
  //second number
  left = right;
  while (left < max && buffer[left]!=' ')
    left++;
  left++;
  right=left+1;
  while (right < max && buffer[right]!=' ')
    right++; 
  oldc = buffer[right];
  buffer[right] = 0;
  number = meshatof(&(buffer[left]));
  point->y = (int)(number*SP_ACCURACY_FACTOR);
  buffer[right]=oldc;

  //third number
  left = right;
  while (left < max && buffer[left]!=' ')
    left++;
  left++;
  right=left+1;
  while (right < max && buffer[right]>' ')
    right++; 
  oldc = buffer[right];
  buffer[right] = 0;
  number = meshatof(&(buffer[left]));
  point->z = (int)(number*SP_ACCURACY_FACTOR);
  buffer[right]=oldc;
}

void meshParseUV(char* buffer,spTexPointPointer point,int max,int texw,int texh)
{
  point->u = 0;
  point->v = 0;
  //search ' ' after 't'
  int left = 0;
  while (left < max && buffer[left]!=' ')
    left++;
  //now buffer[left] is the ' ' after 't' before the first number
  left++;
  int right=left+1;
  while (right < max && buffer[right]!=' ')
    right++; 
  //now buffer[right] is the ' ' after the number
  char oldc = buffer[right];
  buffer[right] = 0;
  float number = meshatof(&(buffer[left]));
  point->u = (int)(number*(float)texw);
  buffer[right]=oldc;
  
  //second number
  left = right;
  while (left < max && buffer[left]!=' ')
    left++;
  left++;
  right=left+1;
  while (right < max && buffer[right]!=' ')
    right++; 
  oldc = buffer[right];
  buffer[right] = 0;
  number = meshatof(&(buffer[left]));
  point->v = (int)(number*(float)texh);
  buffer[right]=oldc;
}

int meshParseFace(char* buffer,int* face,int max) //3 triangle, 4 quad
{
  int count = 0;
  //search ' ' after 'f'
  int left = 0;
  while (left < max && buffer[left]!=' ')
    left++;
  //now buffer[left] is the ' ' after 'f' before the first number
  while (1)
  {
    left++;
    int right=left+1;
    while (right < max && buffer[right]!='/')
      right++; 
    //now buffer[right] is the '/' after the number
    char oldc = buffer[right];
    buffer[right] = 0;
    face[count*3+0] = atoi(&(buffer[left]));
    buffer[right]=oldc;
    //second number?
    if (buffer[right+1]!='/')
    {
      left = right+1;
      right=left+1;
      while (right < max && buffer[right]!='/')
        right++; 
      //now buffer[right] is the '/' after the number
      oldc = buffer[right];
      buffer[right] = 0;
      face[count*3+1] = atoi(&(buffer[left]));
      buffer[right]=oldc;
      left = right+1;
    }
    else
    {
      face[count*3+1] = -1;
      left = right+2;
    }
    right=left+1;
    while (right < max && buffer[right]!=' ' && buffer[right]!='\n')
      right++; 
    //now buffer[right] is the sign after the number
    oldc = buffer[right];
    buffer[right] = 0;
    face[count*3+2] = atoi(&(buffer[left]));
    buffer[right]=oldc;
    left = right;
    count++;
    if (buffer[left]=='\n' || buffer[left]==0 || left>=max)
      return count;
  }
}

PREFIX spModelPointer spMeshLoadObj(char* name,SDL_Surface* texture,Uint16 color)
{
    char buffer[256];
    //Counting
    SDL_RWops *file=SDL_RWFromFile(name,"rb");
    if (file == NULL)
      return NULL;
    int vertexCount = 0,uvCount = 0,triCount = 0, triTexCount = 0,quadCount = 0, quadTexCount = 0;
    while (meshReadLine(buffer,file,255))
    {
      if (buffer[0]=='v' && buffer[1]==' ')
        vertexCount++;
      if (buffer[0]=='v' && buffer[1]=='t')
        uvCount++;
      if (buffer[0]=='f' && buffer[1]==' ')
      {
        // searching"//"
        int count = 0;
        int pos;
        for (pos = 2; pos < 256 && buffer[pos]!=0; pos++)
          if (buffer[pos]=='/')
            count++;
        pos = 2;
        while (pos < 256 && buffer[pos]!=0)
        {
          if (buffer[pos]=='/' && buffer[pos-1]=='/')
            break;
          pos++;
        }            
        if (buffer[pos]=='/')
        {
          if (count > 6)
            quadCount++;
          else
            triCount++;
        }
        else
        {
          if (count > 6)
            quadTexCount++;
          else
            triTexCount++;
        }
      }
    }
    SDL_RWclose(file);
    //Getting memory
    spPointPointer rawPoints = (spPointPointer)malloc(sizeof(spPoint)*vertexCount);
    spTexPointPointer rawUV = NULL;    
    if (uvCount > 0)
      rawUV = (spTexPointPointer)malloc(sizeof(spTexPoint)*uvCount);
      
    spTrianglePointer triangles = NULL;
    if (triCount > 0)
      triangles = (spTrianglePointer)malloc(sizeof(spTriangleS)*triCount);
    spTrianglePointer texTriangles = NULL;
    if (triTexCount > 0)
      texTriangles = (spTrianglePointer)malloc(sizeof(spTriangleS)*triTexCount);
    spQuadPointer quads = NULL;
    if (quadCount > 0)
      quads = (spQuadPointer)malloc(sizeof(spQuadS)*quadCount);
    spQuadPointer texQuads = NULL;
    if (quadTexCount > 0)
      texQuads = (spQuadPointer)malloc(sizeof(spQuadS)*quadTexCount);
      
    printf("Reading \"%s\" with\n",name);
    printf("  %i Triangles without uv\n",triCount);
    printf("  %i Triangles with uv\n",triTexCount);
    printf("  %i Quads without uv\n",quadCount);
    printf("  %i Quads with uv\n",quadTexCount);
      
    
    //Reading
    file=SDL_RWFromFile(name,"rb");
    if (file == NULL)
      return NULL;
    vertexCount = 0;
    uvCount = 0;
    triCount = 0;
    triTexCount = 0;
    quadCount = 0;
    quadTexCount = 0;
    int texw = 256;
    int texh = 256;
    if (texture)
    {
      texw = texture->w;
      texh = texture->h;
    }
    spMeshTempPointer tempPointer = NULL;
    spMeshTempPointer texTempPointer = NULL;
    while (meshReadLine(buffer,file,255))
    {
      //Reading Vertexes and uv coordinates
      if (buffer[0]=='v' && buffer[1]==' ')
      {
        meshParseVertex(buffer,&(rawPoints[vertexCount]),255);
        vertexCount++;
      }
      if (buffer[0]=='v' && buffer[1]=='t')
      {
        meshParseUV(buffer,&(rawUV[uvCount]),255,texw,texh);
        uvCount++;
      }
      if (buffer[0]=='f' && buffer[1]==' ')
      {
        int face[12];
        int count = meshParseFace(buffer,face,255);
        if (count == 3)
        {
          if (face[1]==-1)
          {
            triangles[triCount].point[0] = meshGetNumber(&tempPointer,face[0]-1,face[1]-1);
            triangles[triCount].point[1] = meshGetNumber(&tempPointer,face[3]-1,face[4]-1);
            triangles[triCount].point[2] = meshGetNumber(&tempPointer,face[6]-1,face[7]-1);
            triCount++;
          }
          else
          {
            texTriangles[triTexCount].point[0] = meshGetNumber(&texTempPointer,face[0]-1,face[1]-1);
            texTriangles[triTexCount].point[1] = meshGetNumber(&texTempPointer,face[3]-1,face[4]-1);
            texTriangles[triTexCount].point[2] = meshGetNumber(&texTempPointer,face[6]-1,face[7]-1);
            triTexCount++;
          }
        }
        else
        {
          if (face[1]==-1)
          {
            quads[quadCount].point[0] = meshGetNumber(&tempPointer,face[0]-1,face[1]-1);
            quads[quadCount].point[1] = meshGetNumber(&tempPointer,face[3]-1,face[4]-1);
            quads[quadCount].point[2] = meshGetNumber(&tempPointer,face[6]-1,face[7]-1);
            quads[quadCount].point[3] = meshGetNumber(&tempPointer,face[9]-1,face[10]-1);
            quadCount++;
          }
          else
          {
            texQuads[quadTexCount].point[0] = meshGetNumber(&texTempPointer,face[0]-1,face[1]-1);
            texQuads[quadTexCount].point[1] = meshGetNumber(&texTempPointer,face[3]-1,face[4]-1);
            texQuads[quadTexCount].point[2] = meshGetNumber(&texTempPointer,face[6]-1,face[7]-1);
            texQuads[quadTexCount].point[3] = meshGetNumber(&texTempPointer,face[9]-1,face[10]-1);
            quadTexCount++;
          }
        }
      }
    }
    //Creating Point lists
    vertexCount = 0;
    spPointPointer points = NULL;
    if (tempPointer)
    {
      vertexCount = tempPointer->nr+1;
      points = (spPointPointer)malloc(sizeof(spPoint)*vertexCount);
    }
    uvCount = 0;
    spTexPointPointer texPoints = NULL;    
    if (texTempPointer)
    {
      uvCount = texTempPointer->nr+1;
      texPoints = (spTexPointPointer)malloc(sizeof(spTexPoint)*uvCount);
    }
    while (tempPointer)
    {
      points[tempPointer->nr].x = rawPoints[tempPointer->point].x;
      points[tempPointer->nr].y = rawPoints[tempPointer->point].y;
      points[tempPointer->nr].z = rawPoints[tempPointer->point].z;
      spMeshTempPointer next = tempPointer->next;
      free(tempPointer);
      tempPointer = next;
    }
    while (texTempPointer)
    {
      texPoints[texTempPointer->nr].x = rawPoints[texTempPointer->point].x;
      texPoints[texTempPointer->nr].y = rawPoints[texTempPointer->point].y;
      texPoints[texTempPointer->nr].z = rawPoints[texTempPointer->point].z;
      texPoints[texTempPointer->nr].u = rawUV[texTempPointer->uv].u;
      texPoints[texTempPointer->nr].v = rawUV[texTempPointer->uv].v;
      spMeshTempPointer next = texTempPointer->next;
      free(texTempPointer);
      texTempPointer = next;
    }
    free(rawPoints);
    free(rawUV);
    
    spModelPointer model = (spModelPointer)malloc(sizeof(spModel));
    model->texture = texture;
    model->pointCount = vertexCount;
    model->point = points;
    model->texPointCount = uvCount;
    model->texPoint = texPoints;
    model->triangleCount = triCount;
    model->texTriangleCount = triTexCount;
    model->triangle = triangles;
    model->texTriangle = texTriangles;
    model->quadCount = quadCount;
    model->texQuadCount = quadTexCount;
    model->quad = quads;
    model->texQuad = texQuads;
    model->edgeCount = 0;
    model->texEdgeCount = 0;
    model->edge = NULL;
    model->texEdges = NULL;
    model->color = color;
    SDL_RWclose(file);
    return model;
}

PREFIX void spMeshDelete(spModelPointer mesh)
{
  free(mesh->point);
  free(mesh->texPoint);
  free(mesh->triangle);
  free(mesh->texTriangle);
  free(mesh->quad);
  free(mesh->texQuad);
  free(mesh);
}
