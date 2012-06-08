/******************************************************************************
  Copyright Netlabs, 1998, this code may not be used for any military purpose
******************************************************************************/
#ifndef _CANDYBARZ_H_
#define _CANDYBARZ_H_

#define INCL_PM
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>             // for offsetof
#include <ctype.h>              //for toupper

#include "psrt.h"
#include "CBZStrings.h"


#define WMGR_MESSAGE_ATOM   "WMGR_MESSAGE"  // atom for communicating with window manager
#define WMGR_FRAMECREATED   1
#define WMGR_YOURWMFRAME    2
#define WMGR_CLOSECONTROLS  3
#define WMGR_AREYOUWMGR     4

#define IAMWMGR             5555    //This is the returned value of WM frame when asked WMGR_AREYOUWMGR

#define SC_CLOSE_ 0x8004

#define PSTB_PSUTREQUIRED   ( int )0x0000000D  // required version of psrt

#define CBZ_SHARE               "\\SHAREMEM\\CANDYBARZ" // shared memory
#define PSTB_COLORCHANGE_ATOM   "PSTBM_COLORCHANGE"  // atom to broadcast changes from cbsetup
#define PSTB_VERSION        0x00000130UL  // version number


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

// structure used for painting (Passed into the Plugin's Render Procedures)
typedef struct
{
    HWND hwnd;                  // hwnd to draw to
    HPS hps;                    // hps to draw in
    HPS hpsWin;
    RECTL rectlWindow;          // window rectangle
    RECTL rectlUpdate;          // update rectangle
    RECTL rectlText;            // text Rectangle
    char szTitleText[CCHMAXPATH];      // titlebar text
}
PSPAINT;

//definitions of the callable plugin procedures.
typedef BOOL(*_Optlink PPAINTFN) (PSPAINT * pPaint, PVOID pPluginData, short sActive);
typedef BOOL(*_Optlink PINITFN) (HINI hIni, char szName[], char szClass[], PVOID * pData);
typedef BOOL(*_Optlink PDESTROYFN) (HWND hwnd, PVOID pData);
typedef BOOL(*_Optlink PPLUGINWP) (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2, PVOID pData, PFNWP pfnOrigWndProc, MRESULT *rc);

//structure used by the Paint Procedure for temporary variables.
typedef struct
{
    // device contexts
    HDC hdc;
    // bitmap stuff
    HBITMAP hbmTemp;
    BITMAPINFOHEADER2 bif2;

    // rectangles, points, etc.
    SIZEL slHPS;
    HWND hwnd;
    HAB hab;

    long lTbColor;
}TEMPPAINT;

//used to store a list of plugins in shared memory!
typedef struct
{
    int cbCount;
    char pluginDLL[20][9];      // list of 10 plugin DLLs.

    BOOL bEnabled;
    BOOL bUseCust;

    BOOL    bTitlebarEnabled;              //which window classes are being replaced
    short   sTitlebarIndex;
    BOOL    bFrameBackgroundEnabled;
    short   sFrameBackgroundIndex;
    BOOL    bFrameBorderEnabled;
    short   sFrameBorderIndex;
    BOOL    bPushButtonEnabled;
    short   sPushButtonIndex;
    BOOL    bRadioButtonEnabled;
    short   sRadioButtonIndex;
    BOOL    bCheckBoxEnabled;
    short   sCheckBoxIndex;
    BOOL    bMinMaxEnabled;
    short   sMinMaxIndex;
    BOOL    bMenuEnabled;
    short   sMenuIndex;
    BOOL    bNoteBookBackgroundEnabled;
    short   sNoteBookBackgroundIndex;
    BOOL    bScrollBarEnabled;
    short   sScrollBarIndex;
    BOOL    bContainerEnabled;
    short   sContainerIndex;
    BOOL    bSystemEnabled;
    short   sSystemIndex;
    BOOL    bDummyEnabled;
    short   sDummyIndex;

}PLUGINSET;

// profile writes are based on ulDataOffset.  Members to be written out should go ahead of it
// additionally, all future updates should update the version number, so that COLORSETS are read
// in correctly if they change
typedef struct
{
    // version
    ULONG ulVersion;            // version id
    PLUGINSET plugins;          // 0 = active, 1 = inactive
    char szThemeFilename[CCHMAXPATH + 1];
    // anything that should be written out with
    // above options should be added just before
    // this comment
    ULONG ulDataOffset;         // anything that gets written out separately should go
                                // in any order after ulDataOffset
    PFNWP pTBOrigWndProc;
    PFNWP pFrameOrigWndProc;
    PFNWP pButtonOrigWndProc;
    PFNWP pMenuOrigWndProc;
    PFNWP pStaticOrigWndProc;
    PFNWP pNotebookOrigWndProc;
    PFNWP pContainerOrigWndProc;
    PFNWP pScrollBarOrigWndProc;
    HWND  hwndWindowManagerObject;
    ATOM  WMgrAtom;
}
CBZSHARE;

//shared memory for static controls.  Needs some work to be more generalized 
typedef struct 
{ 
  ULONG ulDataOffset; // anything that gets written out separately should go 

  PFNWP pfnOrigWndProc; 
}PSSTATICSHARE;

// data structure to store the Plugin Info.
typedef struct
{
    char szPluginDLL[9];        //The dll name
    HMODULE hModDll;            //handle to the plugin dll
    int cbData;                 //sizeof the plugin's data
    PVOID pData;                //pointer to the plugin's data
    BOOL bActiveEnabled;        //is this plugin enabled?
    BOOL bInactiveEnabled;
    PPLUGINWP pPluginWndProc;   //pointer to the WndProc for the plugin
    PPAINTFN pfnPluginRender;   //pointer to the plugin's render function.
}PSPLUGIN;

// data structure for all window replacement data!  consolidated into 1 structure
typedef struct
{
    int cbPlugins;              //number of plugins in use
    PSPLUGIN Plugins[20];       //20 Plugins Max - for now.. should create a more general solution!?
    char szWPSViewText[64];
    BOOL bEnabled;              // enabled?
    BOOL bUseCust;              //set if this window is using custom colors.

    HWND    hwndWMgrFrame;      /* this is the control frame of the indow manager decoration */

    BOOL    bTitlebarEnabled;              //which window classes are being replaced
    short   sTitlebarIndex;
    BOOL    bFrameBackgroundEnabled;
    short   sFrameBackgroundIndex;
    BOOL    bFrameBorderEnabled;
    short   sFrameBorderIndex;
    BOOL    bPushButtonEnabled;
    short   sPushButtonIndex;
    BOOL    bRadioButtonEnabled;
    short   sRadioButtonIndex;
    BOOL    bCheckBoxEnabled;
    short   sCheckBoxIndex;
    BOOL    bMinMaxEnabled;
    short   sMinMaxIndex;
    BOOL    bMenuEnabled;
    short   sMenuIndex;
    BOOL    bNoteBookBackgroundEnabled;
    short   sNoteBookBackgroundIndex;
    BOOL    bScrollBarEnabled;
    short   sScrollBarIndex;
    BOOL    bContainerEnabled;
    short   sContainerIndex;
    BOOL    bSystemEnabled;
    short   sSystemIndex;
    BOOL    bDummyEnabled;
    short   sDummyIndex;


}CBZDATA;

typedef struct
{
  CBZSHARE *pCBZShare;
  ATOM  Atom;
  ATOM  WMgrAtom;
  BOOL  bCustomPaint;
}PRIVATEFRAMEDATA;

////////// begin procedure/function prototypes ////////////


//General Paint Procedure, Each window paint calls this procedure with the appropriate parameters
BOOL CandyBarZGeneralPaintProc(HWND hwnd, CBZDATA *pCBZData, BOOL bEntire, int sState,
                                int startIndex, int endIndex, ULONG ulWindowType);


// the replaced TB window procedure
MRESULT EXPENTRY CandyBarZTBProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// TB paint procedure
#define CandyBarZTBPaintProc(a,b,c,d) CandyBarZGeneralPaintProc(a, b, c, d, b->sTitlebarIndex, b->sFrameBackgroundIndex, CBZ_TITLEBAR)
// set text/text background colors into presentation space
void TBSetPSColors(HWND hwnd, HPS hps, int sState);
void TBFillBG(PSPAINT *pPaint, int sState);

// blt completed image to the screen
BOOL BltToWindow(PSPAINT * pPaint);

// the  Frame window procedure
MRESULT EXPENTRY CandyBarZFrameProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// Frame paint procedure
BOOL CandyBarZFramePaintBackgroundProc(HWND hwnd, CBZDATA *pCBZData, RECTL * pRectl, HPS hps);
BOOL CandyBarZMinMaxPaintProc(HWND hwnd, CBZDATA *pCBZData, RECTL * rect, HPS hpsControl, short state);

BOOL CandyBarZFrameBorderPaintProc(HWND hwnd, RECTL * rect, HPS hpsFrame, short sState);

// the static control window procedure
MRESULT EXPENTRY CandyBarZStaticProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

// the button window procedure
MRESULT EXPENTRY CandyBarZButtonProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// PushButton paint procedure
#define CandyBarZPushButtonPaintProc(a, b, c) CandyBarZGeneralPaintProc(a, b, FALSE, c, b->sPushButtonIndex, b->sRadioButtonIndex, CBZ_PUSHBUTTON)
// CheckBox/RadioButton paint procedure
#define CandyBarZRadioButtonPaintProc(a, b, c) CandyBarZGeneralPaintProc(a,b, TRUE, c, b->sRadioButtonIndex, b->sCheckBoxIndex, CBZ_RADIOBUTTON)

#define CandyBarZCheckBoxPaintProc(a, b, c) CandyBarZGeneralPaintProc(a,b, TRUE, c, b->sCheckBoxIndex, b->sMinMaxIndex, CBZ_CHECKBOX)

// The notebook window procedure
MRESULT EXPENTRY CandyBarZNotebookProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
#define CandyBarZNotebookPaintProc(a, b, c) CandyBarZGeneralPaintProc(a,b, FALSE, c, b->sNoteBookBackgroundIndex, b->sScrollBarIndex, CBZ_NOTEBOOK)
 
// set text/text background colors into presentation space
void ButtonSetPSColors(HWND hwnd, HPS hps, int sState);
void ButtonFillBG(PSPAINT *pPaint, int sState, CBZDATA *pCBZData);

// the  menu window procedure
MRESULT EXPENTRY CandyBarZMenuProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
// menu paint procedure
BOOL CandyBarZMenuPaintProc(HWND hwnd/*, RECTL * pRectl, HPS hps*/);
//BOOL CandyBarZMenuItemPaintProc(HWND hwndMenu, HPS hpsMenu, short id, short sState, PSMENUDATA *pMenuData);

BOOL CandyBarZWindowCreate(HWND hwnd);
BOOL CandyBarZSettingsChange(HWND hwnd);
BOOL CandyBarZDestroyWindowData(HWND hwnd);
HWND CandyBarZGetTopLevelFrameHWND(HWND hwnd);

MRESULT EXPENTRY CandyBarZContainerProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

MRESULT EXPENTRY CandyBarZScrollBarProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

/*************************************************************************************************************

    $Id: candybarz.h,v 1.6 2000/09/24 21:06:21 cwohlgemuth Exp $

    $Log: candybarz.h,v $
    Revision 1.6  2000/09/24 21:06:21  cwohlgemuth
    Support for SYSTEM plugins added.

    Revision 1.5  2000/09/09 22:50:55  cwohlgemuth
    Support for scrollbars and containers added.

    Revision 1.4  2000/09/06 14:34:25  cwohlgemuth
    Support for notebook procedure added

    Revision 1.3  2000/08/31 17:24:21  cwohlgemuth
    *** empty log message ***

    Revision 1.2  2000/05/10 15:39:17  cwohlgemuth
    Changed to support custom frame backgrounds in static controls (CW)

    Revision 1.1  2000/03/26 06:56:54  enorman
    resyncing with my local tree.

    Revision 1.1  1999/06/15 20:48:01  ktk
    Import

    Revision 1.11  1998/10/17 19:33:16  enorman
    Made some changes to eliminate PSRT.DLL

    Revision 1.10  1998/09/22 12:06:38  mwagner
    Added better dialog support

    Revision 1.8  1998/09/12 10:09:47  pgarner
    Added copyright notice

    Revision 1.7  1998/09/12 07:35:47  mwagner
    Added support for bitmap images

    Revision 1.4  1998/08/31 19:35:48  enorman
    Finished the implementation of 3D Text

    Revision 1.3  1998/08/10 02:52:39  enorman
    Added the 3D text define

    Revision 1.2  1998/06/29 15:34:53  mwagner
    Matts S3 and PMView fixes

    Revision 1.1  1998/06/08 14:18:57  pgarner
    Initial Checkin

*************************************************************************************************************/

#endif
