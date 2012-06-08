/* $Id: plugin.h,v 1.2 1999/12/29 19:38:12 enorman Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#ifndef PLUGIN_H
#define PLUGIN_H

#define INCL_PM
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>
#include "psMultimedia.h"
#include "psBitmap.h"
#include "pluginUtils.h"

#define PLUGIN_SHARE "\\SHAREMEM\\BRDRIMG"  // shared mem
#define LEFT    0
#define RIGHT   1
#define BOTTOM  2
#define TOP     3

#define DLGID_PLUGIN             8100
#define PBID_OK                  8101
#define PBID_CANCEL              8102

#define GBID_IMAGEPROPERTIES      8110
#define CBID_ACTIVEENABLED              8111
#define CBID_INACTIVEENABLED            8112

#define STID_LEFTIMAGELABEL      8120
#define EFID_LEFTIMAGE           8121
#define PBID_LEFTCHANGEIMAGE     8122
#define SPBID_LEFTTOP            8123
#define SPBID_LEFTBOTTOM         8124

#define STID_RIGHTIMAGELABEL     8130
#define EFID_RIGHTIMAGE          8131
#define PBID_RIGHTCHANGEIMAGE    8132
#define SPBID_RIGHTTOP           8133
#define SPBID_RIGHTBOTTOM        8134

#define STID_TOPIMAGELABEL       8140
#define EFID_TOPIMAGE            8141
#define PBID_TOPCHANGEIMAGE      8142
#define SPBID_TOPLEFT            8143
#define SPBID_TOPRIGHT           8144

#define STID_BOTTOMIMAGELABEL    8150
#define EFID_BOTTOMIMAGE         8151
#define PBID_BOTTOMCHANGEIMAGE   8152
#define SPBID_BOTTOMLEFT         8153
#define SPBID_BOTTOMRIGHT        8154


typedef struct
{
    char szLeftFile[CCHMAXPATH];
    char szRightFile[CCHMAXPATH];
    char szTopFile[CCHMAXPATH];
    char szBottomFile[CCHMAXPATH];

    short sLeftTopOffset;
    short sLeftBottomOffset;
    short sRightTopOffset;
    short sRightBottomOffset;
    short sTopLeftOffset;
    short sTopRightOffset;
    short sBottomLeftOffset;
    short sBottomRightOffset;

    HBITMAP hbmLeft;
    HBITMAP hbmRight;
    HBITMAP hbmTop;
    HBITMAP hbmBottom;

    BOOL bActiveEnabled;
    BOOL bInactiveEnabled;
}
PLUGINSHARE;

// this was used with cache...
typedef struct
{
    BITMAPINFOHEADER2 bif2;
    SIZEL slHPS;
    HDC hdc;
    HBITMAP hbm;
    HPS hps;
    RECTL rectl;
    float fHorzScale;
    float fVertScale;
    long lScreenWidth;
    long lScreenHeight;
    MMFORMATINFO mmFormat;
    MMIOINFO mmioInfo;
    HMMIO hmmio;
    MMIMAGEHEADER mmImgHdr;
    ULONG cbRow;
    ULONG cbData;
    void *pvImage;
}
PSTBFILE;

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pInfo);
//called to allocate memory and setup initial settings
BOOL CBZInit(HINI hIni, char szName[], char szClass[], PVOID * pData);
//change current settings.
BOOL CBZApply(char szClass[], KEY_VALUE * kv, int count, int enabledState, PVOID pData);
//save current settings to the given ini file
BOOL CBZSave(HINI hIni, char szClass[], PVOID pData, char szCustomName[]);
//Cleanup allocated Resources.
BOOL CBZDestroy(HWND hwnd, PVOID pData);
//Draw the Plugin effect into the given presentation space
BOOL CBZPluginRender(PSPAINT * pPaint, PVOID pData, short sActive);
//Called before the default wnd procedure if you wish to provide additional
//  functionality.  (i.e. modify what happens with mouse clicks, drag/drop, etc...
BOOL CBZPluginWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2,
                        PVOID pData, PFNWP pfnOrigWndProc, MRESULT *rc);
//A Dlg procedure if the plugin has selectable settings.
MRESULT EXPENTRY CBZPluginSetupDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
BOOL CBZWriteAttributes(char *retStr, PVOID pData);

HBITMAP SetupBitmapFromFile(HWND hwnd, char *pszFileName, long lVertical);
BOOL DrawFile(PSPAINT * pPaint, HBITMAP hbm, short sSide, short leftOffset, short rightOffset, short topOffset, short bottomOffset, BOOL bActive);

#endif //PLUGIN_H
