
/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#ifndef _SETUP_H_
#define _SETUP_H_
#define INCL_PM
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "candybarz.h"
#include "psrt.h"
#include "CandyBarZParser.h"

// for folks without the warp 4 toolkit
#ifndef BKS_TABBEDDIALOG
#define BKS_TABBEDDIALOG         0x00000800  /* Tabbed dialog         */
#endif

//for now...
#define DLGID_PLUGIN             8100
#define PBID_OK                  8101
#define PBID_CANCEL              8102

#define DLGID_PREVIEW            700
#define ID_NONE                  701
#define PBID_COLOROK             702
#define CHID_PREVIEW             703
#define RBID_PREVIEW1            704
#define RBID_PREVIEW2            705

#define DLGID_DEFAULTS           200  // default themes page
#define GBID_AVAILABLETHEMES     201
#define LBID_AVAILABLETHEMES     202
#define PBID_SELECTTHEME         203
#define PBID_PREVIEWTHEME        204
#define GBID_THEMEPROPERTIES     205
#define STID_THEMENAMELABEL      206
#define STID_THEMENAME           207
#define STID_THEMEAUTHORLABEL    208
#define STID_THEMEAUTHOR         209
#define STID_THEMEDATELABEL      210
#define STID_THEMEDATE           211
#define STID_THEMEVERSIONLABEL   212
#define STID_THEMEVERSION        213
#define STID_THEMEFILELABEL      214
#define STID_THEMEFILE           215

#define GBID_CURRENTTHEME        216
#define EFID_CURRENTTHEME        217

#define LBID_AVAILABLETHEMEFILES 218  //invisible

//These are used in the customize page.
#define LBID_ACTIVE_SELECTEDPLUGINS          260
#define LBID_ACTIVE_AVAILABLEPLUGINS         261
#define LBID_ACTIVE_AVAILABLEPLUGINDLLS      262  //invisible listbox
#define LBID_INACTIVE_SELECTEDPLUGINS        263
#define LBID_INACTIVE_AVAILABLEPLUGINS       264
#define LBID_INACTIVE_AVAILABLEPLUGINDLLS    265  //invisible listbox

#define PBID_APPLY                  230  // apply button
#define PBID_SAVE                   231  // save button
#define PBID_UNDO                   232  // undo button

#define TITLEBAR_BITMAP             290  //titlebar bitmap
#define FILL_BITMAP                 291  //fill bitmap

#define DLGID_SETUP                 100
#define NBID_SETUP                  101
#define PBID_EXIT                   104  // exit button
#define PBID_HELP                   105  // help button


#define DLGID_INSTALL               400  // install dlg
#define STID_DLLDEST1               401  // text for dll destination
#define STID_DLLDEST2               402  // text for dll destination
#define CBID_DLLDEST                403  // combox box for choosing dll destination
#define STID_INIDEST                404  // text for ini dest
#define EFID_INIDEST                405  // entryfield for ini dest
#define PBID_BROWSE                 406  // browse button
#define MLEID_STATUS                407  // status mle
#define PBID_INSTALL                408  // install button
#define PBID_UNINSTALL              409  // uninstall button
#define STID_STATUS                 410  // text for status mle

#define DLGID_SETUPPAGE2            500  // custom colors page
#define LBID_CUSTOMLIST             501  // listbox showing custom entries
#define PBID_CUSTOMADD              502  // add button
#define PBID_CUSTOMREMOVE           503  // remove button
#define PBID_CUSTOMEDIT             505  // edit button
#define PBID_CUSTOMCLONE            506  // clone button

#define DLGID_CUSTOMIZE             600  // customize dlg (most controls are under defaultcolors dlg)
#define EFID_CUSTTITLE              601  // title entryfield
#define CHID_ENABLECB               602  // enable candybarz checkbox
#define CHID_ENABLECUST             603  // enable custom colors checkbox
#define STID_CUSTTITLE              604  // "Title"
#define PBID_CUSTSAVE               605  // save button
#define PBID_CUSTCANCEL             606  // cancel button
#define ARBID_ACTIVE                607  // radio button to choose active page
#define ARBID_INACTIVE              608  // radio button to choose inactive page

#define DLGID_ABOUT                 800
#define STID_WHO                    801

#define HLPTBL_SETUP                1000  // help tables
#define HLPSTBL_SETUP               1001
#define HLPIDX_SETUP                1002

#define HLPSTBL_INSTALL             2000  // install help
#define HLPIDX_INSTALL              2001
#define HLPIDX_DLLDEST              2002
#define HLPIDX_INIDEST              2003
#define HLPIDX_STATUS               2004
#define HLPIDX_INSTBUTTON           2005
#define HLPIDX_UNINSTALL            2006

#define HLPSTBL_PAGE1               3000  // default colors help
#define HLPIDX_PAGE1                3001
#define HLPIDX_TOPBOTTOM            3002
#define HLPIDX_APPLYSAVE            3003
#define HLPIDX_UNDODEF              3004
#define HLPIDX_ENABLEDEF            3005
#define HLPIDX_HORIZONTAL           3006
#define HLPIDX_DRAWBORDER           3007
#define HLPIDX_FILLBUTTONS          3008
#define HLPIDX_MENU                 3009
#define HLPIDX_FILES                3010
#define HLPIDX_3D                   3011
#define HLPIDX_S3                   3012
#define HLPIDX_ACTIVEINACTIVE       3013
#define HLPIDX_PMVIEW               3014

#define HLPSTBL_PAGE2               4000  // custom colors help
#define HLPIDX_PAGE2                4001
#define HLPIDX_ADDEDIT              4002
#define HLPIDX_REMOVE               4003
#define HLPIDX_CLONE                4004

#define HLPSTBL_CLRCHANGE           5000  // color change dlg help
#define HLPIDX_CLRCHANGE            5001

#define HLPSTBL_CUSTOMIZE           6000  // customize dlg help. most entries are under default colors
#define HLPIDX_CUSTOMIZE            6001
#define HLPIDX_CUSTTITLE            6002
#define HLPIDX_CUSTUNDO             6003
#define HLPIDX_CHKBOXES             6004
#define HLPIDX_SAVECANCEL           6005
#define HLPIDX_CUSTMENU             6006

#define MAX_CUSTOM_TITLE_SIZE       64  // longest title for custom entry
#define ORIG_COLORS                 0  // ???
#define CURR_COLORS                 1  // ???

#define MYLBM_INIT  WM_USER     // for subclassing listbox

// structure to return information about the plugin from the pluginDLL
typedef struct
{
    char PluginName[64];
    char PluginAuthor[64];
    char PluginDate[16];
    long lWindowTypes;
}
PLUGIN_INFO;

// function prototypes
typedef HBITMAP(*_Optlink SETUPPFN) (HWND, long, long, long, long);
typedef HBITMAP(*_Optlink FSETUPPFN) (HWND, char *, long);
typedef void (*_Optlink PLUGINPFN) (PLUGIN_INFO * pInfo);

typedef BOOL(*_Optlink PAPPLYFN) (char szClass[], KEY_VALUE * kv, int count, int enabledState, PVOID pData);
typedef BOOL(*_Optlink PSAVEFN) (HINI hIni, char szClass[], PVOID pData, char szCustomName[]);


// for passing title to customize dlg
typedef struct
{
    USHORT cbFix;               // size
    char szTitle[MAX_CUSTOM_TITLE_SIZE];  // title
    BOOL bCloning;
}
TCUST;

typedef struct
{
    CBZDATA *pCBZData;
    BOOL bActiveFlag;
    char szTitleOrig[MAX_CUSTOM_TITLE_SIZE];
    char *pszTitle;
}
DLGDATA;

// install dlg
MRESULT EXPENTRY InstallDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// update install status mle
void SetStatusLine(HWND hwnd, char *pszText);
// subclass file dlg so user can choose directory
MRESULT EXPENTRY INIDestFileDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// main dlg proc
MRESULT EXPENTRY SetupDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// default colors dlg procs
MRESULT EXPENTRY ColorDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// for custom colors page
MRESULT EXPENTRY CustomPageProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// for customize dlg
MRESULT EXPENTRY CustomizeDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// subclass listbox proc to enable double click
MRESULT EXPENTRY ListBoxDblClickProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// imported from doscalls.  used to unlock dll for upgrade installation
APIRET APIENTRY DosReplaceModule(PSZ pszOldModule, PSZ pszNewModule, PSZ pszBackup);

BOOL AddPlugin(HWND hwnd, CBZDATA *pCBZData, char dllName[], char appName[], BOOL bActive);
BOOL RemovePlugin(CBZDATA *pCBZData, int index, BOOL bActive);
void SwapPlugins(CBZDATA *pCBZData, int index1, int index2, BOOL bActive);
BOOL LoadPlugins(HWND hwnd, /*CBZDATA *pCBZData,*/ HWND hwndLBAvailable, HWND hwndLBAvailableDLL, BOOL bActive);
BOOL InitPlugins(HWND hwnd, CBZDATA *pCBZData, char appName[], HWND hwndLBSelected, HWND hwndLBAvailable, HWND hwndLBAvailableDLL, BOOL bActive);

BOOL UpdateControls(HWND hwnd, char themeFile[]);
BOOL ApplyTheme(char themeFile[]);
BOOL SetCurrentThemeText(HWND hwndEntryField, char filename[]);

BOOL SaveSettings(HWND hwnd, HINI hIni, /*char szClassName[], char szShareName[],*/
                    char szCustomAppName[], char szThemeFile[]);


BOOL ApplyPreviewTheme(HWND hwndPreview, char themeFile[]);
int ApplyPreviewBlock(HWND hwnd, char themeFile[], char szClass[], char szCBZBase[], HINI hIni, int count);

int ApplyBlock(char themeFile[], char szClass[], char szCBZBase[], HINI hIni, int count);

// preview dlg
MRESULT EXPENTRY PreviewDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#define CBZ_UPDATEPREVIEW  WM_USER + 1

BOOL SetControlText(HAB hab, HWND hwndControl, int stringID);

/*************************************************************************************************************

    $Id: setup.h,v 1.4 2000/09/24 21:06:58 cwohlgemuth Exp $

    $Log: setup.h,v $
    Revision 1.4  2000/09/24 21:06:58  cwohlgemuth
    About page added.

    Revision 1.3  2000/03/26 06:51:23  enorman
    resyncing with my local tree.

    Revision 1.2  1999/12/29 19:38:10  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:48:02  ktk
    Import

    Revision 1.17  1998/10/17 19:33:25  enorman
    Made some changes to eliminate PSRT.DLL

    Revision 1.16  1998/09/30 05:37:42  mwagner
    Made the PMView fix optional, this replaces version 1.15

    Revision 1.14  1998/09/27 23:31:30  enorman
    Added help for the color cloning

    Revision 1.13  1998/09/26 07:04:46  enorman
    Added code for custom color cloning

    Revision 1.12  1998/09/22 12:06:26  mwagner
    Added better dialog support

    Revision 1.10  1998/09/15 10:11:41  enorman
    added #defines for some new controls
    reordered/renumbered controls for the new dialogs for readability

    Revision 1.9  1998/09/12 10:10:18  pgarner
    Added copyright notice

    Revision 1.8  1998/09/12 07:35:47  mwagner
    Added support for bitmap images

    Revision 1.5  1998/08/31 19:35:54  enorman
    Finished the implementation of 3D Text

    Revision 1.4  1998/08/10 02:02:06  enorman
    Added defines for active and inactive 3Dtext and ShadowColor
    Revision 1.3  1998/06/29 15:35:09  mwagner
    Matts S3 and PMView fixes
    Revision 1.2  1998/06/14 11:19:57  mwagner
    Added prototype for DosReplaceModule
    Revision 1.1  1998/06/08 14:18:58  pgarner
    Initial Checkin

*************************************************************************************************************/

#endif
