/*
licenced by New BSD License

Copyright (c) 2014, devil.tamachan@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "MainDlg.h"

class CMainDlg;

static const char *pathPixologic = "C:\\Users\\Public\\Pixologic\\";
static const char *pathGoZProjects = "C:\\Users\\Public\\Pixologic\\GoZProjects\\Default\\";
static const char *pathGoZ_ObjectList = "C:\\Users\\Public\\Pixologic\\GoZBrush\\GoZ_ObjectList.txt";
static const TCHAR *pathGoZFromApp = _T("C:\\Users\\Public\\Pixologic\\GoZBrush\\GoZBrushFromApp.exe");

class GoM : public MQCommandPlugin
{
public:
  CMainDlg *m_dlgMain;
  BOOL m_bShowDlgMain;

  GoM()
  {
    m_dlgMain = NULL;
    m_bShowDlgMain = FALSE;
  }
  ~GoM()
  {
    DestroyMainDialog();
  }

  void DestroyMainDialog();

  virtual void GetPlugInID(DWORD *Product, DWORD *ID)
  {
    *Product = 0x0C510C68;
    *ID      = 0xBE4288BF;
  }
  virtual const char* GetPlugInName(void) { return "GoM   Copyright(C) 2014 devil.tamachan@gmail.com"; }
  virtual const char* EnumString(void) { return "GoM"; }
  virtual BOOL Initialize();
  virtual void Exit()
  {
    DestroyMainDialog();
  }
  virtual BOOL Activate(MQDocument doc, BOOL flag);
  virtual BOOL IsActivated(MQDocument doc)
  {
    return (m_dlgMain==NULL) ? FALSE : m_bShowDlgMain;
  }

  void ImportGoZ(MQDocument doc)
  {
  }
  void ExportGoZ(MQDocument doc)
  {
    FILE *fpList = fopen(pathGoZ_ObjectList, "wt");
    if(!fpList)return;

    for(int i=0; i < doc->GetObjectCount(); i++)
    {
      MQObject obj = doc->GetObject(i);
      char chName[201] = {0};
      obj->GetName(chName, 200);
      CString strName = chName;
      strName.Replace('.', '_');
      strName.Replace(',', '_');
      strName.Replace('\\', '_');
      obj->SetName(CW2A(strName));
      if(FAILED(ExportObj(doc, obj, strName)))return;
      if(FAILED(CreateZtn(strName)))return;
      CString pathName = pathGoZProjects + strName;
      fprintf(fpList, CW2A(pathName+_T("\n")));
    }
    fclose(fpList);
    ShellExecute(NULL, _T("open"), pathGoZFromApp, NULL, NULL, SW_SHOWDEFAULT);
    UpdateUndo();
    RedrawAllScene();
  }

  HRESULT CreateZtn(CString strName)
  {
    CString pathName = pathGoZProjects + strName;
    CString pathZtn = pathName + ".ztn";
    FILE *fp = fopen(CW2A(pathZtn), "wt");
    if(!fp)return E_FAIL;
    fprintf(fp, CW2A(pathName));
    fclose(fp);
    return S_OK;
  }

#define WRITEB(val) { ucTmp = val;\
  fwrite(&ucTmp, 1, 1, fp); }

#define WRITEW(val) { usTmp = val;\
  fwrite(&usTmp, 2, 1, fp); }

#define WRITEUL(val) { ulTmp = val;\
  fwrite(&ulTmp, 4, 1, fp); }

#define WRITEULL(val) { ullTmp = val;\
  fwrite(&ullTmp, 8, 1, fp);}

#define WRITEF(val) { fTmp = val;\
  fwrite(&fTmp, 4, 1, fp); }

#define WRITEUV(coord) { WRITEF(coord.u);\
  WRITEF(coord.v); }

#define WRITEUV0() { WRITEF(0.0);\
  WRITEF(0.0); }

  HRESULT ExportObj(MQDocument doc, MQObject obj, CString strName)
  {
    CString pathGoZ = pathGoZProjects + strName + ".GoZ";
    const char *GoZHeader10 = "GoZb 1.0 ZBrush GoZ Binary\x2E\x2E\x2E\x2E\x2E\x2E";
    FILE *fp = fopen(CT2A(pathGoZ), "wb");
    if(!fp)return E_FAIL;
    fwrite(GoZHeader10, strlen(GoZHeader10), 1, fp);

    unsigned int ulTmp;
    unsigned long long ullTmp;

    WRITEUL(1);//objtag

    char utf8Name[400] = {0};
    unsigned int lenUtf8Name = AtlUnicodeToUTF8(strName, -1, utf8Name, 400);

    WRITEUL(lenUtf8Name + 24);
    WRITEULL(1);

    const char *chGoZMesh = "GoZMesh_";
    fwrite(chGoZMesh, strlen(chGoZMesh), 1, fp);

    fwrite(utf8Name, lenUtf8Name, 1, fp);

    WRITEUL(0x00001389);
    WRITEUL(20);
    WRITEULL(1);
    WRITEUL(0);

    WRITEUL(0x00002711);
    ulTmp = obj->GetVertexCount();
    WRITEUL(ulTmp*3*4+16);
    WRITEULL(obj->GetVertexCount());
    WriteVertexArray(fp, obj);

    unsigned int numFace = FaceCount(obj);
    WRITEUL(0x00004E21);
    WRITEUL(numFace*4*4+16);
    WRITEULL(numFace);
    WriteFaces(fp, obj);

    WRITEUL(0x000061A9);
    WRITEUL(numFace*4*4*2+16);
    WRITEULL(numFace);
    WriteUVs(fp, obj);

    WRITEUL(0x000088b9);
    ulTmp = obj->GetVertexCount();
    WRITEUL(ulTmp*4+16);
    WRITEULL(obj->GetVertexCount());
    WriteVColor(fp, obj);
    
    WRITEUL(0x00009C41);
    WRITEUL(numFace*2+16);
    WRITEULL(numFace);
    WritePolygroup(fp, doc, obj);

    for(int i=0; i<16; i++) fputc(0, fp);

    fclose(fp);

    return S_OK;
  }

  WORD CalcCRC16(MQMaterial mat)
  {
    char name[255];
    mat->GetName(name, 254);
    size_t lenName = strlen(name);
    return (WORD)crc1(lenName, (byte *)name);
  }

#define NOMAT 0x2a95
  void WritePolygroup(FILE *fp, MQDocument doc, MQObject obj)
  {
    unsigned short usTmp;
    for(int i=0; i<obj->GetFaceCount(); i++)
    {
      int iMat = obj->GetFaceMaterial(i);
      if(iMat==-1)
      {
        WRITEW(NOMAT);
      } else {
        MQMaterial mat = doc->GetMaterial(iMat);
        WRITEW(CalcCRC16(mat));
      }
    }
  }

#define GETR(rgba) ((BYTE)(rgba & 0xFF))
#define GETG(rgba) ((BYTE)((rgba & 0xFF00)>>8))
#define GETB(rgba) ((BYTE)((rgba & 0xFF0000)>>16))
#define GETA(rgba) ((BYTE)((rgba & 0xFF000000)>>24))

  void WriteVColor(FILE *fp, MQObject obj)
  {
    BYTE ucTmp;
    unsigned int byteCols = obj->GetVertexCount();
    byteCols *= 4;
    BYTE *cols = (BYTE *)malloc(byteCols);
    ZeroMemory(cols, obj->GetVertexCount()*4);
    for(int i=0; i<obj->GetFaceCount(); i++)
    {
      int vCount = obj->GetFacePointCount(i);
      if(vCount >= 3)
      {
        int numRect = (vCount-2)/2;
        int numTri = (vCount-2)%2;
        int *vertexIndex = new int[vCount];
        obj->GetFacePointArray(i, vertexIndex);
        for(int j=0; j<vCount; j++)
        {
          DWORD rgba = obj->GetFaceVertexColor(i, j);
          int vi = vertexIndex[j];
          unsigned int iCols = vi;
          iCols *= 4;
          cols[iCols] = GETB(rgba);
          cols[iCols+1] = GETG(rgba);
          cols[iCols+2] = GETR(rgba);
          cols[iCols+3] = 0;//GETA(rgba); Colorizeの強さ? 10以上だとShaderじゃなくて頂点色がそのまま着色される。255だとまさにフラットシェーダー
        }
        delete [] vertexIndex;
      }
    }
    for(unsigned int k=0; k<byteCols; k++)
    {
      WRITEB(cols[k]);
    }
    free(cols);
  }

  void WriteVertexArray(FILE *fp, MQObject obj)
  {
    float fTmp;

    for(int i=0; i<obj->GetVertexCount(); i++)
    {
      MQPoint pt = obj->GetVertex(i);
      fTmp = pt.x;
      if(fTmp!=0.0f)fTmp*=0.01f;
      WRITEF(fTmp);
      fTmp = pt.y;
      if(fTmp!=0.0f)fTmp*=0.01f;
      WRITEF(fTmp);
      fTmp = pt.z;
      if(fTmp!=0.0f)fTmp*=0.01f;
      WRITEF(fTmp);
    }
  }

  void WriteUVs(FILE *fp, MQObject obj)
  {
    float fTmp;
    for(int i=0; i<obj->GetFaceCount(); i++)
    {
      int vCount = obj->GetFacePointCount(i);
      if(vCount >= 3)
      {
        int numRect = (vCount-2)/2;
        int numTri = (vCount-2)%2;
        MQCoordinate *uvs = new MQCoordinate[vCount];
        obj->GetFaceCoordinateArray(i, uvs);

        int iVertex = 0;
        BOOL bFirst = TRUE;
        for(int j=0;j<numRect;j++)
        {
          if(bFirst)
          {
            WRITEUV(uvs[iVertex+3]);
            WRITEUV(uvs[iVertex+2]);
            WRITEUV(uvs[iVertex+1]);
            WRITEUV(uvs[iVertex]);
            iVertex+=3;
            bFirst = FALSE;
          } else {
            WRITEUV(uvs[iVertex+2]);
            WRITEUV(uvs[iVertex+1]);
            WRITEUV(uvs[iVertex]);
            WRITEUV(uvs[0]);
            iVertex+=2;
          }
        }
        for(int j=0;j<numTri;j++)
        {
          if(bFirst)
          {
            WRITEUV(uvs[iVertex+2]);
            WRITEUV(uvs[iVertex+1]);
            WRITEUV(uvs[iVertex]);
            WRITEUV0();
            iVertex+=2;
            bFirst = FALSE;
          } else {
            WRITEUV(uvs[iVertex+1]);
            WRITEUV(uvs[iVertex]);
            WRITEUV(uvs[0]);
            WRITEUV0();
            iVertex+=1;
          }
        }
        delete [] uvs;
      }
    }
  }

  void WriteFaces(FILE *fp, MQObject obj)
  {
    unsigned int ulTmp;
    for(int i=0; i<obj->GetFaceCount(); i++)
    {
      int vCount = obj->GetFacePointCount(i);
      if(vCount >= 3)
      {
        int numRect = (vCount-2)/2;
        int numTri = (vCount-2)%2;
        int *vertexIndex = new int[vCount];
        obj->GetFacePointArray(i, vertexIndex);

        int iVertex = 0;
        BOOL bFirst = TRUE;
        for(int j=0;j<numRect;j++)
        {
          if(bFirst)
          {
            WRITEUL(vertexIndex[iVertex+3]);
            WRITEUL(vertexIndex[iVertex+2]);
            WRITEUL(vertexIndex[iVertex+1]);
            WRITEUL(vertexIndex[iVertex]);
            iVertex+=3;
            bFirst = FALSE;
          } else {
            WRITEUL(vertexIndex[iVertex+2]);
            WRITEUL(vertexIndex[iVertex+1]);
            WRITEUL(vertexIndex[iVertex]);
            WRITEUL(vertexIndex[0]);
            iVertex+=2;
          }
        }
        for(int j=0;j<numTri;j++)
        {
          if(bFirst)
          {
            WRITEUL(vertexIndex[iVertex+2]);
            WRITEUL(vertexIndex[iVertex+1]);
            WRITEUL(vertexIndex[iVertex]);
            WRITEUL(0xFFFFffff);
            iVertex+=2;
            bFirst = FALSE;
          } else {
            WRITEUL(vertexIndex[iVertex+1]);
            WRITEUL(vertexIndex[iVertex]);
            WRITEUL(vertexIndex[0]);
            WRITEUL(0xFFFFffff);
            iVertex+=1;
          }
        }
        delete [] vertexIndex;
      }
    }
  }

  unsigned int FaceCount(MQObject obj)
  {
    unsigned int numFace = 0;
    for(int i=0; i<obj->GetFaceCount(); i++)
    {
      int vCount = obj->GetFacePointCount(i);
      if(vCount >= 3)
      {
        numFace += (vCount-2)/2;
        numFace += (vCount-2)%2;
      }
    }
    return numFace;
  }

protected:
  virtual bool ExecuteCallback(MQDocument doc, void *option)
  {
    if(option==0)ImportGoZ(doc);
    else ExportGoZ(doc);
    return false;
  }
};
