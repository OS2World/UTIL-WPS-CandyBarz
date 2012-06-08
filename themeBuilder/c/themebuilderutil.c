/* $Id: themebuilderutil.c,v 1.2 2000/09/10 16:52:20 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "themeBuilder.h"

BOOL OpenThemeInfo(HWND hwnd, char szFilename[])
{
    PLUGIN_LIST List;
    char szError[128];
    int i;

    if (!getThemeInfo(szFilename, &List, szError))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "OpenThemeInfo", szError, 0UL);
        return FALSE;
    }

    //search the list of Attributes for the "Name" attribute.
    for (i = 0; i < List.pPluginAttributes[0].cbAttributes; i++)
    {
        if (strcmp(List.pPluginAttributes[0].keyValue[i].key, "Name") == 0)
        {
          WinSendMsg(WinWindowFromID(hwnd, EFID_THEMENAME), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSetWindowText(WinWindowFromID(hwnd, EFID_THEMENAME),
                             List.pPluginAttributes[0].keyValue[i].value);
            continue;
        }
        if (strcmp(List.pPluginAttributes[0].keyValue[i].key, "Author") == 0)
        {
          WinSendMsg(WinWindowFromID(hwnd, EFID_THEMEAUTHOR), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSetWindowText(WinWindowFromID(hwnd, EFID_THEMEAUTHOR),
                             List.pPluginAttributes[0].keyValue[i].value);
            continue;
        }
        if (strcmp(List.pPluginAttributes[0].keyValue[i].key, "Version") == 0)
        {
            WinSetWindowText(WinWindowFromID(hwnd, EFID_THEMEVERSION),
                             List.pPluginAttributes[0].keyValue[i].value);
            continue;
        }

    }
    CBZFreePLUGIN_LIST(&List);
    return TRUE;
}

BOOL OpenThemeClass(HWND hwnd, char szFilename[], CBZDATA * pCBZData, char szClassName[])
{
    PLUGIN_LIST List;
    char szError[128], tempText[64];
    int i, i2, index;
    HWND hwndControl;
    PAPPLYFN pApplyFn;

    if (!runParser(szFilename, szClassName, &List, szError))
    {
        PSUTErrorFunc(NULLHANDLE, "Error", "OpenThemeClass", szError, 0UL);
        return FALSE;
    }

    //Clear the current selected plugins by placing them back into the
    // AvailablePlugins ListBox
    hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
    index = (SHORT) WinSendMsg(hwndControl, LM_QUERYITEMCOUNT,
                               MPFROMLONG(0),
                               MPFROMLONG(0));
    for (i = 0; i < index; i++)
    {
        hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);

        //Save the text so it can be added to Available Plugin Listbox
        WinSendMsg(hwndControl, LM_QUERYITEMTEXT,
                   MPFROM2SHORT((SHORT) 0, 64),
                   MPFROMP((PSZ) tempText));
        //remove item from Selected Plugin ListBox
        WinSendMsg(hwndControl, LM_DELETEITEM,
                   MPFROMSHORT((SHORT) 0),
                   MPFROMLONG(0));

        //Add the item to the Available Plugins ListBox
        hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINS);
        i2 = (SHORT) WinSendMsg(hwndControl, LM_INSERTITEM,
                                MPFROMSHORT((SHORT) LIT_SORTASCENDING),
                                MPFROMP((PSZ) tempText));

        //Add the dll name to the AvailablePluginsDLL ListBox at the correct index
        strcpy(tempText, pCBZData->Plugins[0].szPluginDLL);
        hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINDLLS);
        WinSendMsg(hwndControl, LM_INSERTITEM,
                   MPFROMSHORT((SHORT) i2),
                   MPFROMP((PSZ) tempText));

        //remove the plugin from the pTBData structure
        if (!RemovePlugin(pCBZData, 0))
        {
            PSUTErrorFunc(hwnd,
                          "Error",
                          "OpenThemeClass",
                          "Error Removing Plugin!",
                          0UL);
            //Put it back in Selected Plugin ListBox?
            break;
        }

    }

    //Try selecting each plugin from the themeFile out of the AvailablePlugins ListBox
    for (i = 0; i < List.cbPlugins; i++)
    {
        hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINDLLS);
        //find the index of the plugin in the AvailablePlugins ListBox
        index = (SHORT) WinSendMsg(hwndControl, LM_SEARCHSTRING,
                                 MPFROM2SHORT(LSS_CASESENSITIVE, LIT_FIRST),
                           MPFROMP(List.pPluginAttributes[i].szPluginName));

        if (index == LIT_NONE)  //Plugin not found

        {
            PSUTErrorFunc(hwnd,
                          "Error",
                          "OpenThemeClass - Plugin Not Found",
                          List.pPluginAttributes[i].szPluginName,
                          0UL);
            break;
        }

        hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINS);
        //Get the text from the Available Plugin ListBox
        WinSendMsg(hwndControl, LM_QUERYITEMTEXT,
                   MPFROM2SHORT((SHORT) index, 64),
                   MPFROMP((PSZ) tempText));
        //remove the selected item from the Available Plugins ListBox
        WinSendMsg(hwndControl, LM_DELETEITEM,
                   MPFROMSHORT((SHORT) index),
                   MPFROMLONG(0));

        //Add the queried text to the Selected Plugin ListBox
        hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
        i2 = (SHORT) WinSendMsg(hwndControl, LM_INSERTITEM,
                                MPFROMSHORT((SHORT) LIT_END),
                                MPFROMP((PSZ) tempText));

        //Get the selected plugin DLL name from the AvailablePluginDLLS listbox
        hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINDLLS);
        WinSendMsg(hwndControl, LM_QUERYITEMTEXT,
                   MPFROM2SHORT((SHORT) index, 64),
                   MPFROMP((PSZ) tempText));
        //remove it from the AvailablePluginDLLS listbox
        WinSendMsg(hwndControl, LM_DELETEITEM,
                   MPFROMSHORT((SHORT) index),
                   MPFROMLONG(0));

        if (!AddPlugin(hwnd, pCBZData, List.pPluginAttributes[i].szPluginName, ""))
        {
            //Error in AddPlugin... Put the plugin back into AvailableListbox?
            DosBeep(500, 50);
        }

        if (DosQueryProcAddr(pCBZData->Plugins[pCBZData->cbPlugins - 1].hModDll,
                             0, "CBZApply", (PFN *) & pApplyFn) == NO_ERROR)
        {
            //check return code from this to make sure it was sucessful.. if not disable the plugin
            pApplyFn(szClassName, (List.pPluginAttributes[i].keyValue),
                     List.pPluginAttributes[i].cbAttributes,
                     List.pPluginAttributes[i].enabledState,
                     pCBZData->Plugins[i].pData);
        }
        else
        {
            PSUTErrorFunc(NULLHANDLE, "Error", "ApplyPreviewBlock", "Failed Querying Procedure Address for CBZApply", 0UL);
        }
    }

    CBZFreePLUGIN_LIST(&List);
    return TRUE;
}

// AddPlugin(hwnd, pTBData, dllName, appName, bActive)
//    hwnd     --> Handle of the calling window
//    pTBData  --> Pointer to the titlebar's CandyBarZ data structure
//    dllName  --> DLL name of the plugin to add to the pTBData structure
//    appName  --> Name of the app to add this plugin to. NULL = default(sharedMem)
//    bActive  --> Whether the plugin should be added to the active or inactive section
BOOL AddPlugin(HWND hwnd, CBZDATA * pCBZData, char dllName[], char appName[])
{
    int index;
    char szError[32];
    char szFile[CCHMAXPATH];
    char szBase[CCHMAXPATH];
    PINITFN pInitFn;

    //determine the index for the added plugin in the plugin array
    index = pCBZData->cbPlugins;

    //Set some initial data for the plugin.
    pCBZData->Plugins[index].bActiveEnabled = TRUE;
    pCBZData->Plugins[index].bInactiveEnabled = TRUE;
    pCBZData->Plugins[index].pData = NULL;
    pCBZData->Plugins[index].cbData = 0;
    strcpy(pCBZData->Plugins[index].szPluginDLL, dllName);

    //plugins are not necessarily in the LIBPATH so
    //    find the full path to the plugin dll.
    PrfQueryProfileString(HINI_USERPROFILE,
                          "CandyBarZ",
                          "BasePath",
                          NULL,
                          szBase,
                          CCHMAXPATH);
    strcat(szBase, "plugins");
    _makepath(szFile, NULL, szBase, pCBZData->Plugins[index].szPluginDLL, "DLL");

    //Load the Plugin Module
    if (DosLoadModule(szError, sizeof(szError), szFile,
                      &(pCBZData->Plugins[index].hModDll)) != NO_ERROR)
    {
        //Error loading the dll...disable this plugin!
        PSUTErrorFunc(hwnd,
                      "Error",
                      "AddPlugin",
                      "Error Loading the Plugin Module",
                      0UL);
        return (FALSE);
    }

    //Query the Plugin's Window Procedure.
    if (DosQueryProcAddr(pCBZData->Plugins[index].hModDll, 0, "CBZPluginWndProc",
            (PFN *) & (pCBZData->Plugins[index].pPluginWndProc)) != NO_ERROR)
    {
        //error getting the Plugin's WndProc...disable the plugin!
        PSUTErrorFunc(hwnd,
                      "Error",
                      "AddPlugin",
                      "Error Quering CBZPluginWndProc!",
                      0UL);
        //clean up
        DosFreeModule(pCBZData->Plugins[index].hModDll);
        return (FALSE);
    }

    //Query The Plugin's CBZInit Function
    if (DosQueryProcAddr(pCBZData->Plugins[index].hModDll, 0, "CBZInit",
                         (PFN *) & (pInitFn)) != NO_ERROR)
    {
        PSUTErrorFunc(hwnd,
                      "Error",
                      "AddPlugin",
                      "Error Quering CBZInit!",
                      0UL);
        //error in Plugin's initialization...disable the plugin!
        pCBZData->Plugins[index].bActiveEnabled = FALSE;
        pCBZData->Plugins[index].bInactiveEnabled = FALSE;
        DosFreeModule(pCBZData->Plugins[index].hModDll);
        return (FALSE);
    }
    //Call the plugins Init function.
    if (!pInitFn(NULLHANDLE, "", "TITLEBAR", (PVOID *) & (pCBZData->Plugins[index].pData)))
    {
        PSUTErrorFunc(hwnd,
                      "Error",
                      "AddPlugin",
                      "Error running the plugin's CBZInit function!",
                      0UL);
        //error in Plugin's initialization...disable the plugin!
        pCBZData->Plugins[index].bActiveEnabled = FALSE;
        pCBZData->Plugins[index].bInactiveEnabled = FALSE;
        DosFreeModule(pCBZData->Plugins[index].hModDll);
        return (FALSE);
    }

    //Query the address of the plugin's Render function.
    if (DosQueryProcAddr(pCBZData->Plugins[index].hModDll, 0, "CBZPluginRender",
           (PFN *) & (pCBZData->Plugins[index].pfnPluginRender)) != NO_ERROR)
    {
        //Error getting CBZPluginRender function!
        PSUTErrorFunc(hwnd,
                      "Error",
                      "AddPlugin",
                      "Error Quering CBZPluginRender!",
                      0UL);

        DosFreeModule(pCBZData->Plugins[index].hModDll);
        DosFreeMem(pCBZData->Plugins[index].pData);
        return (FALSE);
    }

    //update the plugin count.
    pCBZData->cbPlugins++;

    return (TRUE);              //plugin add sucessfull!

}

// RemovePlugin(hwnd, pTBData, dllName, appName, bActive)
//    hwnd     --> Handle of the calling window
//    pTBData  --> Pointer to the titlebar's CandyBarZ data structure
//    index    --> index of the plugin to be removed
BOOL RemovePlugin( /*HWND hwnd, */ CBZDATA *pCBZData, int index)
{
    //remove the plugin from the given index, and move all plugins below it up 1
    int i, count;

    if ((index >= pCBZData->cbPlugins) || (index < 0))
        return (FALSE);
    count = pCBZData->cbPlugins;

    //free resources of removed plugin
    if (pCBZData->Plugins[index].hModDll != NULLHANDLE)
        DosFreeModule(pCBZData->Plugins[index].hModDll);
//should call the CBZDestroy Function here instead!
    if (pCBZData->Plugins[index].pData != NULL)
        DosFreeMem(pCBZData->Plugins[index].pData);

    //move the lower plugins up 1
    for (i = index; i < count; i++)
    {
        SwapPlugins(pCBZData, i, i + 1);
    }

    //update the plugin count
    pCBZData->cbPlugins--;

    return (TRUE);
}

// SwapPlugin(pTBData, index1, index2, bActive)
//    pTBData  --> Pointer to the titlebar's CandyBarZ data structure
//    index1   --> swap this with index2
//    index2   --> swap this with index1
void SwapPlugins(CBZDATA *pCBZData, int index1, int index2)
{
    PSPLUGIN tempPlugin;

    memcpy(&tempPlugin, &(pCBZData->Plugins[index1]), sizeof(PSPLUGIN));
    memcpy(&(pCBZData->Plugins[index1]), &(pCBZData->Plugins[index2]), sizeof(PSPLUGIN));
    memcpy(&(pCBZData->Plugins[index2]), &tempPlugin, sizeof(PSPLUGIN));
}

// LoadPlugins(hwnd, pTBData, hwndLBAvailable, hwndLBAvailableDLL, bActive)
//    hwnd     --> Handle of the calling window
//    pTBData  --> Pointer to the titlebar's CandyBarZ data structure
//    hwndLBAvailable      --> Handle of the ListBox containing the Available Plugin Names
//    hwndLBAvailableDLL   --> Handle of the ListBox containing the Available Plugin DLL Names
BOOL LoadPlugins(HWND hwnd, CBZDATA * pCBZData, HWND hwndLBAvailable, HWND hwndLBAvailableDLL, long Win_Type)
{
    HMODULE hModPlug;

    int index;
    char dllName[9];

    char szError[32];
    PLUGINPFN pPluginInfoFunction;
    PLUGIN_INFO pluginInfo;

    HDIR hdirFindHandle = HDIR_CREATE;
    FILEFINDBUF3 FindBuffer = {0};
    ULONG ulResultBufLen = sizeof(FILEFINDBUF3);
    ULONG ulFindCount = 1;
    APIRET rc = NO_ERROR;

    char szBase[CCHMAXPATH], szFile[CCHMAXPATH];

    //Query the BasePath to the CandyBarZ Plugins
    PrfQueryProfileString(HINI_USERPROFILE,
                          "CandyBarZ",
                          "BasePath",
                          NULL,
                          szBase,
                          CCHMAXPATH);

    strcpy(szFile, szBase);
    strcat(szFile, "plugins\\*.dll");  //Wildcard for searching available plugins

    strcat(szBase, "plugins");  //used for completing full path to plugins

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
        //construct the full path the to the plugin
        _splitpath(FindBuffer.achName, NULL, NULL, dllName, NULL);
        strupr(dllName);
        _makepath(szFile, NULL, szBase, dllName, "DLL");

        if (DosLoadModule(szError, sizeof(szError), szFile, &hModPlug) == NO_ERROR)
        {
            //Load the CBZPluginInfo function so we can Query the plugin's Information
            if (DosQueryProcAddr(hModPlug, 0, "CBZPluginInfo", (PFN *) & (pPluginInfoFunction)) == NO_ERROR)
            {
                //Query the PLUGIN_INFO struct from the plugin referenced by hModPlug
                pPluginInfoFunction(&pluginInfo);
                if (pluginInfo.lWindowTypes & Win_Type)
                {
                    //Add the Plugin's Name to the AvailablePlugins ListBox.
                    index = (SHORT) WinSendMsg(hwndLBAvailable, LM_INSERTITEM,
                                     MPFROMSHORT((SHORT) LIT_SORTASCENDING),
                                            MPFROMP(pluginInfo.PluginName));

                    //Add the Plugin's DLL Name to the offscreen AvailablePluginDLLs ListBox.
                    // keeping it synchronized with the AvailablePlugins ListBox
                    WinSendMsg(hwndLBAvailableDLL, LM_INSERTITEM,
                               MPFROMSHORT((SHORT) index),
                               MPFROMP(dllName));
                }
            }
            DosFreeModule(hModPlug);
        }

        //keep looping until all the available plugins have be found
        while (rc != ERROR_NO_MORE_FILES)
        {
            ulFindCount = 1;

            //Get the next Plugin DLL
            rc = DosFindNext(hdirFindHandle, &FindBuffer, ulResultBufLen, &ulFindCount);
            if ((rc != ERROR_NO_MORE_FILES))
            {
                //construct the full path the to the plugin
                _splitpath(FindBuffer.achName, NULL, NULL, dllName, NULL);
                strupr(dllName);
                _makepath(szFile, NULL, szBase, dllName, "DLL");

                if (DosLoadModule(szError, sizeof(szError), szFile, &hModPlug) == NO_ERROR)
                {
                    //Load the CBZPluginInfo function so we can Query the plugin's Information
                    if (DosQueryProcAddr(hModPlug, 0, "CBZPluginInfo", (PFN *) & (pPluginInfoFunction)) == NO_ERROR)
                    {
                        pPluginInfoFunction(&pluginInfo);

                        if (pluginInfo.lWindowTypes & Win_Type)
                        {
                            //Add the Plugin's Name to the AvailablePlugins ListBox.
                            index = (SHORT) WinSendMsg(hwndLBAvailable, LM_INSERTITEM,
                                     MPFROMSHORT((SHORT) LIT_SORTASCENDING),
                                            MPFROMP(pluginInfo.PluginName));

                            //Add the Plugin's DLL Name to the offscreen AvailablePluginDLLs ListBox.
                            // keeping it synchronized with the AvailablePlugins ListBox
                            WinSendMsg(hwndLBAvailableDLL, LM_INSERTITEM,
                                       MPFROMSHORT((SHORT) index),
                                       MPFROMP(dllName));
                        }
                    }
                    DosFreeModule(hModPlug);
                }               //end DosLoadModule if

            }                   //end NO_MORE_FILES if

        }                       //end while

        DosFindClose(hdirFindHandle);
    }                           //end else

    //plugins loaded sucessfully
    return (TRUE);
}

// InitPlugins(hwnd, pTBData, appName, hwndLBSelected,
//                   hwndLBAvailable, hwndLBAvailableDLL )
//    hwnd     --> Handle of the calling window
//    pTBData  --> Pointer to the titlebar's CandyBarZ data structure
//    hwndLBSelected       --> Handle of the ListBox containing the Selected Plugin Names
//    hwndLBAvailable      --> Handle of the ListBox containing the Available Plugin Names
//    hwndLBAvailableDLL   --> Handle of the ListBox containing the Available Plugin DLL Names
//    bActive  --> which array to load into. (active or inactive)
BOOL InitPlugins(HWND hwnd, CBZDATA *pCBZData, char appName[], HWND hwndLBSelected,
                 HWND hwndLBAvailable, HWND hwndLBAvailableDLL)
{
    int i, index, count;
    char szBase[CCHMAXPATH], szFile[CCHMAXPATH];
    char szError[32];
    PLUGINPFN pPluginInfoFunction;
    PLUGIN_INFO pluginInfo;
    PINITFN pInitFn;

    //Query the path to the Plugin directory
    PrfQueryProfileString(HINI_USERPROFILE,
                          "CandyBarZ",
                          "BasePath",
                          NULL,
                          szBase,
                          CCHMAXPATH);
    strcat(szBase, "plugins");

    count = pCBZData->cbPlugins;

    //initialize each plugin
    for (i = 0; i < count; i++)
    {
        //Initialize to some initial values...
        pCBZData->Plugins[i].bActiveEnabled = TRUE;
        pCBZData->Plugins[i].bInactiveEnabled = TRUE;
        pCBZData->Plugins[i].pData = NULL;
        pCBZData->Plugins[i].cbData = 0;

        //Load the Plugin Module
        _makepath(szFile, NULL, szBase, pCBZData->Plugins[i].szPluginDLL, "DLL");
        if (DosLoadModule(szError, sizeof(szError), szFile,
                          &(pCBZData->Plugins[i].hModDll)) != NO_ERROR)
        {
            //Error loading the dll...disable this plugin!
            pCBZData->Plugins[i].bActiveEnabled = FALSE;
            pCBZData->Plugins[i].bInactiveEnabled = FALSE;
            continue;           //end this iteration...try next plugin.

        }

        //Query the Plugin's Window Procedure.
        if (DosQueryProcAddr(pCBZData->Plugins[i].hModDll, 0, "CBZPluginWndProc",
                (PFN *) & (pCBZData->Plugins[i].pPluginWndProc)) != NO_ERROR)
        {
            //error getting the Plugin's WndProc...disable the plugin!
            pCBZData->Plugins[i].bActiveEnabled = FALSE;
            pCBZData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pCBZData->Plugins[i].hModDll);
            continue;           //end this iteration...try next plugin.

        }

        //Query The Plugin's CBZInit Function
        if (DosQueryProcAddr(pCBZData->Plugins[i].hModDll, 0, "CBZInit",
                             (PFN *) & (pInitFn)) != NO_ERROR)
        {
            PSUTErrorFunc(hwnd,
                          "Error",
                          "AddPlugin",
                          "Error Quering CBZInit!",
                          0UL);
            //error in Plugin's initialization...disable the plugin!
            pCBZData->Plugins[i].bActiveEnabled = FALSE;
            pCBZData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pCBZData->Plugins[i].hModDll);
            return (FALSE);
        }
        //Call the plugins Init function.
        if (!pInitFn(NULLHANDLE, "", "TITLEBAR", (PVOID *) & (pCBZData->Plugins[i].pData)))
        {
            PSUTErrorFunc(hwnd,
                          "Error",
                          "AddPlugin",
                          "Error running the plugin's CBZInit function!",
                          0UL);
            //error in Plugin's initialization...disable the plugin!
            pCBZData->Plugins[i].bActiveEnabled = FALSE;
            pCBZData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pCBZData->Plugins[i].hModDll);
            return (FALSE);
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

            continue;           //end this iteration...try next plugin.

        }

        if (DosQueryProcAddr(pCBZData->Plugins[i].hModDll, 0, "CBZPluginInfo",
                             (PFN *) & (pPluginInfoFunction)) != NO_ERROR)
        {
            //Error getting CBZPluginInfo function!
            pCBZData->Plugins[i].bActiveEnabled = FALSE;
            pCBZData->Plugins[i].bInactiveEnabled = FALSE;
            DosFreeModule(pCBZData->Plugins[i].hModDll);
            DosFreeMem(pCBZData->Plugins[i].pData);
            continue;           //end this iteration...try next plugin

        }

        //query the plugin's info structure
        pPluginInfoFunction(&pluginInfo);

        //Add this plugin to the SelectedPlugins ListBox
        WinSendMsg(hwndLBSelected, LM_INSERTITEM,
                   MPFROMSHORT((SHORT) LIT_END),
                   MPFROMP(pluginInfo.PluginName));

        //Determine the index of the corresponding entry in the AvailablePluginDLLS ListBox
        index = (SHORT) WinSendMsg(hwndLBAvailableDLL, LM_SEARCHSTRING,
                                 MPFROM2SHORT(LSS_CASESENSITIVE, LIT_FIRST),
                                   MPFROMP(pCBZData->Plugins[i].szPluginDLL));
        if (index != LIT_ERROR && index != LIT_NONE)
        {
            //Remove the entry from the AvailablePlugin... ListBoxes so we can only select
            // each plugin once.
            WinSendMsg(hwndLBAvailableDLL, LM_DELETEITEM,
                       MPFROMSHORT((SHORT) index),
                       MPFROMLONG(0));
            WinSendMsg(hwndLBAvailable, LM_DELETEITEM,
                       MPFROMSHORT((SHORT) index),
                       MPFROMLONG(0));
        }
    }                           //end for(..) loop

    return (TRUE);              //plugins initialized sucessfully!

}

BOOL AddNotebookPage(HWND hwndNoteBook, long pageID, PFNWP pfnDlgProc,
                     char szTabText[], char szMinorText[],
                     char szStatusText[], BOOL bMajor, char szClassName[])
{
    ULONG ulPageID;
    HWND hwndDialog;
    BOOKPAGEINFO bpInfo;

    if (bMajor)
    {
        // insert page
        if ((ulPageID = (ULONG) WinSendMsg(hwndNoteBook,
                                           BKM_INSERTPAGE,
                                           (MPARAM) NULL,
                                           MPFROM2SHORT(BKA_AUTOPAGESIZE |
                                                        BKA_STATUSTEXTON |
                                                        BKA_MAJOR | BKA_MINOR,
                                                        BKA_LAST))) == 0L)
        {
             PSUTErrorFunc(hwndNoteBook, "Error", "AddNotebookPage",
                "Error Inserting the Major Notebook Page", 0UL);
            return FALSE;
        }
    }
    else
    {
        // insert page
        if ((ulPageID = (ULONG) WinSendMsg(hwndNoteBook,
                                           BKM_INSERTPAGE,
                                           (MPARAM) NULL,
                                           MPFROM2SHORT(BKA_AUTOPAGESIZE |
                                                        BKA_STATUSTEXTON |
                                                        BKA_MINOR,
                                                        BKA_LAST))) == 0L)
        {
             PSUTErrorFunc(hwndNoteBook, "Error", "AddNotebookPage",
                "Error Inserting the Minor Notebook Page", 0UL);
            return FALSE;
        }
    }

    if ((hwndDialog = WinLoadDlg(hwndNoteBook,
                                 hwndNoteBook,
                                 pfnDlgProc,
                                 NULLHANDLE,
                                 pageID,
                                 MPFROMP(szClassName))) == NULLHANDLE)
    {
        PSUTErrorFunc(hwndNoteBook, "Error", "AddNotebookPage",
           "Error Loading the Notebook Dialog", 0UL);

        return FALSE;
    }

    // set page info
    WinSendMsg(hwndNoteBook,
               BKM_SETPAGEDATA,
               MPFROMLONG(ulPageID),
               MPFROMSHORT(pageID));
    WinSendMsg(hwndNoteBook,
               BKM_SETTABTEXT,
               MPFROMLONG(ulPageID),
               MPFROMP(szTabText));
    WinSendMsg(hwndNoteBook,
               BKM_SETPAGEWINDOWHWND,
               MPFROMLONG(ulPageID),
               MPFROMHWND(hwndDialog));
    WinSendMsg(hwndNoteBook,
               BKM_SETSTATUSLINETEXT,
               MPFROMLONG(ulPageID),
               MPFROMP(szStatusText));

    // minor tab info
    memset(&bpInfo, 0, sizeof(bpInfo));
    bpInfo.cb = sizeof(bpInfo);
    bpInfo.fl = BFA_MINORTABTEXT;
    bpInfo.cbMinorTab = strlen(szMinorText);
    bpInfo.pszMinorTab = szMinorText;
    //Set the Minor text info, so popup menu has the correct text
    WinSendMsg(hwndNoteBook,
               BKM_SETPAGEINFO,
               MPFROMLONG(ulPageID),
               MPFROMP(&bpInfo));

    return TRUE;
}

