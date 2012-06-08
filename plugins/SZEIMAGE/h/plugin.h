/* $Id: plugin.h,v 1.3 2000/09/09 11:01:31 cwohlgemuth Exp $ */

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

#define PLUGIN_SHARE "\\SHAREMEM\\SZEIMAGE"  // shared mem

#define DLGID_PLUGIN             8100
#define PBID_OK                  8101
#define PBID_CANCEL              8102

#define GBID_ACTIVEIMAGEPROPERTIES 8110
#define CBID_ACTIVEENABLED         8111
#define STID_ACTIVEIMAGELABEL      8112
#define STID_ACTIVEIMAGE           8113
#define PBID_CHANGEACTIVEIMAGE     8114
#define SPBID_ACTIVETOP              8115
#define SPBID_ACTIVEBOTTOM           8116
#define SPBID_ACTIVELEFT             8117
#define SPBID_ACTIVERIGHT            8118

#define GBID_INACTIVEIMAGEPROPERTIES 8120
#define CBID_INACTIVEENABLED         8121
#define STID_INACTIVEIMAGELABEL      8122
#define STID_INACTIVEIMAGE           8123
#define PBID_CHANGEINACTIVEIMAGE     8124
#define SPBID_INACTIVETOP              8125
#define SPBID_INACTIVEBOTTOM           8126
#define SPBID_INACTIVELEFT             8127
#define SPBID_INACTIVERIGHT            8128

#define STID_ACTIVEIMAGEMASK        8104
#define PBID_CHANGEACTIVEIMAGEMASK  8105
#define STID_INACTIVEIMAGEMASK      8107
#define PBID_CHANGEINACTIVEIMAGEMASK 8108

typedef struct
{
    char szActiveFile[CCHMAXPATH];
    char szActiveFileMask[CCHMAXPATH];
    char szInactiveFile[CCHMAXPATH];
    char szInactiveFileMask[CCHMAXPATH];

    short sActiveTopOffset;
    short sActiveBottomOffset;
    short sActiveLeftOffset;
    short sActiveRightOffset;

    short sInactiveTopOffset;
    short sInactiveBottomOffset;
    short sInactiveLeftOffset;
    short sInactiveRightOffset;

    HBITMAP hbmActive;
    HBITMAP hbmActiveMask;
    HBITMAP hbmInactive;
    HBITMAP hbmInactiveMask;

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
BOOL DrawFile(PSPAINT * pPaint, HBITMAP hbm, HBITMAP hbmMask, short topOffset, short bottomOffset, short leftOffset, short rightOffset);
#endif //PLUGIN_H
