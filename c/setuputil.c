

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

#include "setup.h"

#ifdef CBSETUP_DEBUG
void PrintSharedMemory(void)
{
    CBZSHARE *pCBZShare;
    char szText[5000];
    char szTemp[5000];
    // get it for writing, so let's hurry
    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ | PAG_WRITE)
            != NO_ERROR)
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "PrintSharedMemory", "Failed to Get Shared Memory", 0UL);
        return;
    }

    sprintf(szText, "cbCount = %d\n", pCBZShare->plugins.cbCount);
    sprintf(szTemp, "sTitlebarIndex = %d, \n  bTitlebarEnabled = %d\n", pCBZShare->plugins.sTitlebarIndex, pCBZShare->plugins.bTitlebarEnabled);
    strcat(szText, szTemp);
    sprintf(szTemp, "sFrameBackgroundIndex = %d, \n  bFrameBackgroundEnabled = %d\n", pCBZShare->plugins.sFrameBackgroundIndex, pCBZShare->plugins.bFrameBackgroundEnabled);
    strcat(szText, szTemp);
    sprintf(szTemp, "sFrameBorderIndex = %d, \n  bFrameBorderEnabled = %d\n", pCBZShare->plugins.sFrameBorderIndex, pCBZShare->plugins.bFrameBorderEnabled);
    strcat(szText, szTemp);
    sprintf(szTemp, "sPushButtonIndex = %d, \n  bPushButtonEnabled = %d\n", pCBZShare->plugins.sPushButtonIndex, pCBZShare->plugins.bPushButtonEnabled);
    strcat(szText, szTemp);
    sprintf(szTemp, "sRadioButtonIndex = %d, \n  bRadioButtonEnabled = %d\n", 
            pCBZShare->plugins.sRadioButtonIndex, pCBZShare->plugins.bRadioButtonEnabled);
    strcat(szText, szTemp);
    sprintf(szTemp, "sCheckBoxIndex = %d, \n  bCheckBoxEnabled = %d\n", pCBZShare->plugins.sCheckBoxIndex, pCBZShare->plugins.bCheckBoxEnabled);
    strcat(szText, szTemp);
    sprintf(szTemp, "sMinMaxIndex = %d, \n  bMinMaxEnabled = %d\n", pCBZShare->plugins.sMinMaxIndex, pCBZShare->plugins.bMinMaxEnabled);
    strcat(szText, szTemp);
    sprintf(szTemp, "sMenuIndex = %d, \n  bMenuEnabled = %d\n", pCBZShare->plugins.sMenuIndex, pCBZShare->plugins.bMenuEnabled);
    strcat(szText, szTemp);
    sprintf(szTemp, "sNoteBookBackgroundIndex = %d, \n  bNoteBookBackgroundEnabled = %d\n", 
            pCBZShare->plugins.sNoteBookBackgroundIndex, pCBZShare->plugins.bNoteBookBackgroundEnabled);
    strcat(szText, szTemp);
    sprintf(szTemp, "sScrollBarIndex = %d, \n  bScrollBarEnabled = %d\n", 
            pCBZShare->plugins.sScrollBarIndex, pCBZShare->plugins.bScrollBarEnabled);
    strcat(szText, szTemp);


        PSUTErrorFunc(NULLHANDLE,
                      "Information",
                      "PrintSharedMemory",
                      szText,
                      0UL);
    DosFreeMem(pCBZShare);

}
#endif

//utility function to load a string from the string table and set the text on the given window
BOOL SetControlText(HAB hab, HWND hwndControl, int stringID)
{
    char szString[256];

    if (hwndControl == NULLHANDLE)
        return FALSE;

    if (WinLoadString(hab, NULLHANDLE, stringID, 256, szString) == 0)
        return FALSE;

    return WinSetWindowText(hwndControl, szString);
}

SaveSettingsBlock(CBZSHARE *pCBZShare, PLUGIN_LIST *pList, HINI hIni, char szClassName[], char szCustomAppName[], char szCBZBase[], char szPluginBase[], int baseIndex)
{
    PSAVEFN pSaveFn;
    PAPPLYFN pApplyFn;
    PINITFN pInitFn;
    PDESTROYFN pDestroyFn;
    char *tempPtr;
    int i, j;
    char szError[CCHMAXPATH];
    char szFile[CCHMAXPATH];
    char szProfile[CCHMAXPATH + 1];


    for (i = 0; i < pList->cbPlugins; i++)
    {
        strcpy(pCBZShare->plugins.pluginDLL[baseIndex + i], pList->pPluginAttributes[i].szPluginName);

        for (j = 0; j < pList->pPluginAttributes[i].cbAttributes; j++)
        {
            //If there is a %CBZBaseDir% in the attribute, replace it with the basePath
            if (strstr(pList->pPluginAttributes[i].keyValue[j].value, "%CBZBaseDir%") != NULL)
            {
                //%CBZBaseDir% must be at the start of the string
                //allocate space for the new string
                tempPtr = (char *) malloc(sizeof(char) * (strlen(szCBZBase) + strlen(pList->pPluginAttributes[i].keyValue[j].value) - strlen("%CBZBaseDir%") + 1));
                tempPtr[0] = '\0';
                strcat(tempPtr, szCBZBase);
                sscanf(pList->pPluginAttributes[i].keyValue[j].value, "%%CBZBaseDir%%\\%s", szProfile);
                strcat(tempPtr, szProfile);
                free(pList->pPluginAttributes[i].keyValue[j].value);
                //replace the old string with the new string.
                pList->pPluginAttributes[i].keyValue[j].value = tempPtr;
            }
        }

    }


//Save Each Plugin's settings here!
    for (i = 0; i < pList->cbPlugins; i++)
    {
        CBZDATA tempCBZData;
        //Call CBZInit
        //Initialize to some initial values...
        tempCBZData.Plugins[i].bActiveEnabled = TRUE;
        tempCBZData.Plugins[i].bInactiveEnabled = TRUE;
        tempCBZData.Plugins[i].pData = NULL;
        tempCBZData.Plugins[i].cbData = 0;

        //copy plugin names to shared memory.
        strcpy(tempCBZData.Plugins[i].szPluginDLL,
                pList->pPluginAttributes[i].szPluginName);

        _makepath(szFile, NULL, szPluginBase, tempCBZData.Plugins[i].szPluginDLL, "DLL");

        //WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,szFile,"Debug",1234,MB_OK);
        //Load the Plugin Module
        if (DosLoadModule(szError, sizeof(szError), szFile,
                      &(tempCBZData.Plugins[i].hModDll)) != NO_ERROR)
        {
            //Error loading the dll...disable this plugin!
            tempCBZData.Plugins[i].bActiveEnabled = FALSE;
            tempCBZData.Plugins[i].bInactiveEnabled = FALSE;
            continue;           //end this iteration...try next plugin.
        }

        //Query The Plugin's CBZPluginDataSize Function
        if (DosQueryProcAddr(tempCBZData.Plugins[i].hModDll, 0, "CBZInit",
                         (PFN *) & (pInitFn)) != NO_ERROR)
        {
            //error in Plugin's initialization...disable the plugin!
            tempCBZData.Plugins[i].bActiveEnabled = FALSE;
            tempCBZData.Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(tempCBZData.Plugins[i].hModDll);
            continue;           //end this iteration..try next plugin.
        }
        //Call the Plugins Init function.
        if (!pInitFn(hIni, "", szClassName, (PVOID *) & (tempCBZData.Plugins[i].pData)))
        {
            //error in Plugin's initialization...disable the plugin!
            tempCBZData.Plugins[i].bActiveEnabled = FALSE;
            tempCBZData.Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(tempCBZData.Plugins[i].hModDll);
            continue;           //end this iteration..try next plugin.
        }


        //Call CBZApply
        if (DosQueryProcAddr(tempCBZData.Plugins[i].hModDll, 0, "CBZApply", (PFN *) & pApplyFn) == NO_ERROR)
        {
            //check return code from this to make sure it was sucessful.. if not disable the plugin
            pApplyFn(szClassName, (pList->pPluginAttributes[i].keyValue),
                    pList->pPluginAttributes[i].cbAttributes,
                    pList->pPluginAttributes[i].enabledState,
                    tempCBZData.Plugins[i].pData);
        }
        else
        {
            PSUTErrorFunc(NULLHANDLE, "Error", "SaveSettings", "Failed Querying Procedure Address for CBZApply", 0UL);
        }

        //Call CBZSave
        if (DosQueryProcAddr(tempCBZData.Plugins[i].hModDll, 0, "CBZSave", (PFN *) & pSaveFn) == NO_ERROR)
        {
            pSaveFn(hIni, szClassName, tempCBZData.Plugins[i].pData, szCustomAppName);
        }
        else
        {
            PSUTErrorFunc(NULLHANDLE,
                          "Error",
                          "SaveSettings",
                          "Error Querying Procedure Address for CBZSave",
                        0UL);
        }
        //clean up here by calling CBZDestroy.
    }


}


BOOL SaveSettings(HWND hwnd, HINI hIni/*, char szClassName[], char szShareName[]*/,
                    char szCustomAppName[], char szThemeFile[])
{
    CBZSHARE *pCBZShare;
    CBZSHARE tempCBZShare;
    HMODULE hMod;
    PSAVEFN pSaveFn;
    char szClassName[15];
    char szError[CCHMAXPATH];
    char szCBZBase[CCHMAXPATH];
    char szPluginBase[CCHMAXPATH];
    char szFile[CCHMAXPATH];
    int j;

    // get shared mem
    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ) != NO_ERROR)
    {
        PSUTErrorFunc(hwnd,
                      "Error",
                      "SaveSettings",
                      "Failed to acquire shared memory",
                      0UL);
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
        PSUTErrorFunc(hwnd, "Error", "SaveSettings", "Failed to Get BasePath", 0UL);
        return FALSE;
    }
    strcpy(szCBZBase, szPluginBase);
    strcat(szPluginBase, "plugins\\");

    if (strlen(szCustomAppName) > 0)
    {
        PLUGIN_LIST List;

        memset(&tempCBZShare, 0, sizeof(CBZSHARE));
        if (WinQueryButtonCheckstate(hwnd, CHID_ENABLECB) == 1 )
            tempCBZShare.plugins.bEnabled = TRUE;
        else
            tempCBZShare.plugins.bEnabled = FALSE;

        if (WinQueryButtonCheckstate(hwnd, CHID_ENABLECUST) == 1 )
            tempCBZShare.plugins.bUseCust = TRUE;
        else
            tempCBZShare.plugins.bUseCust = FALSE;

        tempCBZShare.plugins.cbCount = 0;


        if (!runParser(szThemeFile, "TITLEBAR", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sTitlebarIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bTitlebarEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "TITLEBAR", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sTitlebarIndex);
        CBZFreePLUGIN_LIST(&List);


        if (!runParser(szThemeFile, "FRAME", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sFrameBackgroundIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bFrameBackgroundEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "FRAME", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sFrameBackgroundIndex);
        CBZFreePLUGIN_LIST(&List);


        if (!runParser(szThemeFile, "FRAMEBRDR", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sFrameBorderIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bFrameBorderEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "FRAMEBRDR", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sFrameBorderIndex);
        CBZFreePLUGIN_LIST(&List);



        if (!runParser(szThemeFile, "PUSHBUTTON", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sPushButtonIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bPushButtonEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "PUSHBUTTON", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sPushButtonIndex);
        CBZFreePLUGIN_LIST(&List);


        if (!runParser(szThemeFile, "RADIOBUTTON", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sRadioButtonIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bRadioButtonEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "RADIOBUTTON", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sRadioButtonIndex);
        CBZFreePLUGIN_LIST(&List);


        if (!runParser(szThemeFile, "CHECKBOX", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sCheckBoxIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bCheckBoxEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "CHECKBOX", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sCheckBoxIndex);
        CBZFreePLUGIN_LIST(&List);



        if (!runParser(szThemeFile, "MINMAX", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sMinMaxIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bMinMaxEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "MINMAX", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sMinMaxIndex);
        CBZFreePLUGIN_LIST(&List);




        if (!runParser(szThemeFile, "MENU", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sMenuIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bMenuEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "MENU", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sMenuIndex);
        CBZFreePLUGIN_LIST(&List);


        if (!runParser(szThemeFile, "NOTEBOOK", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sNoteBookBackgroundIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bNoteBookBackgroundEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "NOTEBOOK", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sNoteBookBackgroundIndex);
        CBZFreePLUGIN_LIST(&List);



        if (!runParser(szThemeFile, "SCROLLBAR", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sScrollBarIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bScrollBarEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "SCROLLBAR", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sScrollBarIndex);
        CBZFreePLUGIN_LIST(&List);


        if (!runParser(szThemeFile, "CONTAINER", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sContainerIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bContainerEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "CONTAINER", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sContainerIndex);
        CBZFreePLUGIN_LIST(&List);

        if (!runParser(szThemeFile, "SYSTEM", &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "ApplyBlock", szError, 0UL);
            return FALSE;
        }

        tempCBZShare.plugins.sSystemIndex = tempCBZShare.plugins.cbCount;
        tempCBZShare.plugins.cbCount += List.cbPlugins;
        if (List.cbPlugins > 0)
            tempCBZShare.plugins.bSystemEnabled = TRUE;

        SaveSettingsBlock(&tempCBZShare, &List, hIni, "SYSTEM", szCustomAppName,
                        szCBZBase, szPluginBase, tempCBZShare.plugins.sSystemIndex);
        CBZFreePLUGIN_LIST(&List);


        /* Insert your plugin here... */


        sprintf(szFile, "%s_DefaultPlugins", szCustomAppName);
        // write shared mem
        if (!PrfWriteProfileData(hIni,
                                "CustomOptionsData",
                                szFile,
                                &tempCBZShare,
                                offsetof(CBZSHARE, ulDataOffset)))
        {
            PSUTErrorFunc(hwnd,
                        "Error",
                        "SaveSettings",
                        "Failed to write profile data",
                        0UL);
            return FALSE;
        }


    }
    else  //Not CustomApp, so Save as Defaults
    {
        // write shared mem
        if (!PrfWriteProfileData(hIni,
                                "UserOptions",
                                "DefaultPlugins",
                                pCBZShare,
                                offsetof(CBZSHARE, ulDataOffset)))
        {
            PSUTErrorFunc(hwnd,
                        "Error",
                        "SaveSettings",
                        "Failed to write profile data",
                        0UL);
            DosFreeMem(pCBZShare);
            return FALSE;
        }

        //call each plugin's save procedure
        for (j = 0; j < pCBZShare->plugins.cbCount; j++)
        {
            //plugins might not be in LIBPATH, create full path here.
            _makepath(szFile, NULL, szPluginBase, pCBZShare->plugins.pluginDLL[j], "DLL");

            if (DosLoadModule(szError, sizeof(szError), szFile, &hMod) == NO_ERROR)
            {
                if (DosQueryProcAddr(hMod, 0, "CBZSave", (PFN *) & pSaveFn) == NO_ERROR)
                {
                    if (j < pCBZShare->plugins.sFrameBackgroundIndex)
                        strcpy(szClassName, "TITLEBAR");
                    else if  (j < pCBZShare->plugins.sFrameBorderIndex)
                        strcpy(szClassName, "FRAME");
                    else if (j < pCBZShare->plugins.sPushButtonIndex)
                        strcpy(szClassName, "FRAMEBRDR");
                    else if (j < pCBZShare->plugins.sRadioButtonIndex)
                        strcpy(szClassName, "PUSHBUTTON");
                    else if (j < pCBZShare->plugins.sCheckBoxIndex)
                        strcpy(szClassName, "RADIOBUTTON");
                    else if (j < pCBZShare->plugins.sMinMaxIndex)
                        strcpy(szClassName, "CHECKBOX");
                    else if (j < pCBZShare->plugins.sMenuIndex)
                        strcpy(szClassName, "MINMAX");
                    else if (j < pCBZShare->plugins.sNoteBookBackgroundIndex)
                        strcpy(szClassName, "MENU");
                    else if (j < pCBZShare->plugins.sScrollBarIndex)
                        strcpy(szClassName, "NOTEBOOK");
                    else if (j < pCBZShare->plugins.sContainerIndex)
                        strcpy(szClassName, "SCROLLBAR");
                    else if (j < pCBZShare->plugins.sSystemIndex)
                        strcpy(szClassName, "CONTAINER");
                    else //if (j < pCBZShare->plugins.)
                        strcpy(szClassName, "SYSTEM");
                        
                    pSaveFn(hIni, szClassName, NULL, "");
                }
                else
                {
                    PSUTErrorFunc(hwnd,
                                  "Error",
                                  "SaveSettings",
                                  "Error Querying Procedure Address for CBZSave",
                                0UL);
                }
                DosFreeModule(hMod);
            }
            else
            {
                PSUTErrorFunc(hwnd,
                              "Error",
                              "SaveSettings",
                              "Failed to load plugin module",
                              0UL);

            }

        }
    }

    // clean up
    DosFreeMem(pCBZShare);

    return TRUE;
}



BOOL SetCurrentThemeText(HWND hwndEntryField, char themeFile[])
{
    PLUGIN_LIST List;
    int i;
    char szError[CCHMAXPATH];
    char szThemeName[128] = "";
    char szBasePath[CCHMAXPATH];

    // get profile name
    if (!PrfQueryProfileString(HINI_USERPROFILE,
                               "CandyBarZ",
                               "BasePath",
                               NULL,
                               szBasePath,
                               CCHMAXPATH))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "SetCurrentThemeText", "Failed to Get BasePath", 0UL);
        return FALSE;
    }
    if (!strstr(themeFile, szBasePath))
        strcat(szBasePath, themeFile);
    else
        strcpy(szBasePath, themeFile);

    //call the parser to get the INFO parameters.
    if (!getThemeInfo(szBasePath, &List, szError))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "LoadPlugins", szError, 0UL);
        //free memory from all the attributes in the plugin/key/value list
        CBZFreePLUGIN_LIST(&List);

        return FALSE;
    }

    //search the list of Attributes for the "Name" attribute.
    for (i = 0; i < List.pPluginAttributes[0].cbAttributes; i++)
    {
        if (strcmp(List.pPluginAttributes[0].keyValue[i].key, "Name") == 0)
        {
            strcpy(szThemeName, List.pPluginAttributes[0].keyValue[i].value);
            break;
        }
    }

    if (szThemeName[0] == '\0') //no name found?

    {
        WinLoadString(WinQueryAnchorBlock(hwndEntryField), NULLHANDLE, IDS_NONESPECIFIED_LABEL, 256, szThemeName);
    }

    //Add the filename in paren' to the end of the Name.
    strcat(szThemeName, " (");
    //get rid of the leading path, if it exists.
    if (strchr(themeFile, '\\'))
        strcat(szThemeName, strrchr(themeFile, '\\') + 1);
    else
        strcat(szThemeName, themeFile);
    strcat(szThemeName, ")");
    //update the entryfield!
    WinSetWindowText(hwndEntryField, szThemeName);

    CBZFreePLUGIN_LIST(&List);
    return TRUE;
}

// try loading each plugin module.
// if opened sucessfully, call the plugin's CBZParseAttributes(hIni, keyValue, count) function
// the plugin is responsible for writing data to the ini file.
BOOL ApplyTheme(char themeFile[])
{
    HAB hab = WinQueryAnchorBlock(HWND_DESKTOP);
    HINI hIni;
    char szProfile[CCHMAXPATH + 1];
    char szPluginBase[CCHMAXPATH + 1];
    HATOMTBL hAtomTbl = WinQuerySystemAtomTable();
    ATOM Atom;
    int count;

    // get profile name
    if (!PrfQueryProfileString(HINI_USERPROFILE,
                               "CandyBarZ",
                               "Profile",
                               NULL,
                               szProfile,
                               CCHMAXPATH))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyTheme", "Failed to Get Profile Filename", 0UL);
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
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyTheme", "Failed to Get BasePath", 0UL);
        return FALSE;
    }

    if ((hIni = PrfOpenProfile(hab, szProfile)) == NULLHANDLE)
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyTheme", "Failed to Open Profile", 0UL);
        return FALSE;
    }

    //insert here!!!!
    count = ApplyBlock(themeFile, "TITLEBAR", szPluginBase, hIni, 0);
    count = ApplyBlock(themeFile, "FRAME", szPluginBase, hIni, count);
    count = ApplyBlock(themeFile, "FRAMEBRDR", szPluginBase, hIni, count);
    count = ApplyBlock(themeFile, "PUSHBUTTON", szPluginBase, hIni, count);
    count = ApplyBlock(themeFile, "RADIOBUTTON", szPluginBase, hIni, count);
    count = ApplyBlock(themeFile, "CHECKBOX", szPluginBase, hIni, count);
    count = ApplyBlock(themeFile, "MINMAX", szPluginBase, hIni, count);
    count = ApplyBlock(themeFile, "MENU", szPluginBase, hIni, count);
    count = ApplyBlock(themeFile, "NOTEBOOK", szPluginBase, hIni, count);
    count = ApplyBlock(themeFile, "SCROLLBAR", szPluginBase, hIni, count);
    count = ApplyBlock(themeFile, "CONTAINER", szPluginBase, hIni, count);
    count = ApplyBlock(themeFile, "SYSTEM", szPluginBase, hIni, count);

#ifdef CBSETUP_DEBUG
PrintSharedMemory();
#endif

    // locate broadcast atom
    if ((Atom = WinFindAtom(hAtomTbl, PSTB_COLORCHANGE_ATOM)) == 0)
    {
        printf("Failed to locate the COLORCHANGE atom!\n");
        return FALSE;
    }


    // broadcast to desktop children
    // mp1==0 to tell the first frame to copy data from shared mem to local DLL storage 
    if (!WinBroadcastMsg(HWND_DESKTOP,
                         Atom,
                         (MPARAM) 0,
                         (MPARAM) 0,
                         BMSG_POST | BMSG_DESCENDANTS))
    {
        printf("Failed to Broadcast Atom!\n");
        return FALSE;
    }

    // broadcast to desktop children
    // mp1==1 to tell the first frame that the message with mp1==0 should be accepted as a signal to copy the shared
    // mem to local DLL data
    if (!WinBroadcastMsg(HWND_DESKTOP,
                         Atom,
                         (MPARAM) 1,
                         (MPARAM) 0,
                         BMSG_POST | BMSG_DESCENDANTS))
    {
        printf("Failed to Broadcast Atom!\n");
        return FALSE;
    }

    // broadcast to hidden windows
    WinBroadcastMsg(HWND_OBJECT,
                    Atom,
                    (MPARAM) 0,
                    (MPARAM) 0,
                    BMSG_POST | BMSG_DESCENDANTS);

    PrfCloseProfile(hIni);
    return TRUE;
}

int ApplyBlock(char themeFile[], char szClass[], char szCBZBase[], HINI hIni, int count)
{
    CBZSHARE *pCBZShare;
    PLUGIN_LIST List;
    HMODULE hMod;
    PAPPLYFN pApplyFn;
    char szError[CCHMAXPATH];
    char szProfile[CCHMAXPATH + 1];
    char szFile[CCHMAXPATH + 1];
    char szPluginBase[CCHMAXPATH + 1];
    char *szBase, *tempPtr;
    int i,j;

    strcpy(szPluginBase, szCBZBase);
    //add the name of the themeFile to the basePath.
    strcat(szPluginBase, themeFile);

    //make a copy of the base directory
    szBase = strdup(szCBZBase);

    // get it for writing, so let's hurry
    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ | PAG_WRITE)
            != NO_ERROR)
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyBlock", "Failed to Get Shared Memory", 0UL);
        return count;
    }

    //update the shared memory.
    if (strcmp(szClass, "TITLEBAR") == 0)
        pCBZShare->plugins.sTitlebarIndex = count;
    else if (strcmp(szClass, "FRAME") == 0)
        pCBZShare->plugins.sFrameBackgroundIndex = count;
    else if (strcmp(szClass, "FRAMEBRDR") == 0)
        pCBZShare->plugins.sFrameBorderIndex = count;
    else if (strcmp(szClass, "PUSHBUTTON") == 0)
        pCBZShare->plugins.sPushButtonIndex = count;
    else if (strcmp(szClass, "RADIOBUTTON") == 0)
        pCBZShare->plugins.sRadioButtonIndex = count;
    else if (strcmp(szClass, "CHECKBOX") == 0)
        pCBZShare->plugins.sCheckBoxIndex = count;
    else if (strcmp(szClass, "MINMAX") == 0)
        pCBZShare->plugins.sMinMaxIndex = count;
    else if (strcmp(szClass, "MENU") == 0)
        pCBZShare->plugins.sMenuIndex = count;
    else if (strcmp(szClass, "NOTEBOOK") == 0)
        pCBZShare->plugins.sNoteBookBackgroundIndex = count;
    else if (strcmp(szClass, "SCROLLBAR") == 0)
        pCBZShare->plugins.sScrollBarIndex = count;
    else if (strcmp(szClass, "CONTAINER") == 0)
        pCBZShare->plugins.sContainerIndex = count;
    else if (strcmp(szClass, "SYSTEM") == 0)
        pCBZShare->plugins.sSystemIndex = count;


    if (!runParser(szPluginBase, szClass, &List, szError))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "ApplyBlock", szError, 0UL);
        return count;
    }
    
    pCBZShare->plugins.cbCount = count + List.cbPlugins;
    strcpy(pCBZShare->szThemeFilename, themeFile);

    if (List.cbPlugins > 0)
    {
        if (strcmp(szClass, "TITLEBAR") == 0)
            pCBZShare->plugins.bTitlebarEnabled = TRUE;
        else if (strcmp(szClass, "FRAME") == 0)
            pCBZShare->plugins.bFrameBackgroundEnabled = TRUE;
        else if (strcmp(szClass, "FRAMEBRDR") == 0)
            pCBZShare->plugins.bFrameBorderEnabled = TRUE;
        else if (strcmp(szClass, "PUSHBUTTON") == 0)
            pCBZShare->plugins.bPushButtonEnabled = TRUE;
        else if (strcmp(szClass, "RADIOBUTTON") == 0)
            pCBZShare->plugins.bRadioButtonEnabled = TRUE;
        else if (strcmp(szClass, "CHECKBOX") == 0)
            pCBZShare->plugins.bCheckBoxEnabled = TRUE;
        else if (strcmp(szClass, "MINMAX") == 0)
            pCBZShare->plugins.bMinMaxEnabled = TRUE;
        else if (strcmp(szClass, "MENU") == 0)
            pCBZShare->plugins.bMenuEnabled = TRUE;
        else if (strcmp(szClass, "NOTEBOOK") == 0)
            pCBZShare->plugins.bNoteBookBackgroundEnabled = TRUE;
        else if (strcmp(szClass, "SCROLLBAR") == 0) 
            pCBZShare->plugins.bScrollBarEnabled = TRUE;
        else if (strcmp(szClass, "CONTAINER") == 0) 
            pCBZShare->plugins.bContainerEnabled = TRUE;
        else if (strcmp(szClass, "SYSTEM") == 0) 
            pCBZShare->plugins.bSystemEnabled = TRUE;

        
    }
    else
    {
        if (strcmp(szClass, "TITLEBAR") == 0)
            pCBZShare->plugins.bTitlebarEnabled = FALSE;
        else if (strcmp(szClass, "FRAME") == 0)
            pCBZShare->plugins.bFrameBackgroundEnabled = FALSE;
        else if (strcmp(szClass, "FRAMEBRDR") == 0)
            pCBZShare->plugins.bFrameBorderEnabled = FALSE;
        else if (strcmp(szClass, "PUSHBUTTON") == 0)
            pCBZShare->plugins.bPushButtonEnabled = FALSE;
        else if (strcmp(szClass, "RADIOBUTTON") == 0)
            pCBZShare->plugins.bRadioButtonEnabled = FALSE;
        else if (strcmp(szClass, "CHECKBOX") == 0)
            pCBZShare->plugins.bCheckBoxEnabled = FALSE;
        else if (strcmp(szClass, "MINMAX") == 0)
            pCBZShare->plugins.bMinMaxEnabled = FALSE;
        else if (strcmp(szClass, "MENU") == 0)
            pCBZShare->plugins.bMenuEnabled = FALSE;
        else if (strcmp(szClass, "NOTEBOOK") == 0)
            pCBZShare->plugins.bNoteBookBackgroundEnabled = FALSE;
        else if (strcmp(szClass, "SCROLLBAR") == 0)
            pCBZShare->plugins.bScrollBarEnabled = FALSE;
        else if (strcmp(szClass, "CONTAINER") == 0)
            pCBZShare->plugins.bContainerEnabled = FALSE;
        else if (strcmp(szClass, "SYSTEM") == 0)
            pCBZShare->plugins.bSystemEnabled = FALSE;
            
    }


    strcpy(szPluginBase, szBase);
    strcat(szPluginBase, "plugins\\");

    for (j = 0; j < List.cbPlugins; j++)
    {
        //copy plugin names to shared memory.
        strcpy(pCBZShare->plugins.pluginDLL[count + j],
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
        
        if (DosLoadModule(szError, sizeof(szError), szFile, &hMod) == NO_ERROR)
        {
            PINITFN pInitFn;
            //call CBZInit to make sure the Plugin's shared memory is allocated.
            if (DosQueryProcAddr(hMod, 0, "CBZInit", (PFN *) & pInitFn) == NO_ERROR)
            {
                PVOID pData;
                //check return code from this to make sure it was sucessful.. if not disable the plugin
                pInitFn(NULLHANDLE, "", szClass, &pData);
//Need to call CBZDestroy to free the memory allocated by CBZInit
            }
            else
            {
                PSUTErrorFunc(NULLHANDLE, "Error", "ApplyBlock", "Failed Querying Procedure Address for CBZInit", 0UL);
            }


            if (DosQueryProcAddr(hMod, 0, "CBZApply", (PFN *) & pApplyFn) == NO_ERROR)
            {
                //check return code from this to make sure it was sucessful.. if not disable the plugin
                if (!pApplyFn(szClass, (List.pPluginAttributes[j].keyValue),
                         List.pPluginAttributes[j].cbAttributes,
                         List.pPluginAttributes[j].enabledState,
                         NULL)){ //set default so pass NULL
                  PSUTErrorFunc(NULLHANDLE, "Error", "ApplyBlock", "Error calling CBZApply", 0UL);
                }
                
            }
            else
            {
                PSUTErrorFunc(NULLHANDLE, "Error", "ApplyBlock", "Failed Querying Procedure Address for CBZApply", 0UL);
            }
            DosFreeModule(hMod);
        }
        else
        {
            PSUTErrorFunc(NULLHANDLE, "Error", "ApplyBlock", "Error Querying Plugin Module.. (Missing?)", 0UL);
            pCBZShare->plugins.cbCount = j - 1;
            break;
            //disable it here
        }

    }

    DosFreeMem(pCBZShare);
    i = List.cbPlugins;
    sprintf(szError, "%s: i=%d",szClass,i);
    //    PSUTErrorFunc(NULLHANDLE, "Error", "ApplyBlock",szError, 0UL);
    //delete all the attributes in the plugin/key/value list
    CBZFreePLUGIN_LIST(&List);
    
    free(szBase);
    return count + i;
}

BOOL UpdateControls(HWND hwnd, char themeFile[])
{
    PLUGIN_LIST List;
    int i;
    char szError[CCHMAXPATH];

//   WinEnableWindowUpdate(WinWindowFromID(hwnd, STID_THEMENAME),   FALSE);
    //   WinEnableWindowUpdate(WinWindowFromID(hwnd, STID_THEMEAUTHOR), FALSE);
    //   WinEnableWindowUpdate(WinWindowFromID(hwnd, STID_THEMEDATE),   FALSE);
    //   WinEnableWindowUpdate(WinWindowFromID(hwnd, STID_THEMEVERSION),FALSE);

//   WinSetWindowText(WinWindowFromID(hwnd, STID_THEMENAME), "" );
    //   WinSetWindowText(WinWindowFromID(hwnd, STID_THEMEAUTHOR), "" );
    //   WinSetWindowText(WinWindowFromID(hwnd, STID_THEMEDATE), "" );
    //   WinSetWindowText(WinWindowFromID(hwnd, STID_THEMEVERSION), "" );
    WinSetWindowText(WinWindowFromID(hwnd, STID_THEMEFILE), strrchr(themeFile, '\\') + 1);

    if (!getThemeInfo(themeFile, &List, szError))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "LoadPlugins", szError, 0UL);
        //free memory from List
        //delete all the attributes in the plugin/key/value list
        CBZFreePLUGIN_LIST(&List);

        return FALSE;
    }

    for (i = 0; i < List.pPluginAttributes[0].cbAttributes; i++)
    {
        if (strcmp(List.pPluginAttributes[0].keyValue[i].key, "Name") == 0)
        {
            WinSetWindowText(WinWindowFromID(hwnd, STID_THEMENAME), List.pPluginAttributes[0].keyValue[i].value);
        }
        else if (strcmp(List.pPluginAttributes[0].keyValue[i].key, "Author") == 0)
        {
            WinSetWindowText(WinWindowFromID(hwnd, STID_THEMEAUTHOR), List.pPluginAttributes[0].keyValue[i].value);
        }
        else if (strcmp(List.pPluginAttributes[0].keyValue[i].key, "Date") == 0)
        {
            WinSetWindowText(WinWindowFromID(hwnd, STID_THEMEDATE), List.pPluginAttributes[0].keyValue[i].value);
        }
        else if (strcmp(List.pPluginAttributes[0].keyValue[i].key, "Version") == 0)
        {
            WinSetWindowText(WinWindowFromID(hwnd, STID_THEMEVERSION), List.pPluginAttributes[0].keyValue[i].value);
        }

    }

    //delete all the attributes in the plugin/key/value list
    CBZFreePLUGIN_LIST(&List);

//   WinShowWindow(WinWindowFromID(hwnd, STID_THEMENAME),   TRUE);
    //   WinShowWindow(WinWindowFromID(hwnd, STID_THEMEAUTHOR), TRUE);
    //   WinShowWindow(WinWindowFromID(hwnd, STID_THEMEDATE),   TRUE);
    //   WinShowWindow(WinWindowFromID(hwnd, STID_THEMEVERSION),TRUE);

    return TRUE;
}

BOOL LoadPlugins(HWND hwnd, HWND hwndLBAvailable, HWND hwndLBAvailableDLL, BOOL bActive)
{
    long i, index;
    char szError[CCHMAXPATH];
    HDIR hdirFindHandle = HDIR_CREATE;
    FILEFINDBUF3 FindBuffer = {0};
    ULONG ulResultBufLen = sizeof(FILEFINDBUF3);
    ULONG ulFindCount = 1;
    APIRET rc = NO_ERROR;
    char szBase[CCHMAXPATH], szFile[CCHMAXPATH];
    PLUGIN_LIST List;

    //Query the BasePath to the CandyBarZ Plugins
    PrfQueryProfileString(HINI_USERPROFILE,
                          "CandyBarZ",
                          "BasePath",
                          NULL,
                          szBase,
                          CCHMAXPATH);

    strcpy(szFile, szBase);
    strcat(szFile, "*.cbz");    //Wildcard for searching available plugins

//     strcat(szBase, "plugins");          //used for completing full path to plugins

    // Querying all the Plugin DLL's in the %candybarz%\plugins directory.
    if ((rc = DosFindFirst(szFile,
                           &hdirFindHandle,
                           FILE_NORMAL,
                           &FindBuffer,
                           ulResultBufLen,
                           &ulFindCount,
                           FIL_STANDARD)) != NO_ERROR)
    {
        PSUTErrorFunc(hwnd, "Error", "LoadPlugins", "Failed to get Available Plugins", 0UL);
        return (FALSE);
    }
    else
    {
        _makepath(szFile, NULL, szBase, FindBuffer.achName, NULL);

        if (!getThemeInfo(szFile, &List, szError))
        {
            PSUTErrorFunc(hwnd, "Error", "LoadPlugins", szError, 0UL);
            //free memory from List
            //delete all the attributes in the plugin/key/value list
            CBZFreePLUGIN_LIST(&List);

            return FALSE;
        }

        for (i = 0; i < List.pPluginAttributes[0].cbAttributes; i++)
        {
            if (strcmp(List.pPluginAttributes[0].keyValue[i].key, "Name") == 0)
            {
                //Add the Plugin's Name to the AvailablePlugins ListBox.
                index = (LONG) WinSendMsg(hwndLBAvailable, LM_INSERTITEM,
                                     MPFROMSHORT((SHORT) LIT_SORTASCENDING),
                                     MPFROMP(List.pPluginAttributes[0].keyValue[i].value));
                WinSendMsg(hwndLBAvailableDLL, LM_INSERTITEM,
                           MPFROMSHORT((short) index),
                           MPFROMP(szFile));

            }
        }

        //keep looping until all the available plugins have be found
        while (rc != ERROR_NO_MORE_FILES)
        {
            ulFindCount = 1;

            //Get the next Plugin DLL
            rc = DosFindNext(hdirFindHandle, &FindBuffer, ulResultBufLen, &ulFindCount);
            if ((rc != ERROR_NO_MORE_FILES))
            {
                _makepath(szFile, NULL, szBase, FindBuffer.achName, NULL);

                if (!getThemeInfo(szFile, &List, szError))
                {
                    PSUTErrorFunc(hwnd, "Error", "LoadPlugins", szError, 0UL);
                    //free memory from List
                    //delete all the attributes in the plugin/key/value list
                    CBZFreePLUGIN_LIST(&List);

                    return FALSE;
                }

                for (i = 0; i < List.pPluginAttributes[0].cbAttributes; i++)
                {
                    if (strcmp(List.pPluginAttributes[0].keyValue[i].key, "Name") == 0)
                    {
                        //Add the Plugin's Name to the AvailablePlugins ListBox.
                        index = (LONG) WinSendMsg(hwndLBAvailable, LM_INSERTITEM,
                                     MPFROMSHORT((SHORT) LIT_SORTASCENDING),
                                                  MPFROMP(List.pPluginAttributes[0].keyValue[i].value));

                        WinSendMsg(hwndLBAvailableDLL, LM_INSERTITEM,
                                   MPFROMSHORT((short) index),
                                   MPFROMP(szFile));
                    }
                }
            }                   //end NO_MORE_FILES if

        }                       //end while

        DosFindClose(hdirFindHandle);
    }                           //end else

    //plugins loaded sucessfully

    //Free memory allocated in List
    //delete all the attributes in the plugin/key/value list
    CBZFreePLUGIN_LIST(&List);

    return (TRUE);
}


/*************************************************************************************************************

    $Id: setuputil.c,v 1.6 2000/10/04 20:12:51 cwohlgemuth Exp $

    $Log: setuputil.c,v $
    Revision 1.6  2000/10/04 20:12:51  cwohlgemuth
    Added support for SYSTEM plugin.

    Revision 1.5  2000/09/24 21:03:58  cwohlgemuth
    Support for SYSTEM plugin added.

    Revision 1.4  2000/09/09 22:46:58  cwohlgemuth
    Support for scrollbars and containers added

    Revision 1.3  2000/03/26 06:51:22  enorman
    resyncing with my local tree.

    Revision 1.2  1999/12/29 19:38:10  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:47:57  ktk
    Import

    Revision 1.12  1998/10/17 19:32:08  enorman
    Made some changes to eliminate PSRT.DLL

    Revision 1.11  1998/09/22 12:08:25  mwagner
    Added better dialog support

    Revision 1.8  1998/09/15 10:10:51  enorman
    many changes to replace references to menu items

    Revision 1.7  1998/09/12 10:08:54  pgarner
    Added copyright notice

    Revision 1.6  1998/09/12 07:35:47  mwagner
    Added support for bitmap images

    Revision 1.3  1998/08/10 02:55:37  enorman
    Added code check and uncheck 3D text menu items

    Revision 1.2  1998/06/29 15:31:44  mwagner
    Matts S3 and PMView fixes

    Revision 1.1  1998/06/08 14:18:57  pgarner
    Initial Checkin

*************************************************************************************************************/
