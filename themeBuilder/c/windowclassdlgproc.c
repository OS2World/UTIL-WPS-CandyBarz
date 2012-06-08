/* $Id: windowclassdlgproc.c,v 1.5 2000/09/24 20:39:13 cwohlgemuth Exp $ */

#include "themeBuilder.h"

//Generic Window Procedure for the Notebook pages for each window class
MRESULT EXPENTRY WindowClassDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HWND hwndControl;
    HWND hwndFocus;
    ULONG ulRc;
    short sControlId;
    short sHelpId;
    int i, index;
    char tempText[64];
    HMODULE hModPlug;
    HWND hwndDialog;
    PLUGINPFN pPluginInfoFunction;
    PFNWP pPluginDlgProc;
    PLUGIN_INFO pluginInfo;
    char szError[32];
    DLGDATA *pDlgData;
    CBZDATA *pCBZData;
    PVOID pDataBackup;
    char pszFile[CCHMAXPATH], pszBase[CCHMAXPATH];
    PSAVEFN pSaveFn;

    switch (msg)
    {
        case CBZ_NOPREVIEW:
        {
            CBZSHARE *pCBZShare = NULL;
            HWND hwndPreview = HWNDFROMMP(mp1);

            if ((pDlgData = (DLGDATA *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
            {
                PSUTErrorFunc(hwnd,
                              "Error",
                              "WindowClassDlgProc",
                              "Failed to query window words",
                              0UL);
                break;
            }

            // get shared data
            if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ)
                    != NO_ERROR)
            {
                return (FALSE);
            }

//need to destroy resources here

            WinSetWindowPtr(hwndPreview, pCBZShare->ulDataOffset, NULL);
            DosFreeMem(pCBZShare);
        }
        break;



        case CBZ_PREVIEW:
        {
            CBZSHARE *pCBZShare = NULL;
            HWND hwndPreview = HWNDFROMMP(mp1);

            if ((pDlgData = (DLGDATA *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
            {
                PSUTErrorFunc(hwnd,
                              "Error",
                              "WindowClassDlgProc",
                              "Failed to query window words",
                              0UL);
                break;
            }

            pCBZData = pDlgData->pCBZData;

            // get shared data
            if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ)
                    != NO_ERROR)
            {
                return (FALSE);
            }

            WinSetWindowPtr(hwndPreview, pCBZShare->ulDataOffset, pCBZData);
            DosFreeMem(pCBZShare);
        }
        break;

        //message passed in to save the current settings to a theme file.
        case CBZ_SAVE:
        {
            FILE *fp;
            char tmpText[4096];
            int i;

            //get fp from mp1
            fp = (FILE *) PVOIDFROMMP(mp1);
            if (fp == NULL)
                return FALSE;

            if ((pDlgData = (DLGDATA *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
            {
                PSUTErrorFunc(hwnd,
                              "Error",
                              "WindowClassDlgProc",
                              "Failed to query window words",
                              0UL);
                break;
            }
            //write the class name to the theme file
            fprintf(fp, "[%s]\n", pDlgData->szClassName);

            pCBZData = pDlgData->pCBZData;
            //for each of the selected plugins, call their CBZWriteAttributes function
            // and then write the resulting string to the theme File.
            for (i = 0; i < pCBZData->cbPlugins; i++)
            {
                strcpy(tmpText, "");

                //Query The Plugin's CBZWriteAttributes Function
                if (DosQueryProcAddr(pCBZData->Plugins[i].hModDll, 0, "CBZWriteAttributes",
                                     (PFN *) & (pSaveFn)) != NO_ERROR)
                {
                    PSUTErrorFunc(hwnd,
                                  "Error",
                                  "WindowClassDlgProc",
                                  "Error Quering CBZWriteAttributes!",
                                  0UL);
                    return (FALSE);
                }


                //Call the plugins CBZWriteAttributes function.
                if (!pSaveFn(tmpText, (PVOID) pCBZData->Plugins[i].pData))
                {
                    PSUTErrorFunc(hwnd,
                                  "Error",
                                  "WindowClassDlgProc",
                                  "Error in the plugin's CBZWriteAttributes function!",
                                  0UL);
                    continue;
                }
                fprintf(fp, "  PluginName = %s\n", pCBZData->Plugins[i].szPluginDLL);
                fprintf(fp, "  {\n");
                fprintf(fp, "%s", tmpText);
                fprintf(fp, "  }\n\n");
            }

        }
        break;

        case WM_INITDLG:
        {
            // alloc window words
            if (DosAllocMem((PPVOID) & pDlgData,
                            sizeof(DLGDATA),
                            PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
            {
                WinSetWindowPtr(hwnd, QWL_USER, NULL);
                PSUTErrorFunc(hwnd, "Error", "WindowClassDlgProc", "Unable to Allocate Memory for Window Words", 0UL);
                WinPostMsg(NULLHANDLE, WM_QUIT, (MPARAM) 0, (MPARAM) 0);
                return ((MRESULT) FALSE);
            }
            memset(pDlgData, 0, sizeof(DLGDATA));

            //store the ClassName for this Notebook page
            strcpy(pDlgData->szClassName, (char *) PVOIDFROMMP(mp2));
            //set the lWindowType flags for this page.
            if (strcmp(pDlgData->szClassName, "SYSTEM") == 0)
                pDlgData->lWindowType = CBZ_SYSTEM;
            else if (strcmp(pDlgData->szClassName, "TITLEBAR") == 0)
                pDlgData->lWindowType = CBZ_TITLEBAR;
            else if (strcmp(pDlgData->szClassName, "FRAME") == 0)
                pDlgData->lWindowType = CBZ_FRAME;
            else if (strcmp(pDlgData->szClassName, "FRAMEBRDR") == 0)
                pDlgData->lWindowType = CBZ_FRAMEBRDR;
            else if (strcmp(pDlgData->szClassName, "PUSHBUTTON") == 0)
                pDlgData->lWindowType = CBZ_PUSHBUTTON;
            else if (strcmp(pDlgData->szClassName, "RADIOBUTTON") == 0)
                pDlgData->lWindowType = CBZ_RADIOBUTTON;
            else if (strcmp(pDlgData->szClassName, "CHECKBOX") == 0)
                pDlgData->lWindowType = CBZ_CHECKBOX;
            else if (strcmp(pDlgData->szClassName, "MINMAX") == 0)
                pDlgData->lWindowType = CBZ_MINMAX;
            else if (strcmp(pDlgData->szClassName, "MENU") == 0)
                pDlgData->lWindowType = CBZ_MENU;
            else if (strcmp(pDlgData->szClassName, "NOTEBOOK") == 0)
                pDlgData->lWindowType = CBZ_NOTEBOOK;
            else if (strcmp(pDlgData->szClassName, "SCROLLBAR") == 0)
                pDlgData->lWindowType = CBZ_SCROLLBAR;
            else if (strcmp(pDlgData->szClassName, "CONTAINER") == 0)
                pDlgData->lWindowType = CBZ_CONTAINER;
            else //add other window classes here
                pDlgData->lWindowType = 0;

            // alloc more window words for the plugin settings
            if (DosAllocMem((PPVOID) & (pDlgData->pCBZData),
                            sizeof(CBZDATA),
                            PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
            {
                PSUTErrorFunc(hwnd, "Error", "WindowClassDlgProc",
                              "Unable to Allocate Memory for Plugin Window Words", 0UL);
                WinPostMsg(NULLHANDLE, WM_QUIT, (MPARAM) 0, (MPARAM) 0);
                return ((MRESULT) FALSE);
            }
            memset(pDlgData->pCBZData, 0, sizeof(CBZDATA));

            WinSetWindowPtr(hwnd, QWL_USER, pDlgData);  // set window words

            //Setup the ListBoxes with the available plugins
            if (!LoadPlugins(hwnd, pDlgData->pCBZData, WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINS),
                             WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINDLLS), pDlgData->lWindowType))
            {
                PSUTErrorFunc(hwnd, "Error", "WindowClassDlgProc", "Error getting Available Plugins", 0UL);
                WinDismissDlg(hwnd, DID_ERROR);
                return (MRFROMLONG(FALSE));
            }

            //Initialize the plugins.
            if (!InitPlugins(hwnd, pDlgData->pCBZData, NULL, WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS),
                             WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINS),
                           WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINDLLS)))
            {
                PSUTErrorFunc(hwnd, "Error", "WindowClassDlgProc", "Error initializing plugins", 0UL);
                WinDismissDlg(hwnd, DID_ERROR);
                return (MRFROMLONG(FALSE));
            }
        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
                //plugin from Available Plugins --> Selected Plugins
                case PBID_SELECTPLUGIN:
                {
                    if ((pDlgData = (DLGDATA *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "Failed to query window words",
                                      0UL);
                        break;
                    }
                    pCBZData = pDlgData->pCBZData;

                    //find out which item is selected.
                    hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINS);
                    index = (SHORT) WinSendMsg(hwndControl, LM_QUERYSELECTION,
                                             MPFROMSHORT((SHORT) LIT_FIRST),
                                               MPFROMLONG(0));

                    if (index == LIT_NONE)  //make sure something is selected!

                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "No Plugin Selected",
                                      0UL);
                        break;
                    }

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
                    i = (SHORT) WinSendMsg(hwndControl, LM_INSERTITEM,
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

                    //try adding this plugin
                    if (!AddPlugin(hwnd, pCBZData, tempText, NULL))
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "Error Adding the Plugin!",
                                      0UL);
                        //remove the entry from the Selected Plugin ListBox
                        hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
                        WinSendMsg(hwndControl, LM_DELETEITEM,
                                   MPFROMSHORT((SHORT) i),
                                   MPFROMLONG(0));
                        break;
                    }

                    hwndControl = WinWindowFromID(hwnd, STID_PLUGINNAME);
                    WinSetWindowText(hwndControl, "<Plugin Name>");
                    hwndControl = WinWindowFromID(hwnd, STID_PLUGINAUTHOR);
                    WinSetWindowText(hwndControl, "<Plugin Author>");

                }
                break;

                //rearrange plugins in Selected Plugins ListBox
                case PBID_PLUGINUP:
                {
                    if ((pDlgData = (DLGDATA *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "Failed to query window words",
                                      0UL);
                        break;
                    }
                    pCBZData = pDlgData->pCBZData;

                    //Get the index of the selected item
                    hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
                    index = (SHORT) WinSendMsg(hwndControl, LM_QUERYSELECTION,
                                             MPFROMSHORT((SHORT) LIT_FIRST),
                                               MPFROMLONG(0));

                    //make sure something is selected
                    if (index == LIT_NONE)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "No Plugin Selected",
                                      0UL);
                        break;
                    }

                    //don't move up if already at the top!
                    if (index > 0)
                    {
                        //Save the text of the selected item
                        WinSendMsg(hwndControl, LM_QUERYITEMTEXT,
                                   MPFROM2SHORT((SHORT) index, 64),
                                   MPFROMP((PSZ) tempText));
                        //remove selected item
                        WinSendMsg(hwndControl, LM_DELETEITEM,
                                   MPFROMSHORT((SHORT) index),
                                   MPFROMLONG(0));
                        //reinsert the item up 1
                        WinSendMsg(hwndControl, LM_INSERTITEM,
                                   MPFROMSHORT((SHORT) index - 1),
                                   MPFROMP((PSZ) tempText));

                        //swap the data in the pCBZData structure to be consistent with
                        // the data in the ListBox
                        SwapPlugins(pCBZData, index, index - 1);

                        //reselect the previously selected item.
                        hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
                        WinSendMsg(hwndControl, LM_SELECTITEM,
                                   MPFROMSHORT((SHORT) index - 1),
                                   MPFROMSHORT((USHORT) TRUE));

                    }
                }
                break;

                //rearrange plugins in Selected Plugins ListBox
                case PBID_PLUGINDOWN:
                {
                    if ((pDlgData = (DLGDATA *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "Failed to query window words",
                                      0UL);
                        break;
                    }
                    pCBZData = pDlgData->pCBZData;

                    //Get the index of the selected item
                    hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
                    index = (SHORT) WinSendMsg(hwndControl, LM_QUERYSELECTION,
                                             MPFROMSHORT((SHORT) LIT_FIRST),
                                               MPFROMLONG(0));

                    //make sure something is selected
                    if (index == LIT_NONE)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "No Plugin Selected",
                                      0UL);
                        break;
                    }

                    //Get the number of items in the listbox
                    i = (SHORT) WinSendMsg(hwndControl, LM_QUERYITEMCOUNT,
                                           MPFROMLONG(0),
                                           MPFROMLONG(0));

                    //don't push down if already at the bottom!
                    if (index < (i - 1))
                    {
                        //Save the selected item text
                        WinSendMsg(hwndControl, LM_QUERYITEMTEXT,
                                   MPFROM2SHORT((SHORT) index, 64),
                                   MPFROMP((PSZ) tempText));
                        //remove the selected item
                        WinSendMsg(hwndControl, LM_DELETEITEM,
                                   MPFROMSHORT((SHORT) index),
                                   MPFROMLONG(0));
                        //reinsert the item down 1
                        WinSendMsg(hwndControl, LM_INSERTITEM,
                                   MPFROMSHORT((SHORT) index + 1),
                                   MPFROMP((PSZ) tempText));

                        //update the data in pCBZData to match the ListBox
                        SwapPlugins(pCBZData, index, index + 1);

                        //reselect the previously selected item
                        hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
                        WinSendMsg(hwndControl, LM_SELECTITEM,
                                   MPFROMSHORT((SHORT) index + 1),
                                   MPFROMSHORT((USHORT) TRUE));

                    }
                }
                break;

                //remove a plugin from the Selected Plugin ListBox
                case PBID_PLUGINREMOVE:
                {
                    if ((pDlgData = (DLGDATA *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "Failed to query window words",
                                      0UL);
                        break;
                    }
                    pCBZData = pDlgData->pCBZData;

                    //Get the index of the selected plugin
                    hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
                    index = (SHORT) WinSendMsg(hwndControl, LM_QUERYSELECTION,
                                             MPFROMSHORT((SHORT) LIT_FIRST),
                                               MPFROMLONG(0));
                    if (index == LIT_NONE)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "No Plugin Selected",
                                      0UL);
                        break;
                    }

                    //Save the text so it can be added to Available Plugin Listbox
                    WinSendMsg(hwndControl, LM_QUERYITEMTEXT,
                               MPFROM2SHORT((SHORT) index, 64),
                               MPFROMP((PSZ) tempText));
                    //remove item from Selected Plugin ListBox
                    WinSendMsg(hwndControl, LM_DELETEITEM,
                               MPFROMSHORT((SHORT) index),
                               MPFROMLONG(0));

                    //Add the item to the Available Plugins ListBox
                    hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINS);
                    i = (SHORT) WinSendMsg(hwndControl, LM_INSERTITEM,
                                     MPFROMSHORT((SHORT) LIT_SORTASCENDING),
                                           MPFROMP((PSZ) tempText));

                    //Add the dll name to the AvailablePluginsDLL ListBox at the correct index
                    strcpy(tempText, pCBZData->Plugins[index].szPluginDLL);
                    hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINDLLS);
                    WinSendMsg(hwndControl, LM_INSERTITEM,
                               MPFROMSHORT((SHORT) i),
                               MPFROMP((PSZ) tempText));

                    //remove the plugin from the pCBZData structure
                    if (!RemovePlugin(pCBZData, index))
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "Error Removing Plugin!",
                                      0UL);
                        //Put it back in Selected Plugin ListBox?
                        break;
                    }

                    hwndControl = WinWindowFromID(hwnd, STID_PLUGINNAME);
                    WinSetWindowText(hwndControl, "<Plugin Name>");
                    hwndControl = WinWindowFromID(hwnd, STID_PLUGINAUTHOR);
                    WinSetWindowText(hwndControl, "<Plugin Author>");

                    hwndControl = WinWindowFromID(hwnd, PBID_PLUGINUP);
                    WinEnableWindow(hwndControl, FALSE);
                    hwndControl = WinWindowFromID(hwnd, PBID_PLUGINDOWN);
                    WinEnableWindow(hwndControl, FALSE);
                }
                break;

                //configure the plugin selected in the Selected Plugin ListBox
                case PBID_PLUGINCONFIGURE:
                {
                    if ((pDlgData = (DLGDATA *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "Failed to query window words",
                                      0UL);
                        break;
                    }
                    pCBZData = pDlgData->pCBZData;

                    //Get index of the selected plugin
                    hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
                    index = (SHORT) WinSendMsg(hwndControl, LM_QUERYSELECTION,
                                             MPFROMSHORT((SHORT) LIT_FIRST),
                                               MPFROMLONG(0));

                    if (index == LIT_NONE)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "No Plugin Selected",
                                      0UL);
                        break;
                    }

                    if (DosQueryProcAddr(pCBZData->Plugins[index].hModDll, 0,
                                         "CBZPluginSetupDlgProc", (PFN *) & (pPluginDlgProc)) != NO_ERROR)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                              "Unable to get CBZPluginSetupDlgProc Address",
                                      0UL);
                        break;
                    }

                    //if the size of the plugins data is > 0 Allocate memory for making a backup
                    if (pCBZData->Plugins[index].cbData > 0)
                    {
                        // alloc window words for the plugin's data
                        if ((DosAllocMem((PPVOID) & (pDataBackup),
                                         pCBZData->Plugins[index].cbData,
                            PAG_READ | PAG_WRITE | PAG_COMMIT)) != NO_ERROR)
                        {
                            PSUTErrorFunc(hwnd,
                                          "Error",
                                          "Plugin Configure",
                                          "Error Allocating memory for plugin data backup.",
                                          0UL);
                            break;
                        }
                        //copy the plugins data so it can be restored if the dialog returns !PBID_OK
                        memcpy(pDataBackup, pCBZData->Plugins[index].pData,
                               pCBZData->Plugins[index].cbData);
                    }

                    //Popup the plugin's Dialog
                    if ((hwndDialog = WinDlgBox(HWND_DESKTOP,
                                                hwnd,
                                                pPluginDlgProc,
                                            pCBZData->Plugins[index].hModDll,
                                                DLGID_PLUGIN,
                                 pCBZData->Plugins[index].pData)) != PBID_OK)
                    {
                        if (hwndDialog == DID_ERROR)
                        {
                            PSUTErrorFunc(hwnd,
                                          "Error",
                                          "Plugin Configure",
                                       "Error Loading Configuration Dialog",
                                          0UL);
                        }

                        //PBID_CANCEL has been hit, so restore the old data (if any)
                        if (pCBZData->Plugins[index].cbData > 0)
                        {
                            memcpy(pCBZData->Plugins[index].pData, pDataBackup,
                                   pCBZData->Plugins[index].cbData);
                        }

                        break;
                    }

                    //Free the memory from the dataBackup (if any)
                    if (pDataBackup != NULL)
                        DosFreeMem(pDataBackup);
                }
                break;
            }
            return ((MPARAM) 0);
        }
        break;

        case WM_CONTROL:
        {
            switch (SHORT1FROMMP(mp1))
            {
                //update the Selected Plugin Properties info

                case LBID_SELECTEDPLUGINS:
                {

                    if ((pDlgData = (DLGDATA *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "WindowClassDlgProc",
                                      "Failed to query window words",
                                      0UL);
                        break;
                    }
                    pCBZData = pDlgData->pCBZData;

                    if (SHORT2FROMMP(mp1) == LN_SELECT)
                    {
                        //get the index of the selected plugin
                        hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
                        index = (SHORT) WinSendMsg(hwndControl, LM_QUERYSELECTION,
                                             MPFROMSHORT((SHORT) LIT_FIRST),
                                                   MPFROMLONG(0));
                        if (index != LIT_NONE)
                        {
                            //update the enabled state of the push buttons
                            i = (SHORT) WinSendMsg(hwndControl, LM_QUERYITEMCOUNT,
                                                   MPFROMLONG(0),
                                                   MPFROMLONG(0));
                            hwndControl = WinWindowFromID(hwnd, PBID_PLUGINUP);

                            if (index == 0)  //at top so disable the Up button

                            {
                                WinEnableWindow(hwndControl, FALSE);
                            }
                            else
                            {
                                WinEnableWindow(hwndControl, TRUE);
                            }

                            hwndControl = WinWindowFromID(hwnd, PBID_PLUGINDOWN);
                            if ((i - 1) == index)  //at bottom so disable the Bottom button

                            {
                                WinEnableWindow(hwndControl, FALSE);
                            }
                            else
                            {
                                WinEnableWindow(hwndControl, TRUE);
                            }

                            //more updates to enabled status
                            hwndControl = WinWindowFromID(hwnd, PBID_SELECTPLUGIN);
                            WinEnableWindow(hwndControl, FALSE);
                            hwndControl = WinWindowFromID(hwnd, PBID_PLUGINREMOVE);
                            WinEnableWindow(hwndControl, TRUE);

                            if (DosQueryProcAddr(pCBZData->Plugins[index].hModDll, 0, "CBZPluginInfo",
                               (PFN *) & (pPluginInfoFunction)) != NO_ERROR)
                            {
                                PSUTErrorFunc(hwnd,
                                              "Error",
                                              "WindowClassDlgProc",
                                    "Failed to query CBZPluginInfo Address",
                                              0UL);
                                break;

                            }

                            pPluginInfoFunction(&pluginInfo);
                            //update the windowText
                            hwndControl = WinWindowFromID(hwnd, STID_PLUGINNAME);
                            WinSetWindowText(hwndControl, pluginInfo.PluginName);
                            hwndControl = WinWindowFromID(hwnd, STID_PLUGINAUTHOR);
                            WinSetWindowText(hwndControl, pluginInfo.PluginAuthor);
                            hwndControl = WinWindowFromID(hwnd, STID_PLUGINDATE);
                            WinSetWindowText(hwndControl, pluginInfo.PluginDate);

                            //de-select all items in the Available Plugins ListBox
                            hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINS);
                            WinSendMsg(hwndControl, LM_SELECTITEM,
                                       MPFROMSHORT((SHORT) LIT_NONE),
                                       MPFROMSHORT((USHORT) TRUE));

                            //enable configure button
                            hwndControl = WinWindowFromID(hwnd, PBID_PLUGINCONFIGURE);
                            WinEnableWindow(hwndControl, TRUE);
                        }
                        else
                        {
                            //disable all the buttons
                            hwndControl = WinWindowFromID(hwnd, PBID_PLUGINUP);
                            WinEnableWindow(hwndControl, FALSE);
                            hwndControl = WinWindowFromID(hwnd, PBID_PLUGINDOWN);
                            WinEnableWindow(hwndControl, FALSE);
                            hwndControl = WinWindowFromID(hwnd, PBID_SELECTPLUGIN);
                            WinEnableWindow(hwndControl, FALSE);
                            hwndControl = WinWindowFromID(hwnd, PBID_PLUGINCONFIGURE);
                            WinEnableWindow(hwndControl, FALSE);

                        }
                    }
                    else if(SHORT2FROMMP(mp1) == LN_ENTER) {
                      WinPostMsg(hwnd,WM_COMMAND,MPFROMSHORT(PBID_PLUGINCONFIGURE),MPFROM2SHORT(CMDSRC_PUSHBUTTON,0));  
                    }
                }
                break;

                //update the Selected Plugin Properties info
                case LBID_AVAILABLEPLUGINS:
                {
                    if (SHORT2FROMMP(mp1) == LN_SELECT)
                    {
                        hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINS);

                        index = (SHORT) WinSendMsg(hwndControl, LM_QUERYSELECTION,
                                             MPFROMSHORT((SHORT) LIT_FIRST),
                                                   MPFROMLONG(0));
                        if (index != LIT_NONE)
                        {
                            //Get the DLL name for the selected Plugin
                            hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLEPLUGINDLLS);
                            WinSendMsg(hwndControl, LM_QUERYITEMTEXT,
                                       MPFROM2SHORT((SHORT) index, 64),
                                       MPFROMP((PSZ) tempText));

                            PrfQueryProfileString(HINI_USERPROFILE,
                                                  "CandyBarZ",
                                                  "BasePath",
                                                  NULL,
                                                  pszBase,
                                                  CCHMAXPATH);
                            strcat(pszBase, "plugins");
                            _makepath(pszFile, NULL, pszBase, tempText, "DLL");

                            if (DosLoadModule(szError, sizeof(szError), pszFile, &hModPlug) == NO_ERROR)
                            {
                                if (DosQueryProcAddr(hModPlug, 0, "CBZPluginInfo", (PFN *) & (pPluginInfoFunction)) == NO_ERROR)
                                {
                                    pPluginInfoFunction(&pluginInfo);
                                    hwndControl = WinWindowFromID(hwnd, STID_PLUGINNAME);
                                    WinSetWindowText(hwndControl, pluginInfo.PluginName);
                                    hwndControl = WinWindowFromID(hwnd, STID_PLUGINAUTHOR);
                                    WinSetWindowText(hwndControl, pluginInfo.PluginAuthor);
                                    hwndControl = WinWindowFromID(hwnd, STID_PLUGINDATE);
                                    WinSetWindowText(hwndControl, pluginInfo.PluginDate);

                                    hwndControl = WinWindowFromID(hwnd, PBID_PLUGINUP);
                                    WinEnableWindow(hwndControl, FALSE);
                                    hwndControl = WinWindowFromID(hwnd, PBID_PLUGINDOWN);
                                    WinEnableWindow(hwndControl, FALSE);
                                    hwndControl = WinWindowFromID(hwnd, PBID_PLUGINREMOVE);
                                    WinEnableWindow(hwndControl, FALSE);
                                    hwndControl = WinWindowFromID(hwnd, PBID_SELECTPLUGIN);
                                    WinEnableWindow(hwndControl, TRUE);
                                }
                            }
                            DosFreeModule(hModPlug);

                            hwndControl = WinWindowFromID(hwnd, LBID_SELECTEDPLUGINS);
                            WinSendMsg(hwndControl, LM_SELECTITEM,
                                       MPFROMSHORT((SHORT) LIT_NONE),
                                       MPFROMSHORT((USHORT) TRUE));

                            hwndControl = WinWindowFromID(hwnd, PBID_PLUGINCONFIGURE);
                            WinEnableWindow(hwndControl, FALSE);

                        }
                    }
                    else if(SHORT2FROMMP(mp1) == LN_ENTER) {
                      WinPostMsg(hwnd,WM_COMMAND,MPFROMSHORT(PBID_SELECTPLUGIN),MPFROM2SHORT(CMDSRC_PUSHBUTTON,0));  
                    }

                }
                break;
            }
            return ((MRESULT) 0);
        }
        break;

        case WM_HELP:
        {
            HWND hwndHelp = WinQueryHelpInstance(hwnd);

            if (hwndHelp)
            {
                // get id of control with focus
                hwndFocus = WinQueryFocus(HWND_DESKTOP);
                sControlId = WinQueryWindowUShort(hwndFocus, QWS_ID);

                switch (sControlId)
                {
                    case LBID_SELECTEDPLUGINS:
                    case LBID_AVAILABLEPLUGINS:
                    {
                       sHelpId = HLPIDX_PLUGINS;
                    }break;

                    case PBID_PLUGINREMOVE:
                    case PBID_SELECTPLUGIN:
                    {
                       sHelpId = HLPIDX_SELECTREMOVE;
                    }break;

                    case PBID_PLUGINUP:
                    case PBID_PLUGINDOWN:
                    {
                       sHelpId = HLPIDX_FORMAT;
                    }break;


                    default:
                    {
                        sHelpId = HLPIDX_PAGE1;
                    }
                    break;
                }

                // display help
                ulRc = (ULONG) WinSendMsg(hwndHelp,
                                          HM_DISPLAY_HELP,
                                          MPFROM2SHORT(sHelpId, NULL),
                                          MPFROMSHORT(HM_RESOURCEID));
                if (ulRc)
                {
                    printf("help error\n");
                }
            }

            return ((MPARAM) 0);
        }
        break;

        case WM_DESTROY:
        {
            //free up the memory allocated for the window words
            if ((pDlgData = (DLGDATA *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
            {
                PSUTErrorFunc(hwnd,
                              "Error",
                              "WindowClassDlgProc",
                              "Failed to query window words",
                              0UL);
                break;
            }
            pCBZData = pDlgData->pCBZData;

            //free resources in the pCBZData structure.
            for (i = 0; i < pCBZData->cbPlugins; i++)
            {
                if (pCBZData->Plugins[i].hModDll != NULLHANDLE)
                    DosFreeModule(pCBZData->Plugins[i].hModDll);
                if (pCBZData->Plugins[i].pData != NULL)
                    DosFreeMem(pCBZData->Plugins[i].pData);
            }
            DosFreeMem(pCBZData);

            DosFreeMem(pDlgData);
            WinSetWindowPtr(hwnd, QWL_USER, NULL);
        }
        break;

    }
    return (WinDefDlgProc(hwnd, msg, mp1, mp2));
}

