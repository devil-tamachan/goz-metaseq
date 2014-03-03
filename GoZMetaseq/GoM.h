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

  void ImportGoZ(MQDocument doc, MYCALLBACKOPT *opt)
  {
    FILE *fpList = fopen(pathGoZ_ObjectList, "r");
    if(!fpList)return;

    Cleanup4Import(doc, opt);

    char objPath[1024+4+2];
    while(fgets(objPath, 1024, fpList))
    {
      CStringA objNameOrig = PathFindFileNameA(objPath);
      CStringA objName;
      doc->GetUnusedObjectName(objName.GetBuffer(256+2), 256, objNameOrig);
      objName.ReleaseBuffer();
      MQObject obj = MQ_CreateObject();
     // obj->SetName(objName);
      CStringA gozPath = objPath;
      gozPath.TrimRight();
      gozPath += ".GoZ";
      ATLTRACE(gozPath);
      if(SUCCEEDED(ImportObj(doc, obj, gozPath, opt)))doc->AddObject(obj);
      else obj->DeleteThis();
    }
    fclose(fpList);
    doc->Compact();
    UpdateUndo();
    RedrawAllScene();
  }

  void AllClearObj(MQDocument doc)
  {
    int numObj = doc->GetObjectCount();
    for(int i=0; i<numObj; i++)
    {
      if(doc->GetObject(i))doc->DeleteObject(i);
    }
  }
  void AllClearMat(MQDocument doc)
  {
    int numMat = doc->GetMaterialCount();
    for(int i=0;i<numMat;i++)
    {
      if(doc->GetMaterial(i))doc->DeleteMaterial(i);
    }
  }
  void Cleanup4Import(MQDocument doc, MYCALLBACKOPT *opt)
  {
    AllClearObj(doc);
    if(opt->mergeMat)AllClearMat(doc);
  }

#define FPOS(pos)  { if(fseek(fp, pos, SEEK_SET))goto IOB_Err1; }
#define FSKIP(pos) { if(fseek(fp, pos, SEEK_CUR))goto IOB_Err1; }
#define FSKIP4()   { if(fseek(fp, 4,   SEEK_CUR))goto IOB_Err1; }
#define FSKIP8()   { if(fseek(fp, 8,   SEEK_CUR))goto IOB_Err1; }
  
#define READB(ret) { if(fread(&ucTmp, 1, 1, fp)!=1)goto IOB_Err1; \
    ret = ucTmp; }
#define READUL(ret) { if(fread(&ulTmp, 4, 1, fp)!=1)goto IOB_Err1; \
    ret = ulTmp; }
#define READULL(ret) { if(fread(&ullTmp, 8, 1, fp)!=1)goto IOB_Err1; \
    ret = ullTmp; }
#define READF(ret) { if(fread(&fTmp, 4, 1, fp)!=1)goto IOB_Err1; \
    ret = fTmp; }

#define READUV(coord) { READF(coord.u);\
  READF(coord.v); }


  HRESULT ImportObj(MQDocument doc, MQObject obj, LPCSTR gozPath, MYCALLBACKOPT *opt)
  {
    FILE *fp = fopen(gozPath, "rb");
    if(!fp)return E_FAIL;
    FPOS(36);

    unsigned char ucTmp;
    unsigned int ulTmp;
    unsigned long long ullTmp;
    float fTmp;

    unsigned int lenUtf8Name = 0;

    READUL(lenUtf8Name);
    lenUtf8Name -= 16;
    FSKIP8();

    {
      CStringA utf8Name;
      if(fread(utf8Name.GetBuffer(lenUtf8Name), lenUtf8Name, 1, fp)!=1)goto IOB_Err1;
      utf8Name.ReleaseBuffer(lenUtf8Name);
      CString objName = CA2W(utf8Name, CP_UTF8);
      objName = objName.Right(objName.GetLength()-8);
      obj->SetName(CW2A(objName));
      ATLTRACE(_T("Import: ")+objName+_T("\n"));
    }

    unsigned int ulTag = 0;
    READUL(ulTag);

    while(ulTag)
    {
#ifdef _DEBUG
      CString dbg;
      dbg.Format(_T("Import tag: 0x%08X\n"), ulTag);
      ATLTRACE(dbg);
#endif
      switch(ulTag)
      {
      case 0x00001389: //Skip
        {
          unsigned long s = 0;
          READUL(s);//ChunkSize
          s-=8;
          FSKIP(s);
          break;
        }
      case 0x00002711:
        {
          FSKIP4();//ChunkSize
          unsigned __int64 numVertex = 0;
          READULL(numVertex);
          MQPoint pt;
          for(unsigned __int64 i=0; i < numVertex; i++)
          {
            if(i>0x5FFFffff)goto IOB_Err1;
            READF(pt.x);pt.x *= 100.0f;
            READF(pt.y);pt.y *= -100.0f;
            READF(pt.z);pt.z *= -100.0f;
            obj->AddVertex(pt);
          }
          break;
        }
      case 0x00004E21:
        {
          FSKIP4();//ChunkSize
          unsigned __int64 numFace = 0;
          READULL(numFace);
          unsigned int idx[4];
          for(unsigned __int64 i=0; i < numFace; i++)
          {
            if(i>0x5FFFffff)goto IOB_Err1;
            READUL(idx[3]);
            READUL(idx[2]);
            READUL(idx[1]);
            READUL(idx[0]);
            if(idx[0]==0xFFFFffff)obj->AddFace(3, (int *)&idx[1]);
            else obj->AddFace(4, (int *)idx);
          }
          break;
        }
      case 0x000061A9:
        {
          FSKIP4();//ChunkSize
          unsigned __int64 numUV = 0;
          READULL(numUV);
          MQCoordinate uvs[4];
          for(unsigned __int64 i=0; i < numUV; i++)
          {
            if(i>0x5FFFffff)goto IOB_Err1;
            READUV(uvs[0]);
            READUV(uvs[1]);
            READUV(uvs[2]);
            READUV(uvs[3]);
            obj->SetFaceCoordinateArray((int)i, uvs);
          }
          break;
        }
      case 0x000088b9:
        {
          FSKIP4();//ChunkSize
          unsigned __int64 numVColor = 0;
          READULL(numVColor);
          unsigned int numVert = obj->GetVertexCount();
          ATLASSERT(numVert==numVColor);
          CAtlArray<DWORD> arrVColor;
          //arrVColor.SetCount(numVert);
          DWORD rgba;
          for(unsigned __int64 i=0; i < numVert; i++)
          {
            if(i>0x5FFFffff)goto IOB_Err1;
            DWORD r,g,b;
            READB(b);
            b<<=16;
            READB(g);
            g<<=8;
            READB(r);
            FSKIP(1);
            rgba = 0xFF000000 | r | g | b;
            arrVColor.Add(rgba);
          }
          ATLASSERT(arrVColor.GetCount()==numVert);
          for(int i=0; i<obj->GetFaceCount(); i++)
          {
            int vCount = obj->GetFacePointCount(i);
            if(vCount >= 3)
            {
              int *vertexIndex = new int[vCount];
              obj->GetFacePointArray(i, vertexIndex);
              for(int j=0; j<vCount; j++)
              {
                obj->SetFaceVertexColor(i, j, rgba);
              }
              delete [] vertexIndex;
            }
          }
          break;
        }
      //case 0x00007532:
      //case 0x00009C41:
      //case 0:
      default:
        {
          unsigned int s=0;
          READUL(s);
          s-=8;
          if(s<0)goto IOB_Err1;
          FSKIP(s);
          break;
        }
      }
      READUL(ulTag);
    }
    /*
    WRITEUL(0x00009C41);
    WRITEUL(numFace*2+16);
    WRITEULL(numFace);
    WritePolygroup(fp, doc, obj);

    for(int i=0; i<16; i++) fputc(0, fp);
    */

    fclose(fp);
    return S_OK;

IOB_Err1:
    fclose(fp);
    MessageBox(NULL, _T(".GoZ Read Error"), _T("Error"), MB_OK);
    return E_FAIL;
  }

  void ExportGoZ(MQDocument doc, MYCALLBACKOPT *opt)
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
      if(FAILED(ExportObj(doc, obj, strName, opt)))return;
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

  HRESULT ExportObj(MQDocument doc, MQObject obj, CString strName, MYCALLBACKOPT *opt)
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
      int vCount = obj->GetFacePointCount(i);
      if(vCount >= 3)
      {
        int numRect = (vCount-2)/2;
        int numTri = (vCount-2)%2;
        int numFace = numRect + numTri;
        int iMat = obj->GetFaceMaterial(i);
        if(iMat==-1)
        {
          for(int j=0;j<numFace;j++)WRITEW(NOMAT);
        } else {
          MQMaterial mat = doc->GetMaterial(iMat);
          for(int j=0;j<numFace;j++)WRITEW(CalcCRC16(mat));
        }
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
    MYCALLBACKOPT *opt = (MYCALLBACKOPT *)option;
    if(opt->bImport)ImportGoZ(doc, opt);
    else ExportGoZ(doc, opt);
    free(opt);
    return true;
  }
};
