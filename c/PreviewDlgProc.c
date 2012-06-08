/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "setup.h"

// dialog proc for default colors pages of notebook
MRESULT EXPENTRY PreviewDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    char *szFile;

    switch (msg)
    {
        case CBZ_UPDATEPREVIEW:
        {
            szFile = (char *) mp1;

            ApplyPreviewTheme(hwnd, szFile);
        }
        break;
    }
    return (WinDefDlgProc(hwnd, msg, mp1, mp2));
}

// try loading each plugin module.
// if opened sucessfully, call the plugin's CBZParseAttributes(hIni, keyValue, count) function
// the plugin is responsible for writing data to the ini file.
BOOL ApplyPreviewTheme(HWND hwndPreview, char themeFile[])
{
    HINI hIni;
    int count = 0;
    char szProfile[CCHMAXPATH + 1];
    char szPluginBase[CCHMAXPATH];

    // get profile name
    if (!PrfQueryProfileString(HINI_USERPROFILE,
                               "CandyBarZ",
                               "Profile",
                               NULL,
                               szProfile,
                               CCHMAXPATH))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyPreviewTheme", "Failed to Get Profile Filename", 0UL);
        return FALSE;
    }

    // get profile name
    if (!PrfQueryProfileString(HINI_USERPROFILE,
                               "CandyBarZ",
                               "BasePath",
                               NULL,
                               szPluginBase,
                               CCHMAXPATH))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyPreviewTheme", "Failed to Get BasePath", 0UL);
        return FALSE;
    }


    if ((hIni = PrfOpenProfile(WinQueryAnchorBlock(hwndPreview), szProfile)) == NULLHANDLE)
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyPreviewTheme", "Failed to Open Profile", 0UL);
        return FALSE;
    }

    count = ApplyPreviewBlock(hwndPreview, themeFile, "TITLEBAR", szPluginBase, hIni, 0);
    count = ApplyPreviewBlock(hwndPreview, themeFile, "FRAME", szPluginBase, hIni, count);
    count = ApplyPreviewBlock(hwndPreview, themeFile, "FRAMEBRDR", szPluginBase, hIni, count);
    count = ApplyPreviewBlock(hwndPreview, themeFile, "PUSHBUTTON", szPluginBase, hIni, count);
    count = ApplyPreviewBlock(hwndPreview, themeFile, "RADIOBUTTON", szPluginBase, hIni, count);
    count = ApplyPreviewBlock(hwndPreview, themeFile, "CHECKBOX", szPluginBase, hIni, count);
    count = ApplyPreviewBlock(hwndPreview, themeFile, "MINMAX", szPluginBase, hIni, count);
    count = ApplyPreviewBlock(hwndPreview, themeFile, "MENU", szPluginBase, hIni, count);
    //insert here!!!!
    PrfCloseProfile(hIni);
    return TRUE;
}



int ApplyPreviewBlock(HWND hwnd, char themeFile[], char szClass[], char szBase[], HINI hIni, int count)
{
    CBZSHARE *pCBZShare;
    CBZDATA *pCBZData;
    PLUGIN_LIST List;
    PAPPLYFN pApplyFn;
    PINITFN  pInitFn;
    char szError[CCHMAXPATH];
    char szProfile[CCHMAXPATH + 1];
    char szFile[CCHMAXPATH + 1];
    char szPluginBase[CCHMAXPATH + 1];
    char *tempPtr;
    int i,j;

    // get it for writing, so let's hurry
    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ)
            != NO_ERROR)
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyPreviewBlock", "Failed to Get Shared Memory", 0UL);
        return count;
    }

    if ((pCBZData = WinQueryWindowPtr(hwnd, pCBZShare->ulDataOffset)) == NULL)
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "Couldn't get Window Data", szFile, 0UL);
        DosFreeMem(pCBZShare);
        return count;
    }
    DosFreeMem(pCBZShare);


    //update the shared memory.
    if (strcmp(szClass, "TITLEBAR") == 0)
        pCBZData->sTitlebarIndex = count;
    else if (strcmp(szClass, "FRAME") == 0)
        pCBZData->sFrameBackgroundIndex = count;
    else if (strcmp(szClass, "FRAMEBRDR") == 0)
        pCBZData->sFrameBorderIndex = count;
    else if (strcmp(szClass, "PUSHBUTTON") == 0)
        pCBZData->sPushButtonIndex = count;
    else if (strcmp(szClass, "RADIOBUTTON") == 0)
        pCBZData->sRadioButtonIndex = count;
    else if (strcmp(szClass, "CHECKBOX") == 0)
        pCBZData->sCheckBoxIndex = count;
    else if (strcmp(szClass, "MINMAX") == 0)
        pCBZData->sMinMaxIndex = count;
    else if (strcmp(szClass, "MENU") == 0)
        pCBZData->sMenuIndex = count;


    if (!runParser(themeFile, szClass, &List, szError))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyBlock", szError, 0UL);
        return count;
    }

    if (List.cbPlugins <= 0)
        return count;


    pCBZData->cbPlugins = count + List.cbPlugins;

    if (List.cbPlugins > 0)
    {
        if (strcmp(szClass, "TITLEBAR") == 0)
            pCBZData->bTitlebarEnabled = TRUE;
        else if (strcmp(szClass, "FRAME") == 0)
            pCBZData->bFrameBackgroundEnabled = TRUE;
        else if (strcmp(szClass, "FRAMEBRDR") == 0)
            pCBZData->bFrameBorderEnabled = TRUE;
        else if (strcmp(szClass, "PUSHBUTTON") == 0)
            pCBZData->bPushButtonEnabled = TRUE;
        else if (strcmp(szClass, "RADIOBUTTON") == 0)
            pCBZData->bRadioButtonEnabled = TRUE;
        else if (strcmp(szClass, "CHECKBOX") == 0)
            pCBZData->bCheckBoxEnabled = TRUE;
        else if (strcmp(szClass, "MINMAX") == 0)
            pCBZData->bMinMaxEnabled = TRUE;
        else if (strcmp(szClass, "MENU") == 0)
            pCBZData->bMenuEnabled = TRUE;
    }
    else
    {
        if (strcmp(szClass, "TITLEBAR") == 0)
            pCBZData->bTitlebarEnabled = FALSE;
        else if (strcmp(szClass, "FRAME") == 0)
            pCBZData->bFrameBackgroundEnabled = FALSE;
        else if (strcmp(szClass, "FRAMEBRDR") == 0)
            pCBZData->bFrameBorderEnabled = FALSE;
        else if (strcmp(szClass, "PUSHBUTTON") == 0)
            pCBZData->bPushButtonEnabled = FALSE;
        else if (strcmp(szClass, "RADIOBUTTON") == 0)
            pCBZData->bRadioButtonEnabled = FALSE;
        else if (strcmp(szClass, "CHECKBOX") == 0)
            pCBZData->bCheckBoxEnabled = FALSE;
        else if (strcmp(szClass, "MINMAX") == 0)
            pCBZData->bMinMaxEnabled = FALSE;
        else if (strcmp(szClass, "MENU") == 0)
            pCBZData->bMenuEnabled = FALSE;
    }


    strcpy(szPluginBase, szBase);
    strcat(szPluginBase, "plugins\\");


    for (j = 0; j < List.cbPlugins; j++)
    {
        //copy plugin names to shared memory.
        strcpy(pCBZData->Plugins[count + j].szPluginDLL,
               List.pPluginAttributes[j].szPluginName);

        for (i = 0; i < List.pPluginAttributes[j].cbAttributes; i++)
        {
            //If there is a %CBZBaseDir% in the attribute, replace it with the basePath
            if (strstr(List.pPluginAttributes[j].keyValue[i].value, "%CBZBaseDir%") != NULL)
            {
                //%CBZBaseDir% must be at the start of the string
                //allocate space for the new string
                tempPtr = (char *) malloc(sizeof(char) * (strlen(szBase) + strlen(List.pPluginAttributes[j].keyValue[i].value) - strlen("%CBZBaseDir%") + 1));
                tempPtr[0] = '\0';
                strcat(tempPtr, szBase);
                sscanf(List.pPluginAttributes[j].keyValue[i].value, "%%CBZBaseDir%%\\%s", szProfile);
                strcat(tempPtr, szProfile);
                free(List.pPluginAttributes[j].keyValue[i].value);
                //replace the old string with the new string.
                List.pPluginAttributes[j].keyValue[i].value = tempPtr;
            }
        }

        //plugins might not be in LIBPATH, create full path here.
        _makepath(szFile, NULL, szPluginBase, List.pPluginAttributes[j].szPluginName, "DLL");

        if (DosLoadModule(szError, sizeof(szError), szFile, &(pCBZData->Plugins[count + j].hModDll)) == NO_ERROR)
        {

            //Query The Plugin's CBZPluginDataSize Function
            if (DosQueryProcAddr(pCBZData->Plugins[count + j].hModDll, 0, "CBZInit",
                                 (PFN *) & (pInitFn)) != NO_ERROR)
            {
                //error in Plugin's initialization...disable the plugin!
                pCBZData->Plugins[count + j].bActiveEnabled = FALSE;
                pCBZData->Plugins[count + j].bInactiveEnabled = FALSE;
                DosFreeModule(pCBZData->Plugins[count + j].hModDll);
                continue;           //end this iteration..try next plugin.

            }
            //Call the Plugins Init function.
            if (!pInitFn(hIni, "", szClass, (PVOID *) & (pCBZData->Plugins[count + j].pData)))
            {
                //error in Plugin's initialization...disable the plugin!
                pCBZData->Plugins[count + j].bActiveEnabled = FALSE;
                pCBZData->Plugins[count + j].bInactiveEnabled = FALSE;
                DosFreeModule(pCBZData->Plugins[count + j].hModDll);
                continue;           //end this iteration..try next plugin.

            }

            //Query the Plugin's Window Procedure.
            if (DosQueryProcAddr(pCBZData->Plugins[count + j].hModDll, 0, "CBZPluginWndProc",
                    (PFN *) & (pCBZData->Plugins[count + j].pPluginWndProc)) != NO_ERROR)
            {
                //error getting the Plugin's WndProc...disable the plugin!
                pCBZData->Plugins[count + j].bActiveEnabled = FALSE;
                pCBZData->Plugins[count + j].bInactiveEnabled = FALSE;
                DosFreeModule(pCBZData->Plugins[count + j].hModDll);
                DosFreeMem(pCBZData->Plugins[count + j].pData);
                continue;           //end this iteration...try next plugin.

            }

            //Query the address of the plugin's Render function.
            if (DosQueryProcAddr(pCBZData->Plugins[count + j].hModDll, 0, "CBZPluginRender",
                   (PFN *) & (pCBZData->Plugins[count + j].pfnPluginRender)) != NO_ERROR)
            {
                //Error getting CBZPluginRender function!
                pCBZData->Plugins[count + j].bActiveEnabled = FALSE;
                pCBZData->Plugins[count + j].bInactiveEnabled = FALSE;
                DosFreeModule(pCBZData->Plugins[count + j].hModDll);
                DosFreeMem(pCBZData->Plugins[count + j].pData);
                continue;           //end this iteration...try next plugin.

            }



            if (DosQueryProcAddr(pCBZData->Plugins[count + j].hModDll, 0, "CBZApply", (PFN *) & pApplyFn) == NO_ERROR)
            {
                //check return code from this to make sure it was sucessful.. if not disable the plugin
                pApplyFn(szClass, (List.pPluginAttributes[j].keyValue),
                         List.pPluginAttributes[j].cbAttributes,
                         List.pPluginAttributes[j].enabledState,
                         pCBZData->Plugins[count + j].pData);
            }
            else
            {
                PSUTErrorFunc(NULLHANDLE, "Error", "ApplyPreviewBlock", "Failed Querying Procedure Address for CBZApply", 0UL);
            }
        }
        else
        {
            PSUTErrorFunc(NULLHANDLE, "Error", "ApplyBlock", "Error Querying Plugin Module.. (Missing?)", 0UL);
            pCBZData->cbPlugins = j - 1;
            break;
            //disable it here
        }

    }


    i = List.cbPlugins;
    //delete all the attributes in the plugin/key/value list
    CBZFreePLUGIN_LIST(&List);
    return count + i;
}

#if 0
BOOL _ApplyPreviewBlock(HWND hwnd, char themeFile[], char szCBZBase[], char szShare[], char szClass[], int index)
{
    PSFRAMESHARE *pFrameShare;
    PLUGIN_LIST List;
    HMODULE hMod;
    PAPPLYFN pApplyFn;
    PINITFN pInitFn;
    char szError[CCHMAXPATH];
    char szProfile[CCHMAXPATH + 1];
    char szFile[CCHMAXPATH + 1];
    char szSaveName[CCHMAXPATH + 1];
    char szPluginBase[CCHMAXPATH + 1];
    char *tempPtr;
    int i, j;
    PSTBDATA *pTBData;
    HINI hIni;
    PDESTROYFN pDestroyFn;
    ULONG ulDataOffset;

//    if (DosGetNamedSharedMem((PPVOID) & pFrameShare, PSFRAME_SHARE, PAG_READ) != NO_ERROR)
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "Couldn't get Shared Memory", szFile, 0UL);
        return FALSE;
    }
    ulDataOffset = pFrameShare->ulDataOffset + index * sizeof(void *);
    DosFreeMem(pFrameShare);

    if ((pTBData = WinQueryWindowPtr(hwnd, ulDataOffset)) == NULL)
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "Couldn't get Window Data", szFile, 0UL);
        return FALSE;
    }

    // get profile name
    if (!PrfQueryProfileString(HINI_USERPROFILE,
                               "CandyBarZ",
                               "Profile",
                               NULL,
                               szFile,
                               CCHMAXPATH))
        return FALSE;
    if ((hIni = PrfOpenProfile(WinQueryAnchorBlock(hwnd), szFile)) == NULLHANDLE)
        return FALSE;

    // free plugin resources
    for (i = 0; i < pTBData->cbPlugins; i++)
    {
        //Query The Plugin's CBZInit Function
        if (DosQueryProcAddr(pTBData->Plugins[i].hModDll, 0, "CBZDestroy",
                             (PFN *) & (pDestroyFn)) == NO_ERROR)
        {
            //Call the plugins destroy function.
            pDestroyFn(pTBData->Plugins[i].pData);
        }

        if (pTBData->Plugins[i].hModDll != NULLHANDLE)
            DosFreeModule(pTBData->Plugins[i].hModDll);
    }

    if (!runParser(themeFile, szClass, &List, szError))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyPreviewBlock", szError, 0UL);
        PrfCloseProfile(hIni);
        return FALSE;
    }

    strcpy(szPluginBase, szCBZBase);
    strcat(szPluginBase, "plugins\\");

    //update the windows memory.
    pTBData->cbPlugins = List.cbPlugins;

    //fill in ptb2 structure.
    for (i = 0; i < (pTBData->cbPlugins); i++)
    {
        //Initialize to some initial values...
        pTBData->Plugins[i].bActiveEnabled = TRUE;
        pTBData->Plugins[i].bInactiveEnabled = TRUE;
        pTBData->Plugins[i].pData = NULL;
        pTBData->Plugins[i].cbData = 0;

        //copy plugin names to shared memory.
        strcpy(pTBData->Plugins[i].szPluginDLL,
               List.pPluginAttributes[i].szPluginName);

        _makepath(szFile, NULL, szPluginBase, pTBData->Plugins[i].szPluginDLL, "DLL");
        //Load the Plugin Module
        if (DosLoadModule(szError, sizeof(szError), szFile,
                          &(pTBData->Plugins[i].hModDll)) != NO_ERROR)
        {
            //Error loading the dll...disable this plugin!
            pTBData->Plugins[i].bActiveEnabled = FALSE;
            pTBData->Plugins[i].bInactiveEnabled = FALSE;
            continue;           //end this iteration...try next plugin.

        }

        //Query The Plugin's CBZPluginDataSize Function
        if (DosQueryProcAddr(pTBData->Plugins[i].hModDll, 0, "CBZInit",
                             (PFN *) & (pInitFn)) != NO_ERROR)
        {
            //error in Plugin's initialization...disable the plugin!
            pTBData->Plugins[i].bActiveEnabled = FALSE;
            pTBData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pTBData->Plugins[i].hModDll);
            continue;           //end this iteration..try next plugin.

        }
        //Call the Plugins Init function.
        if (!pInitFn(hIni, "", szClass, (PVOID *) & (pTBData->Plugins[i].pData)))
        {
            //error in Plugin's initialization...disable the plugin!
            pTBData->Plugins[i].bActiveEnabled = FALSE;
            pTBData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pTBData->Plugins[i].hModDll);
            continue;           //end this iteration..try next plugin.

        }

        //Query the Plugin's Window Procedure.
        if (DosQueryProcAddr(pTBData->Plugins[i].hModDll, 0, "CBZPluginWndProc",
                (PFN *) & (pTBData->Plugins[i].pPluginWndProc)) != NO_ERROR)
        {
            //error getting the Plugin's WndProc...disable the plugin!
            pTBData->Plugins[i].bActiveEnabled = FALSE;
            pTBData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pTBData->Plugins[i].hModDll);
            DosFreeMem(pTBData->Plugins[i].pData);
            continue;           //end this iteration...try next plugin.

        }

        //Query the address of the plugin's Render function.
        if (DosQueryProcAddr(pTBData->Plugins[i].hModDll, 0, "CBZPluginRender",
               (PFN *) & (pTBData->Plugins[i].pfnPluginRender)) != NO_ERROR)
        {
            //Error getting CBZPluginRender function!
            pTBData->Plugins[i].bActiveEnabled = FALSE;
            pTBData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pTBData->Plugins[i].hModDll);
            DosFreeMem(pTBData->Plugins[i].pData);
            continue;           //end this iteration...try next plugin.

        }
    }                           //end active plugin for(..) loop

    for (j = 0; j < List.cbPlugins; j++)
    {

        for (i = 0; i < List.pPluginAttributes[j].cbAttributes; i++)
        {
            //If there is a %CBZBaseDir% in the attribute, replace it with the basePath
            if (strstr(List.pPluginAttributes[j].keyValue[i].value, "%CBZBaseDir%") != NULL)
            {
                //%CBZBaseDir% must be at the start of the string
                //allocate space for the new string
                tempPtr = (char *) malloc(sizeof(char) * (strlen(szCBZBase) + strlen(List.pPluginAttributes[j].keyValue[i].value) - strlen("%CBZBaseDir%") + 1));
                tempPtr[0] = '\0';
                strcat(tempPtr, szCBZBase);
                sscanf(List.pPluginAttributes[j].keyValue[i].value, "%%CBZBaseDir%%\\%s", szProfile);
                strcat(tempPtr, szProfile);
                free(List.pPluginAttributes[j].keyValue[i].value);
                //replace the old string with the new string.
                List.pPluginAttributes[j].keyValue[i].value = tempPtr;
            }
        }

        //plugins might not be in LIBPATH, create full path here.
        _makepath(szFile, NULL, szPluginBase, List.pPluginAttributes[j].szPluginName, "DLL");

        if (DosLoadModule(szError, sizeof(szError), szFile, &hMod) == NO_ERROR)
        {
            if (DosQueryProcAddr(hMod, 0, "CBZApply", (PFN *) & pApplyFn) == NO_ERROR)
            {
                if (pApplyFn == NULL)
                {
                    PSUTErrorFunc(NULLHANDLE, "Error", "ApplyPreviewBlock", "pApplyFn is NULL!", 0UL);
                }

                //check return code from this to make sure it was sucessful.. if not disable the plugin
                pApplyFn(szClass, (List.pPluginAttributes[j].keyValue),
                         List.pPluginAttributes[j].cbAttributes,
                         List.pPluginAttributes[j].enabledState,
                         pTBData->Plugins[j].pData);
            }
            else
            {
                PSUTErrorFunc(NULLHANDLE, "Error", "ApplyPreviewBlock", "Failed Querying Procedure Address for CBZApply", 0UL);
            }
            DosFreeModule(hMod);
        }
        else
        {
            PSUTErrorFunc(NULLHANDLE, "Error", "ApplyPreviewBlock", "Error Querying Plugin Module.. (Missing?)", 0UL);
            pTBData->cbPlugins = j - 1;
            break;
            //disable it here
        }

    }

    //delete all the attributes in the plugin/key/value list
    CBZFreePLUGIN_LIST(&List);

    PrfCloseProfile(hIni);
    return TRUE;
}

#endif
/*************************************************************************************************************

    $Id: PreviewDlgProc.c,v 1.2 1999/12/29 19:38:10 enorman Exp $

    $Log: PreviewDlgProc.c,v $
    Revision 1.2  1999/12/29 19:38:10  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:47:57  ktk
    Import

    Revision 1.14  1998/09/30 05:08:34  mwagner
    Made the PMView fix optional

    Revision 1.13  1998/09/22 12:12:45  mwagner
     Fixed some minor bugs

    Revision 1.10  1998/09/15 10:10:03  enorman
    Split the dlgProc into two procedures. One for the active
    page and one for the inactive page.
    Changed many references to menu related things to references to the new controls

    Revision 1.9  1998/09/12 10:06:16  pgarner
    Added copyright notice

    Revision 1.8  1998/09/12 07:35:46  mwagner
    Added support for bitmap images

    Revision 1.5  1998/08/31 19:35:41  enorman
    Finished the implementation of 3D Text

    Revision 1.4  1998/08/10 03:00:01  enorman
    Added alot of code to support 3D text

    Revision 1.3  1998/06/29 15:32:10  mwagner
    Matts S3 and PMView fixes

    Revision 1.2  1998/06/14 11:33:20  mwagner
    Added a call to 'PrfCloseProfile( hIni )' at the end of processing the
    WM_INITDLG message.

    Revision 1.1  1998/06/08 14:18:56  pgarner
    Initial Checkin

*************************************************************************************************************/
