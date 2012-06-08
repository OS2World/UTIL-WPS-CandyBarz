/* $Id: plugin.h,v 1.3 2000/10/04 19:52:12 cwohlgemuth Exp $ */

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

//The following are for the plugin's setup dialog
#define DLGID_PLUGIN                8100  //don't change
#define PBID_OK                     8101  //don't change
#define PBID_CANCEL                 8102
#define GBID_ACTIVETEXTPROPERTIES   8103
#define CBID_ACTIVEENABLED          8104
#define CBID_ACTIVETEXTCOLOR        8105
#define PBID_ACTIVETEXTCOLOR        8106
#define CBID_ACTIVE3DTEXT           8107
#define PBID_ACTIVESHADOWCOLOR      8108
#define CBID_ACTIVEFONT             8109
#define PBID_ACTIVEFONT             8110
#define GBID_ACTIVETEXTALIGNMENT    8111
#define RBID_ACTIVELEFTTEXT         8112
#define RBID_ACTIVECENTERTEXT       8113
#define RBID_ACTIVERIGHTTEXT        8114
#define CBID_ACTIVETEXTOFFSET     8115
#define SPBID_ACTIVETEXTOFFSET    8116

#define GBID_INACTIVETEXTPROPERTIES   8120
#define CBID_INACTIVEENABLED          8121
#define CBID_INACTIVETEXTCOLOR        8122
#define PBID_INACTIVETEXTCOLOR        8123
#define CBID_INACTIVE3DTEXT           8124
#define PBID_INACTIVESHADOWCOLOR      8125
#define CBID_INACTIVEFONT             8126
#define PBID_INACTIVEFONT             8127
#define GBID_INACTIVETEXTALIGNMENT    8128
#define RBID_INACTIVELEFTTEXT         8129
#define RBID_INACTIVECENTERTEXT       8130
#define RBID_INACTIVERIGHTTEXT        8131
#define CBID_INACTIVETEXTOFFSET     8132
#define SPBID_INACTIVETEXTOFFSET    8133

#define PLUGIN_SHARE "\\SHAREMEM\\CBZTEXT"  // shared mem

#define LEFT_TEXT    0
#define CENTER_TEXT  1
#define RIGHT_TEXT   2
#define MANUAL_TEXT  3
#define FLAG_CUSTOM_COLOR    0x00000001UL
#define FLAG_DEF_FONT     0x00000002UL
#define FLAG_3DTEXT       0x00000004UL


//structure used for the shared memory AND window words
typedef struct
{
    long lActiveTextColor;
    long lActiveShadowColor;
    short sActiveTextAlignment;
    long lActiveTextOffset;
    ULONG flActive;
    long lInactiveTextColor;
    long lInactiveShadowColor;
    short sInactiveTextAlignment;
    long lInactiveTextOffset;
    ULONG flInactive;
    BOOL bActiveEnabled;
    BOOL bInactiveEnabled;
}
PLUGINSHARE;


//data needed by the CBZPluginRender method...It is allocated
// dynamically in that procedure to keep it off the stack.
typedef struct
{
    // font stuff
    FONTMETRICS font;
    SIZEF sizef;
    FATTRS fattrs;
    char *szFontName;
    long lcid;
    HPS hpsWin;
    HAB hab;
    POINTL aptl[TXTBOX_COUNT];
    long lShadowColor;
}
PLUGINPAINT;

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
