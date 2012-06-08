
/******************************************************************************

  Copyright Netlabs, 1998-2000 

  http://www.netlabs.org

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

#include "candybarz.h"


int _CRT_init(void);
void _CRT_term(void);


ULONG       ulOffsetPrivateData;
CBZSHARE    CBZShare;
LONG lcxIcon;
LONG lcyIcon;
LONG lcxPointer;
LONG lcyPointer;

ULONG _System _DLL_InitTerm(HMODULE hmod, ULONG ulInitTerm)
{
    CBZSHARE *pCBZShare;
    char szProfile[CCHMAXPATH + 1];
    CLASSINFO ciTB = {0}, ciFrame = {0}, ciStatic = {0}, ciButton = {0};
    CLASSINFO ciMenu = {0}, ciNotebook ={0}, ciScrollBar={0},ciContainer={0};
    ULONG cbSize;
    HINI hIni;
    BOOL bEnabled;
    BOOL bNotebook=TRUE;
    BOOL bContainer=TRUE;
    BOOL bScrollBar=TRUE;

    switch (ulInitTerm)
    {
        case 0:                    // initialization
        {

            // init c runtime
            if (_CRT_init() == -1)
                return (0);

            // get original titlebar class info
            if (!WinQueryClassInfo(NULLHANDLE, WC_TITLEBAR, &ciTB))
                return (0);

            // get original frame class info
            if (!WinQueryClassInfo(NULLHANDLE, WC_FRAME, &ciFrame))
                return (0);

            // get original static class info
            if (!WinQueryClassInfo(NULLHANDLE, WC_STATIC, &ciStatic))
                return (0);

            // get original button class info
            if (!WinQueryClassInfo(NULLHANDLE, WC_BUTTON, &ciButton))
                return (0);

            // get original menu class info
            if (!WinQueryClassInfo(NULLHANDLE, WC_MENU, &ciMenu))
               return (0);

            // CANDYBAR _must_ be inserted before PMCTRL in the SYS_DLL entry of *.ini
            // to get this class info. Unfortunatly there must be a specific
            // Order with ObjectDesktops DLL, too, to get both to work. Not sure if it's
            // possible  to have all.
            // get original notebook background class info
            if (!WinQueryClassInfo(NULLHANDLE, WC_NOTEBOOK, &ciNotebook)) {           
               bNotebook=FALSE;
               /* Ok, we couldn't get the classinfo so set a flag to indicate
                  that we can't replace the proc later. */
            }

            // CANDYBAR _must_ be inserted before PMCTRL in the SYS_DLL entry of *.ini
            // to get this class info. Unfortunatly there must be a specific
            // Order with ObjectDesktops DLL, too, to get both to work. Not sure if it's
            // possible  to have all.
            // get original notebook background class info
            // get original scrollbar class info
            if (!WinQueryClassInfo(NULLHANDLE, WC_SCROLLBAR, &ciScrollBar)) {
               bScrollBar=FALSE;
            }

            // CANDYBAR _must_ be inserted before PMCTRL in the SYS_DLL entry of *.ini
            // to get this class info. Unfortunatly there must be a specific
            // Order with ObjectDesktops DLL, too, to get both to work. Not sure if it's
            // possible  to have all.
            // get container class info
            if (!WinQueryClassInfo(NULLHANDLE, WC_CONTAINER, &ciContainer)) {
             bContainer=FALSE;
               /* Ok, we couldn't get the classinfo so set a flag to indicate
                  that we can't replace the proc later. */
            }

            // query location of candybarz profile
            if (!PrfQueryProfileString(HINI_USERPROFILE,
                                       "CandyBarZ",
                                       "Profile",
                                       NULL,
                                       szProfile,
                                       sizeof(szProfile)))
            {
                return (0);
            }
            if ((hIni = PrfOpenProfile(NULLHANDLE, szProfile)) == NULLHANDLE)
            {
                return (0);
            }

            // find out if CandyBarZ is enabled
            cbSize = sizeof(bEnabled);
            if (PrfQueryProfileData(hIni, "UserOptions", "Enabled", &bEnabled, &cbSize))
            {
                if (!bEnabled)
                {
                    PrfCloseProfile(hIni);
                    return (0);
                }
            }
            else
            {
                bEnabled = TRUE;
                PrfWriteProfileData(hIni, "UserOptions", "Enabled", &bEnabled, sizeof(bEnabled));
            }


            // allocate shared mem
            if (DosAllocSharedMem((PPVOID) & pCBZShare,
                                  CBZ_SHARE,
                                  sizeof(CBZSHARE),
                             PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
                return (0);

            // clear it
            memset(pCBZShare, 0, sizeof(CBZSHARE));
            memset(&CBZShare, 0, sizeof(CBZSHARE));

            cbSize = offsetof(CBZSHARE, ulDataOffset);

            // if user options are present, use them
            if (!PrfQueryProfileData(hIni, "UserOptions", "DefaultPlugins", pCBZShare, &cbSize))
            {
                cbSize = offsetof(CBZSHARE, ulDataOffset);
                if (!PrfQueryProfileData(hIni, "DefaultOptions", "Plugins", pCBZShare, &cbSize))
                {
                    //No DefaultOptions defined...Use these defaults.
                    pCBZShare->ulVersion = PSTB_VERSION;
                    strcpy(pCBZShare->szThemeFilename, "default.cbz");

                    pCBZShare->plugins.cbCount = 2;

                    strcpy(pCBZShare->plugins.pluginDLL[0], "GRADIENT");
                    strcpy(pCBZShare->plugins.pluginDLL[1], "CBZTEXT");
                    pCBZShare->plugins.bEnabled = TRUE;
                    pCBZShare->plugins.bUseCust = FALSE;
                    pCBZShare->plugins.sTitlebarIndex = 0;
                    pCBZShare->plugins.bTitlebarEnabled = TRUE;
                    pCBZShare->plugins.sFrameBackgroundIndex = 2;

                    //write these defaults to the ini file
                    cbSize = offsetof(CBZSHARE, ulDataOffset);
                    if (!PrfWriteProfileData(hIni,
                                             "UserOptions",
                                             "DefaultPlugins",
                                             pCBZShare,
                                             cbSize) ||
                            !PrfWriteProfileData(hIni,
                                                 "DefaultOptions",
                                                 "Plugins",
                                                 pCBZShare,
                                                 cbSize))
                    {
                        PrfCloseProfile(hIni);
                        DosFreeMem(pCBZShare);
                        return (0);
                    }
                }
             }

            pCBZShare->ulDataOffset         = ciFrame.cbWindowData;
            pCBZShare->pTBOrigWndProc       = ciTB.pfnWindowProc;
            pCBZShare->pFrameOrigWndProc    = ciFrame.pfnWindowProc;
            pCBZShare->pButtonOrigWndProc   = ciButton.pfnWindowProc;
            pCBZShare->pMenuOrigWndProc     = ciMenu.pfnWindowProc;
            pCBZShare->pStaticOrigWndProc   = ciStatic.pfnWindowProc;
            pCBZShare->pNotebookOrigWndProc = ciNotebook.pfnWindowProc; // This does not hurt
            pCBZShare->pScrollBarOrigWndProc = ciScrollBar.pfnWindowProc; // This does not hurt
            pCBZShare->pContainerOrigWndProc = ciContainer.pfnWindowProc; // This does not hurt

            // register titlebar class
            if (!WinRegisterClass(NULLHANDLE,
                                  WC_TITLEBAR,
                                  CandyBarZTBProc,
                                  ciTB.flClassStyle | CS_PUBLIC,
                                  ciTB.cbWindowData))
                 return (0);


            // register frame class
            if (!WinRegisterClass(NULLHANDLE,
                                  WC_FRAME,
                                  CandyBarZFrameProc,
                                  ciFrame.flClassStyle | CS_PUBLIC,
                                  ciFrame.cbWindowData + sizeof(void *) + sizeof(PRIVATEFRAMEDATA)))
                 return (0);

            ulOffsetPrivateData=ciFrame.cbWindowData+sizeof(void*);

            // register button class
            if (!WinRegisterClass(NULLHANDLE,
                                  WC_BUTTON,
                                  CandyBarZButtonProc,
                                  ciButton.flClassStyle | CS_PUBLIC,
                                  ciButton.cbWindowData))
                 return (0);

            // register menu class
            if (!WinRegisterClass(NULLHANDLE,
                                  WC_MENU,
                                  CandyBarZMenuProc,
                                  ciMenu.flClassStyle | CS_PUBLIC,
                                  ciMenu.cbWindowData))
                 return (0);

            // register static class
            if (!WinRegisterClass(NULLHANDLE,
                                  WC_STATIC,
                                  CandyBarZStaticProc,
                                  ciStatic.flClassStyle | CS_PUBLIC,
                                  ciStatic.cbWindowData))
                 return (0);

            // Since we can't always query the classinfo we check if we can replace this class
            if(bNotebook) {
              // register notebook class
              if (!WinRegisterClass(NULLHANDLE,
                                    WC_NOTEBOOK,
                                    CandyBarZNotebookProc,
                                    ciNotebook.flClassStyle | CS_PUBLIC,
                                    ciNotebook.cbWindowData)) {
                return (0);
              }
            }

            // Since we can't always query the classinfo we check if we can replace this class
            if(bScrollBar) {
              // register notebook class
              if (!WinRegisterClass(NULLHANDLE,
                                    WC_SCROLLBAR,
                                    CandyBarZScrollBarProc,
                                    ciScrollBar.flClassStyle | CS_PUBLIC,
                                    ciScrollBar.cbWindowData)) {
                return (0);
              }
            }

            // Since we can't always query the classinfo we check if we can replace this class
            if(bContainer) {
              // register notebook class
              if (!WinRegisterClass(NULLHANDLE,
                                    WC_CONTAINER,
                                    CandyBarZContainerProc,
                                    ciContainer.flClassStyle | CS_PUBLIC,
                                    ciContainer.cbWindowData)) {
                return (0);
              }
            }

     
            /* Copy to local CBZShare. Will be a problem for on the fly changes!!! */
            memcpy(&CBZShare,pCBZShare,sizeof(CBZSHARE));

            lcxIcon=WinQuerySysValue(HWND_DESKTOP,SV_CXICON);
            lcyIcon=WinQuerySysValue(HWND_DESKTOP,SV_CYICON);
            lcxPointer=WinQuerySysValue(HWND_DESKTOP,SV_CXPOINTER);
            lcyPointer=WinQuerySysValue(HWND_DESKTOP,SV_CYPOINTER);

            PrfCloseProfile(hIni);
            
        }
        break;

    case 1:                    // termination

        {

            // get shared mem
            if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ | PAG_WRITE)
                    != NO_ERROR)
            {
                return (0);
            }

            // free it
            DosFreeMem(pCBZShare);

            // terminate c runtime
            _CRT_term();
        }
        break;
    }
    return (1);
}

/*************************************************************************************************************

    $Id: candybarz.c,v 1.7 2000/09/24 20:45:44 cwohlgemuth Exp $

    $Log: candybarz.c,v $
    Revision 1.7  2000/09/24 20:45:44  cwohlgemuth
    Query of icon sizes on startup added.

    Revision 1.6  2000/09/09 22:44:51  cwohlgemuth
    Support for scrollbars and containers added

    Revision 1.5  2000/09/06 14:33:18  cwohlgemuth
    Added support for notebook procedure

    Revision 1.4  2000/08/31 17:14:41  cwohlgemuth
    Started changes for speed improvement.

    Revision 1.3  2000/05/10 15:40:53  cwohlgemuth
    candybarz static control procedure added (CW)

    Revision 1.2  1999/12/29 19:38:08  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:47:56  ktk
    Import

    Revision 1.6  1998/09/12 10:02:39  pgarner
    Added copyright notice

    Revision 1.5  1998/09/12 07:35:44  mwagner
    Added support for bitmap images

    Revision 1.2  1998/06/29 15:32:59  mwagner
    Matts S3 and PMView fixes

    Revision 1.1  1998/06/08 14:18:56  pgarner
    Initial Checkin

*************************************************************************************************************/
