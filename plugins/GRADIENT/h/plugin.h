/* $Id: plugin.h,v 1.4 2000/04/15 15:02:00 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#ifndef PLUGIN_H
#define PLUGIN_H

#define INCL_PM
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>
#include "pluginUtils.h"

#define PLUGIN_SHARE "\\SHAREMEM\\GRADIENT"  // shared mem

#define DLGID_PLUGIN                   8100
#define PBID_OK                        8101
#define PBID_CANCEL                    8102
#define GBID_ACTIVEGRADIENTPROPERTIES  8103
#define CBID_ACTIVEENABLED             8104
#define PBID_ACTIVETOP                 8105
#define PBID_ACTIVEBOTTOM              8106
#define CBID_ACTIVEHORIZ               8107

#define GBID_INACTIVEGRADIENTPROPERTIES  8110
#define CBID_INACTIVEENABLED             8111
#define PBID_INACTIVETOP                 8112
#define PBID_INACTIVEBOTTOM              8113
#define CBID_INACTIVEHORIZ               8114

typedef struct
{
    long lActiveTopColor;
    long lActiveBottomColor;
    long lInactiveTopColor;
    long lInactiveBottomColor;
    BOOL bActiveHorizontal;
    BOOL bInactiveHorizontal;

    BOOL bActiveEnabled;
    BOOL bInactiveEnabled;
    HBITMAP hbmActive;
    HBITMAP hbmInactive;
}
PLUGINSHARE;


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

//helper functions for CBZPluginRender
HBITMAP SetupBitmap(HWND hwnd, long lColorTop, long lColorBottom, long lX, long lY);
BOOL TileGradient(PSPAINT * pPaint, HBITMAP hbm);

#endif //PLUGIN_H
