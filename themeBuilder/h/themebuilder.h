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
#include "CBZStrings.h"
#include "CandyBarZParser.h"
#define CBZ_TITLEBAR          0x00000001UL
#define CBZ_FRAME             0x00000002UL
#define CBZ_PUSHBUTTON        0x00000004UL
#define CBZ_RADIOBUTTON       0x00000008UL
#define CBZ_CHECKBOX          0x00000010UL
#define CBZ_FRAMEBRDR         0x00000020UL
#define CBZ_MINMAX            0x00000040UL
#define CBZ_MENU              0x00000080UL
#define CBZ_NOTEBOOK          0x00000100UL
#define CBZ_SCROLLBAR         0x00000200UL
#define CBZ_CONTAINER         0x00000400UL
#define CBZ_SYSTEM            0x00000800UL

// for folks without the warp 4 toolkit
#ifndef BKS_TABBEDDIALOG
#define BKS_TABBEDDIALOG         0x00000800  /* Tabbed dialog         */
#endif

//for now...
#define DLGID_PLUGIN             8100
#define PBID_OK                  8101
#define PBID_CANCEL              8102
#define PBID_OPEN                8103

#define DLGID_PREVIEW            700
#define ID_NONE                  701

#define DLGID_DEFAULTS             200  // default themes page
#define GBID_AVAILABLEPLUGINS      201
#define LBID_AVAILABLEPLUGINS      202
#define PBID_SELECTPLUGIN          203
#define GBID_SELECTEDPLUGINS       210
#define LBID_SELECTEDPLUGINS       211
#define PBID_PLUGINUP              212
#define PBID_PLUGINDOWN            213
#define PBID_PLUGINREMOVE          214
#define GBID_PLUGINPROPERTIES      220
#define STID_PLUGINNAMELABEL       221
#define STID_PLUGINAUTHORLABEL     222
#define STID_PLUGINNAME            223
#define STID_PLUGINAUTHOR          224
#define STID_PLUGINDATE            225
#define STID_PLUGINDATELABEL       226

#define PBID_PLUGINCONFIGURE       228
#define LBID_AVAILABLEPLUGINDLLS   250  //invisible listbox

#define PBID_SAVE                   231  // save button

#define DLGID_THEMESETUP            100
#define NBID_SETUP                  101
#define PBID_EXIT                   104  // exit button
#define PBID_HELP                   105  // help button
#define PBID_PREVIEW                106

#define DLGID_INFO                  300
#define GBID_THEMEINFO              301
#define STID_THEMENAMELABEL         302
#define EFID_THEMENAME              303
#define STID_THEMEAUTHORLABEL       304
#define EFID_THEMEAUTHOR            305
#define STID_THEMEDATELABEL         306
#define EFID_THEMEDATE              307
#define STID_THEMEVERSIONLABEL      308
#define EFID_THEMEVERSION           309
#define STID_THEMEFILELABEL         310
#define EFID_THEMEFILE              311
#define INFO_BITMAP                 320

#define HLPTBL_SETUP                1000  // help tables
#define HLPSTBL_SETUP               1001
#define HLPIDX_SETUP                1002

#define HLPSTBL_INFOPAGE            2000
#define HLPIDX_INFOPAGE             2001

#define HLPSTBL_PAGE1               3000  // default colors help
#define HLPIDX_PAGE1                3001
#define HLPIDX_PLUGINS              3002
#define HLPIDX_SELECTREMOVE         3003
#define HLPIDX_FORMAT               3004
#define HLPIDX_SAVE                 3005


#define MAX_CUSTOM_TITLE_SIZE       64  // longest title for custom entry
#define MYLBM_INIT  WM_USER     // for subclassing listbox

#define CBZ_SAVE    WM_USER + 1
#define CBZ_PREVIEW WM_USER + 2
#define CBZ_NOPREVIEW WM_USER + 3

// structure to return information about the plugin from the pluginDLL
typedef struct
{
    char PluginName[64];
    char PluginAuthor[64];
    char PluginDate[16];
    long lWindowTypes;
}
PLUGIN_INFO;

typedef struct
{
    CBZDATA *pCBZData;
    char szTitleOrig[MAX_CUSTOM_TITLE_SIZE];
    char *pszTitle;
    char szClassName[16];
    long lWindowType;
}
DLGDATA;

// setup function prototypes
typedef HBITMAP(*_Optlink SETUPPFN) (HWND, long, long, long, long);
typedef HBITMAP(*_Optlink FSETUPPFN) (HWND, char *, long);
typedef void (*_Optlink PLUGINPFN) (PLUGIN_INFO * pInfo);
typedef BOOL(*_Optlink PSAVEFN) (char *szText, PVOID pData);

typedef BOOL(*_Optlink PAPPLYFN) (char szClass[], KEY_VALUE * kv, int count,
                                    int enabledState, PVOID pData);

// main dlg proc
MRESULT EXPENTRY ThemeDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY WindowClassDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY InfoDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

BOOL AddNotebookPage(HWND hwndNoteBook, long pageID, PFNWP pfnDlgProc,
                     char szTabText[], char szMinorText[], char szStatusText[],
                     BOOL bMajor, char szWindowClass[]);

BOOL OpenThemeClass(HWND hwnd, char szFilename[], CBZDATA *pCBZData, char szClassName[]);
BOOL OpenThemeInfo(HWND hwnd, char szFilename[]);

BOOL AddPlugin(HWND hwnd, CBZDATA *pCBZData, char dllName[], char appName[]);
BOOL RemovePlugin(CBZDATA *pCBZData, int index);
void SwapPlugins(CBZDATA *pCBZData, int index1, int index2);
BOOL LoadPlugins(HWND hwnd, CBZDATA * pCBZData, HWND hwndLBAvailable,
                 HWND hwndLBAvailableDLL, long Win_Type);
BOOL InitPlugins(HWND hwnd, CBZDATA *pCBZData, char appName[], HWND hwndLBSelected,
                 HWND hwndLBAvailable, HWND hwndLBAvailableDLL);

#endif

/*************************************************************************************************************
    $Id: themebuilder.h,v 1.3 2000/09/24 20:42:02 cwohlgemuth Exp $

    $Log: themebuilder.h,v $
    Revision 1.3  2000/09/24 20:42:02  cwohlgemuth
    Support for SYSTEM plugins added.

    Revision 1.2  1999/12/29 19:38:20  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:48:30  ktk
    Import


*************************************************************************************************************/

