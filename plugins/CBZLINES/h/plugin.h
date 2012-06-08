/* $Id: plugin.h,v 1.3 2000/08/29 18:09:12 cwohlgemuth Exp $ */

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

#define DLGID_PLUGIN                   8100
#define PBID_OK                        8101
#define PBID_CANCEL                    8102
#define GBID_ACTIVELINEPROPERTIES      8104
#define CBID_ACTIVEENABLED             8105
#define PBID_ACTIVELINECOLOR           8106
#define PBID_ACTIVESHADOWCOLOR         8107
#define GBID_INACTIVELINEPROPERTIES    8108
#define CBID_INACTIVEENABLED           8109
#define PBID_INACTIVELINECOLOR         8110
#define PBID_INACTIVESHADOWCOLOR       8111
#define GBID_LINESTYLEPROPERTIES       8112
#define LBID_LINESTYLE                 8113

#define PLUGIN_SHARE "\\SHAREMEM\\CBZLINES"  // shared mem

// structure to return information about the plugin

typedef struct
{
    long lActiveLineColor;
    long lActiveShadowColor;
    long lInactiveLineColor;
    long lInactiveShadowColor;
    long lLineStyle;
    BOOL bActiveEnabled;
    BOOL bInactiveEnabled;
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

#endif //PLUGIN_H
