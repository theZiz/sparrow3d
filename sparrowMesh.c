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
  int right=left+1;
  while (right < max && buffer[right]!=' ')
    right++; 
  //now buffer[right] is the ' ' after the number
  char oldc = buffer[right];
  buffer[right] = 0;
  float number = atof(&(buffer[left]));
  point->x = (int)(number*SP_ACCURACY_FACTOR);
  buffer[right]=oldc;
  
  //second number
  left = right;
  while (left < max && buffer[left]!=' ')
    left++;
  right=left+1;
  while (right < max && buffer[right]!=' ')
    right++; 
  oldc = buffer[right];
  buffer[right] = 0;
  number = atof(&(buffer[left]));
  point->y = (int)(number*SP_ACCURACY_FACTOR);
  buffer[right]=oldc;

  //third number
  left = right;
  while (left < max && buffer[left]!=' ')
    left++;
  right=left+1;
  while (right < max && buffer[right]!=' ')
    right++; 
  oldc = buffer[right];
  buffer[right] = 0;
  number = atof(&(buffer[left]));
  point->z = (int)(number*SP_ACCURACY_FACTOR);
  buffer[right]=oldc;
}

void meshParseUV(char* buffer,spTexPointPointer point,int max)
{
  point->u = 0;
  point->v = 0;
  //search ' ' after 't'
  int left = 0;
  while (left < max && buffer[left]!=' ')
    left++;
  //now buffer[left] is the ' ' after 't' before the first number
  int right=left+1;
  while (right < max && buffer[right]!=' ')
    right++; 
  //now buffer[right] is the ' ' after the number
  char oldc = buffer[right];
  buffer[right] = 0;
  float number = atof(&(buffer[left]));
  point->u = (int)(number*SP_ACCURACY_FACTOR);
  buffer[right]=oldc;
  
  //second number
  left = right;
  while (left < max && buffer[left]!=' ')
    left++;
  right=left+1;
  while (right < max && buffer[right]!=' ')
    right++; 
  oldc = buffer[right];
  buffer[right] = 0;
  number = atof(&(buffer[left]));
  point->v = (int)(number*SP_ACCURACY_FACTOR);
  buffer[right]=oldc;
}

PREFIX spModelPointer spMeshLoadObj(char* name,SDL_Surface* texture)
{
    char buffer[256];
    //Counting
    SDL_RWops *file=SDL_RWFromFile(name,"rb");
    if (file == NULL)
      return NULL;
    int vertexCount = 0,uvCount = 0,faceCount = 0, faceTexCount = 0;
    while (meshReadLine(buffer,file,255))
    {
      if (buffer[0]=='v' && buffer[1]==' ')
        vertexCount++;
      if (buffer[0]=='v' && buffer[1]=='t')
        uvCount++;
      if (buffer[0]=='f' && buffer[1]==' ')
      {
        // searching"//"
        int pos = 2;
        while (pos < 256 && buffer[pos]!=0)
        {
          if (buffer[pos]=='/' && buffer[pos-1]=='/')
            break;
          pos++;
        }
        if (buffer[pos]=='/')
          faceCount++;
        else
          faceTexCount++;
      }
    }
    SDL_RWclose(file);
    //Getting memory
    spPointPointer rawPoints = (spPointPointer)malloc(sizeof(spPoint)*vertexCount);
    spTexPointPointer rawUV = NULL;
    if (uvCount > 0)
      rawUV = (spTexPointPointer)malloc(sizeof(spTexPoint)*uvCount);
    spTrianglePointer triangles = NULL;
    if (faceCount > 0)
      triangles = (spTrianglePointer)malloc(sizeof(spTriangleS)*faceCount);
    spTrianglePointer texTriangles = NULL;
    if (faceTexCount > 0)
      texTriangles = (spTrianglePointer)malloc(sizeof(spTriangleS)*faceTexCount);
      
    
    //Reading
    file=SDL_RWFromFile(name,"rb");
    if (file == NULL)
      return NULL;
    vertexCount = 0;
    uvCount = 0;
    faceCount = 0;
    faceTexCount = 0;
    while (meshReadLine(buffer,file,255))
    {
      if (buffer[0]=='v' && buffer[1]==' ')
      {
        meshParseVertex(buffer,&(rawPoints[vertexCount]),255);
        vertexCount++;
      }
      if (buffer[0]=='v' && buffer[1]=='t')
      {
        meshParseUV(buffer,&(rawUV[uvCount]),255);
        uvCount++;
      }
    }
    SDL_RWclose(file);
}

PREFIX void spMeshDelete(spModelPointer mesh)
{
  
}
