
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


#define IDMENU_MAINMENU             8200
#define IDMENUITEM_MAIN             8201
#define MLEID_SYSCOLOR              8119
#define PBID_SAVE                   8120
#define PBID_LAYOUTPALETTE          8123

#define SYSCOLORTEXT "\
The theme builder uses the layout palette for creating color setups. Modify one of the layouts to fit your \
theme. Save a copy of your current color setup by pressing the 'Save' button. This will create a Rexx skript to \
restore the colors. Afterwards drop the modified palette somewhere while holding down the ALT-key. \
This will change your system colors to the layout colors. When saving the theme these new colors will be\
 saved in the theme file.\n\
The previous colors are not restored when leaving the builder! Use the Rexx skript." 

#define REXXHEAD "\
/***********************************************************/\n\
/*                                                         */\n\
/* This Rexx skript restores a previous saved color setup. */\n\
/* It was created by the CandyBarZ theme builder           */\n\
/*                                                         */\n\
/***********************************************************/\n\
\n\
CALL RxFuncAdd \"SysLoadFuncs\", \"RexxUtil\", \"SysLoadFuncs\"\n\
CALL SysLoadFuncs\n\
\n\
SAY \"This skript will restore a saved color setup. The current colors will be lost!\"\n\
SAY \"The changes take effect after reboot.\"\n\
SAY \"Press Y (and Enter) to continue ...\"\n\
PULL answer\n\
IF answer \\= \"Y\" THEN EXIT\n\
\n"


#define CALLSYSINI1 "call SysIni \"USER\", \"PM_Colors\", \""
#define CALLSYSINI2 "\", \""
#define CALLSYSINI3 "\"\n"

#define CALLSYSINI4 "call SysIni \"USER\", \"PM_SystemFonts\", \""

typedef struct
{
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

//helper functions for CBZPluginRender
HBITMAP SetupBitmap(HWND hwnd, long lColorTop, long lColorBottom, long lX, long lY);
BOOL TileGradient(PSPAINT * pPaint, HBITMAP hbm);

#endif //PLUGIN_H

#define PBID_BROWSEDESKTOP          8105
#define STID_DESKTOPBACKGROUND      8104
#define PBID_BROWSEDESKTOPBG        8106
