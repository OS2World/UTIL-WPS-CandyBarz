/* $Id: candybarzcommon.c,v 1.4 2000/09/09 22:45:51 cwohlgemuth Exp $ */

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

HWND CandyBarZGetTopLevelFrameHWND(HWND hwnd)
{
    HWND hwnd1, hwnd2, hwndDesktop;
    HAB hab = WinQueryAnchorBlock(HWND_DESKTOP);
    hwndDesktop = WinQueryDesktopWindow(hab, NULLHANDLE);

    hwnd1 = hwnd2 = hwnd;
    while ( (hwnd1 != hwndDesktop) && WinIsWindow(hab, hwnd1) )
    {
       hwnd2 = hwnd1;
       hwnd1 = WinQueryWindow(hwnd1, QW_PARENT);
    }

    return hwnd2;

}

BOOL CandyBarZWindowCreate(HWND hwnd)
{
    CBZDATA *pCBZData = NULL;
    CBZSHARE *pCBZShare = NULL;
    CBZSHARE *pPluginSet = NULL;  //Used for checking for custom config options
    BOOL bUseDef = FALSE;
    HAB hab = WinQueryAnchorBlock(hwnd);
    PPIB ppib = NULL;
    PTIB ptib = NULL;
    char *pszFile, *pszBase;
    int i;
    ULONG cbPluginSet;          //stores sizeof PLUGINSET structure.
    HINI hIni;
    char szError[32];
    PINITFN pInitFn;
    char szName[CCHMAXPATH];
    ULONG ulDataOffset;
    char szClass[10];

    // get shared mem
    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ) == NO_ERROR)
    {
        ulDataOffset = pCBZShare->ulDataOffset;
        if ( (pCBZData = (CBZDATA *)WinQueryWindowPtr(hwnd, ulDataOffset)) != NULL)
        {
            //already defined, so don't reallocate
            DosFreeMem(pCBZShare);
            return (TRUE);
        }
    }
    else
        return FALSE;

    // alloc profile name to keep it off the stack
    if (DosAllocMem((PPVOID) & pszFile, CCHMAXPATH, PAG_READ | PAG_WRITE | PAG_COMMIT)
            != NO_ERROR)
    {
        DosFreeMem(pCBZShare);
        return (FALSE);
    }
    // alloc base path name to keep it off the stack
    if (DosAllocMem((PPVOID) & pszBase, CCHMAXPATH, PAG_READ | PAG_WRITE | PAG_COMMIT)
            != NO_ERROR)
    {
        DosFreeMem(pszFile);
        DosFreeMem(pCBZShare);
        return (FALSE);
    }

    // alloc window words
    if (DosAllocMem((PPVOID) & pCBZData,
                    sizeof(CBZDATA),
                    PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        WinSetWindowPtr(hwnd, ulDataOffset, NULL);
        DosFreeMem(pszFile);
        DosFreeMem(pszBase);
        DosFreeMem(pCBZShare);
        return (FALSE);
    }
    memset(pCBZData, 0, sizeof(CBZDATA));

    pCBZData->bEnabled = TRUE;  // assume enabled

    strcpy(pCBZData->szWPSViewText, "");  // assume NOT WPS window

    WinSetWindowPtr(hwnd, ulDataOffset, pCBZData);  // store window words

    strcpy(szName, ""); //assume not custom.

    // query for custom config
    cbPluginSet = sizeof(CBZSHARE);
    if (DosAllocMem((PPVOID) & pPluginSet, cbPluginSet, PAG_READ | PAG_WRITE | PAG_COMMIT)
            != NO_ERROR)
    {
        DosFreeMem(pszFile);
        DosFreeMem(pszBase);
        DosFreeMem(pCBZShare);
        return (FALSE);
    }
    memset(pPluginSet, 0, cbPluginSet);
    // get profile name
    if (PrfQueryProfileString(HINI_USERPROFILE,
                              "CandyBarZ",
                              "Profile",
                              NULL,
                              pszFile,
                              CCHMAXPATH))
    {
        if ((hIni = PrfOpenProfile(hab, pszFile)) != NULLHANDLE)
        {
            // find out executable name to locate custom config
            if (DosGetInfoBlocks(&ptib, &ppib) == NO_ERROR)
            {
                _splitpath(ppib->pib_pchcmd, NULL, NULL, pszFile, NULL);
                for (i = 0; pszFile[i]; i++)
                {
                    if (pszFile[i] >= 'a' && pszFile[i] <= 'z')
                    {
                        pszFile[i] -= 0x20;
                    }
                }

                strcpy(pszBase, pszFile);
                strcat(pszBase, "_");
                strcat(pszBase, "DefaultPlugins");
                // see if custom config exists for this app
                if (!PrfQueryProfileData(hIni, "CustomOptionsData", pszBase, pPluginSet, &cbPluginSet))
                {
                    bUseDef = TRUE;
                }
                else
                {
                    // see if we're enabled
                    if (!pPluginSet->plugins.bEnabled)
                    {
                        pCBZData->bEnabled = FALSE;
                        WinSetWindowPtr(hwnd, ulDataOffset, pCBZData);  // store window words
                        PrfCloseProfile(hIni);
                        DosFreeMem(pPluginSet);
                        DosFreeMem(pszFile);
                        DosFreeMem(pszBase);
                        DosFreeMem(pCBZShare);
                        return FALSE;
                    }
                    else if (!pPluginSet->plugins.bUseCust)
                    {
                        bUseDef = TRUE;
                    }
                    else

                    {
                        strcpy(szName, pszFile);
                        bUseDef = FALSE;
                    }

                }
            }
            else        // couldn't get info blocks.  use default
            {
                bUseDef = TRUE;
            }
        }
        else            // couldn't open profile.  use default
        {
            bUseDef = TRUE;
        }
    }
    else                // couldn't get profile name.  disable CandyBarZ
    {
        bUseDef = TRUE;
    }


    // if using defaults, pull from shared mem
    if (bUseDef)
    {
        //Copy The Plugin DLL Names from SharedMem to the Window's Window-Words.
        pCBZData->bEnabled = pCBZShare->plugins.bEnabled;
        pCBZData->bUseCust = FALSE;

        pCBZData->cbPlugins = pCBZShare->plugins.cbCount;

        pCBZData->bTitlebarEnabled         = pCBZShare->plugins.bTitlebarEnabled;
        pCBZData->sTitlebarIndex           = pCBZShare->plugins.sTitlebarIndex;
        pCBZData->bFrameBackgroundEnabled  = pCBZShare->plugins.bFrameBackgroundEnabled;
        pCBZData->sFrameBackgroundIndex    = pCBZShare->plugins.sFrameBackgroundIndex;
        pCBZData->bFrameBorderEnabled      = pCBZShare->plugins.bFrameBorderEnabled;
        pCBZData->sFrameBorderIndex        = pCBZShare->plugins.sFrameBorderIndex;
        pCBZData->bPushButtonEnabled       = pCBZShare->plugins.bPushButtonEnabled;
        pCBZData->sPushButtonIndex         = pCBZShare->plugins.sPushButtonIndex;
        pCBZData->bRadioButtonEnabled      = pCBZShare->plugins.bRadioButtonEnabled;
        pCBZData->sRadioButtonIndex        = pCBZShare->plugins.sRadioButtonIndex;
        pCBZData->bCheckBoxEnabled         = pCBZShare->plugins.bCheckBoxEnabled;
        pCBZData->sCheckBoxIndex           = pCBZShare->plugins.sCheckBoxIndex;
        pCBZData->bMinMaxEnabled           = pCBZShare->plugins.bMinMaxEnabled;
        pCBZData->sMinMaxIndex             = pCBZShare->plugins.sMinMaxIndex;
        pCBZData->bMenuEnabled             = pCBZShare->plugins.bMenuEnabled;
        pCBZData->sMenuIndex               = pCBZShare->plugins.sMenuIndex;

        pCBZData->bNoteBookBackgroundEnabled = pCBZShare->plugins.bNoteBookBackgroundEnabled;
        pCBZData->sNoteBookBackgroundIndex   = pCBZShare->plugins.sNoteBookBackgroundIndex;
        pCBZData->bScrollBarEnabled        = pCBZShare->plugins.bScrollBarEnabled;
        pCBZData->sScrollBarIndex          = pCBZShare->plugins.sScrollBarIndex;
        pCBZData->bContainerEnabled        = pCBZShare->plugins.bContainerEnabled;
        pCBZData->sContainerIndex          = pCBZShare->plugins.sContainerIndex;

        for (i = 0; i < (pCBZData->cbPlugins); i++)
        {
            strcpy(pCBZData->Plugins[i].szPluginDLL, pCBZShare->plugins.pluginDLL[i]);
        }
    }
    else
    {
        //Copy The Plugin DLL Names from CustomPlugin Structure to the Window's Window-Words.
        pCBZData->cbPlugins = pPluginSet->plugins.cbCount;
        pCBZData->bEnabled = pPluginSet->plugins.bEnabled;
        pCBZData->bUseCust = pPluginSet->plugins.bUseCust;

        pCBZData->bTitlebarEnabled         = pPluginSet->plugins.bTitlebarEnabled;
        pCBZData->sTitlebarIndex           = pPluginSet->plugins.sTitlebarIndex;
        pCBZData->bFrameBackgroundEnabled  = pPluginSet->plugins.bFrameBackgroundEnabled;
        pCBZData->sFrameBackgroundIndex    = pPluginSet->plugins.sFrameBackgroundIndex;
        pCBZData->bFrameBorderEnabled      = pPluginSet->plugins.bFrameBorderEnabled;
        pCBZData->sFrameBorderIndex        = pPluginSet->plugins.sFrameBorderIndex;
        pCBZData->bPushButtonEnabled       = pPluginSet->plugins.bPushButtonEnabled;
        pCBZData->sPushButtonIndex         = pPluginSet->plugins.sPushButtonIndex;
        pCBZData->bRadioButtonEnabled      = pPluginSet->plugins.bRadioButtonEnabled;
        pCBZData->sRadioButtonIndex        = pPluginSet->plugins.sRadioButtonIndex;
        pCBZData->bCheckBoxEnabled         = pPluginSet->plugins.bCheckBoxEnabled;
        pCBZData->sCheckBoxIndex           = pPluginSet->plugins.sCheckBoxIndex;
        pCBZData->bMinMaxEnabled           = pPluginSet->plugins.bMinMaxEnabled;
        pCBZData->sMinMaxIndex             = pPluginSet->plugins.sMinMaxIndex;
        pCBZData->bMenuEnabled             = pPluginSet->plugins.bMenuEnabled;
        pCBZData->sMenuIndex               = pPluginSet->plugins.sMenuIndex;

        pCBZData->bNoteBookBackgroundEnabled = pCBZShare->plugins.bNoteBookBackgroundEnabled;
        pCBZData->sNoteBookBackgroundIndex   = pCBZShare->plugins.sNoteBookBackgroundIndex;
        pCBZData->bScrollBarEnabled        = pCBZShare->plugins.bScrollBarEnabled;
        pCBZData->sScrollBarIndex          = pCBZShare->plugins.sScrollBarIndex;
        pCBZData->bContainerEnabled        = pCBZShare->plugins.bContainerEnabled;
        pCBZData->sContainerIndex          = pCBZShare->plugins.sContainerIndex;

        for (i = 0; i < pCBZData->cbPlugins; i++)
        {
            strcpy(pCBZData->Plugins[i].szPluginDLL, pPluginSet->plugins.pluginDLL[i]);
        }
    }


    //probably should check the return code on the following.  If BasePath isn't found, we will be stuck!
    //so set pData->cbCount to 0 and pData->bEnabled to FALSE;
    // Get path to the plugin directory
    PrfQueryProfileString(HINI_USERPROFILE,
                          "CandyBarZ",
                          "BasePath",
                          NULL,
                          pszBase,
                          CCHMAXPATH);
    strcat(pszBase, "plugins");

    // fill in pData structure for each plugin
    for (i = 0; i < (pCBZData->cbPlugins); i++)
    {
        if (i < pCBZData->sFrameBackgroundIndex)
            strcpy(szClass, "TITLEBAR");
        else if (i < pCBZData->sFrameBorderIndex)
            strcpy(szClass, "FRAME");
        else if (i < pCBZData->sPushButtonIndex)
            strcpy(szClass, "FRAMEBRDR");
        else if (i < pCBZData->sRadioButtonIndex)
            strcpy(szClass, "PUSHBUTTON");
        else if (i < pCBZData->sCheckBoxIndex)
            strcpy(szClass, "RADIOBUTTON");
        else if (i < pCBZData->sMinMaxIndex)
            strcpy(szClass, "CHECKBOX");
        else if (i < pCBZData->sMenuIndex)
            strcpy(szClass, "MINMAX");
        else if (i < pCBZData->sNoteBookBackgroundIndex)
            strcpy(szClass, "MENU");
        else if (i < pCBZData->sScrollBarIndex)
            strcpy(szClass, "NOTEBOOK");
        else if (i < pCBZData->sContainerIndex)
            strcpy(szClass, "SCROLLBAR");
        else
          strcpy(szClass, "CONTAINER");

        //Initialize to some initial values...
        pCBZData->Plugins[i].bActiveEnabled = TRUE;
        pCBZData->Plugins[i].bInactiveEnabled = TRUE;
        pCBZData->Plugins[i].pData = NULL;
        pCBZData->Plugins[i].cbData = 0;

        //plugins might not be in LIBPATH, create full path here.
        _makepath(pszFile, NULL, pszBase, pCBZData->Plugins[i].szPluginDLL, "DLL");

        //Load the Plugin Module
        if (DosLoadModule(szError, sizeof(szError), pszFile,
                        &(pCBZData->Plugins[i].hModDll)) != NO_ERROR)
        {
            //Error loading the dll...disable this plugin!
            pCBZData->Plugins[i].bActiveEnabled = FALSE;
            pCBZData->Plugins[i].bInactiveEnabled = FALSE;
            continue;   //end this iteration...try next plugin.

        }

        //Query The Plugin's CBZInit Function
        if (DosQueryProcAddr(pCBZData->Plugins[i].hModDll, 0, "CBZInit",
                             (PFN *) & (pInitFn)) != NO_ERROR)
        {
            //error in Plugin's initialization...disable the plugin!
            pCBZData->Plugins[i].bActiveEnabled = FALSE;
            pCBZData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pCBZData->Plugins[i].hModDll);
            continue;   //end this iteration..try next plugin.

        }

        //Call the plugins Init function.
        if (!pInitFn(hIni, szName, szClass, (PVOID *) & (pCBZData->Plugins[i].pData)))
        {
            //error in Plugin's initialization...disable the plugin!
            pCBZData->Plugins[i].bActiveEnabled = FALSE;
            pCBZData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pCBZData->Plugins[i].hModDll);
            continue;   //end this iteration..try next plugin.
        }

        //Query the Plugin's Window Procedure.
        if (DosQueryProcAddr(pCBZData->Plugins[i].hModDll, 0, "CBZPluginWndProc",
        (PFN *) & (pCBZData->Plugins[i].pPluginWndProc)) != NO_ERROR)
        {
            //error getting the Plugin's WndProc...disable the plugin!
            pCBZData->Plugins[i].bActiveEnabled = FALSE;
            pCBZData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pCBZData->Plugins[i].hModDll);
            continue;   //end this iteration...try next plugin.

        }

        //Query the address of the plugin's Render function.
        if (DosQueryProcAddr(pCBZData->Plugins[i].hModDll, 0, "CBZPluginRender",
        (PFN *) & (pCBZData->Plugins[i].pfnPluginRender)) != NO_ERROR)
        {
            //Error getting CBZPluginRender function!
            pCBZData->Plugins[i].bActiveEnabled = FALSE;
            pCBZData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pCBZData->Plugins[i].hModDll);
            DosFreeMem(pCBZData->Plugins[i].pData);
            continue;   //end this iteration...try next plugin.

        }
    }                   //end active plugin for(..) loop

    // free up profile buffer
    DosFreeMem(pszFile);
    DosFreeMem(pszBase);
    DosFreeMem(pPluginSet);
    DosFreeMem(pCBZShare);
    PrfCloseProfile(hIni);

    return TRUE;
}


BOOL CandyBarZSettingsChange(HWND hwnd)
{
    CBZDATA  *pCBZData = NULL;
    CBZSHARE *pCBZShare = NULL;

    // get shared mem
    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ) == NO_ERROR)
    {
        if ( (pCBZData = (CBZDATA *)WinQueryWindowPtr(hwnd, pCBZShare->ulDataOffset)) != NULL)
        {
            //already defined, so free old data
            CandyBarZDestroyWindowData(hwnd);
        }
    }
    DosFreeMem(pCBZShare);

    return CandyBarZWindowCreate(hwnd);
}


BOOL CandyBarZDestroyWindowData(HWND hwnd)
{
    CBZDATA *pCBZData = NULL;
    CBZSHARE *pCBZShare = NULL;
    int i;
    PDESTROYFN pDestroyFn;

    // get shared data
    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ | PAG_WRITE)
            != NO_ERROR)
    {
        return (FALSE);
    }

    // get window words
    if ((pCBZData = WinQueryWindowPtr(hwnd, pCBZShare->ulDataOffset)) != NULL)
    {
        WinSetWindowPtr(hwnd, pCBZShare->ulDataOffset, NULL);

        // free plugin resources
        for (i = 0; i < pCBZData->cbPlugins; i++)
        {
            //Query The Plugin's CBZDestroy Function
            if (DosQueryProcAddr(pCBZData->Plugins[i].hModDll, 0, "CBZDestroy",
                                 (PFN *) & (pDestroyFn)) == NO_ERROR)
            {
                //Call the plugins destroy function.
                pDestroyFn(hwnd, pCBZData->Plugins[i].pData);
            }

            if (pCBZData->Plugins[i].hModDll != NULLHANDLE)
                DosFreeModule(pCBZData->Plugins[i].hModDll);
        }

        // free window words
        DosFreeMem(pCBZData);
    }

    DosFreeMem(pCBZShare);

    return TRUE;
}





