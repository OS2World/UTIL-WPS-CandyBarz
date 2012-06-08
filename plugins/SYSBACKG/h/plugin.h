
/******************************************************************************

  Copyright Netlabs, 1998-2000 

  http://www.netlabs.org

  (C) E. Norman, C. Wohlgemuth

******************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef PLUGIN_H
#define PLUGIN_H

#define INCL_PM
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>
#include "pluginUtils.h"

#define PLUGIN_SHARE "\\SHAREMEM\\SYSCOLOR"  // shared mem

#define DLGID_PLUGIN                   8100
#define PBID_OK                        8101
#define PBID_CANCEL                    8102
#define STID_DESKTOPBACKGROUND         8103
#define PBID_BROWSEDESKTOPBG           8104
#define GBID_DESKTOP                   8105

typedef struct
{
    BOOL bActiveEnabled;
  char szDesktopBG[CCHMAXPATH];
  
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

