/* $Id: plugin.h,v 1.2 1999/12/29 19:38:13 enorman Exp $ */

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
#define STID_PLUGINNAME                8103
#define GBID_ACTIVEBORDERPROPERTIES    8104
#define PBID_ACTIVEDARKCOLOR           8105
#define PBID_ACTIVELIGHTCOLOR          8106
#define GBID_INACTIVEBORDERPROPERTIES  8107
#define PBID_INACTIVEDARKCOLOR         8108
#define PBID_INACTIVELIGHTCOLOR        8109

#define CBID_ACTIVEENABLED             8120
#define CBID_INACTIVEENABLED           8121

#define PLUGIN_SHARE "\\SHAREMEM\\CBZBRDR"  // shared mem


typedef struct
{
    long lActiveDarkColor;
    long lActiveLightColor;
    long lInactiveDarkColor;
    long lInactiveLightColor;
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
