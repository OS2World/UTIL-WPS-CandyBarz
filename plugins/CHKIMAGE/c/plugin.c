/* $Id: plugin.c,v 1.4 2000/08/30 21:04:40 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998-2000, this code may not be used for any military purpose

******************************************************************************/
#include "plugin.h"

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pInfo)
{
    strcpy(pInfo->PluginName, "CandyBarZ CheckBox/RadioButton Images");
    strcpy(pInfo->PluginAuthor, "CandyBarZ Team @ OS/2 Netlabs");
    strcpy(pInfo->PluginDate, "Aug. 29, 2000");
    pInfo->lWindowTypes = CBZ_RADIOBUTTON | CBZ_CHECKBOX;
    return;
}

//called to allocate memory and setup initial settings
BOOL CBZInit(HINI hIni, char szName[], char szClass[], PVOID * pData)
{
    PLUGINSHARE *pPluginShare;
    PLUGINSHARE *pPluginData;
    char szReadName[128];
    char szShareName[64];
    ULONG szData;

    // alloc window words for the plugin's data
    if ((DosAllocMem((PPVOID) (pData),
                     sizeof(PLUGINSHARE),
                     PAG_READ | PAG_WRITE | PAG_COMMIT)) != NO_ERROR)
    {
        return FALSE;
    }

    //initialize the plugin's data to zeros.
    memset(*pData, 0, sizeof(PLUGINSHARE));

    pPluginData = (PLUGINSHARE *) * pData;

    strcpy(szShareName, PLUGIN_SHARE);
    strcat(szShareName, szClass);

    //custom name was specified, try to load the custom config for it.
    if (strlen(szName) > 0)
    {
        strcpy(szReadName, szName);
        strcat(szReadName, "_");
        strcat(szReadName, szClass);
        strcat(szReadName, "_CHKIMAGEPlugin");
        szData = sizeof(PLUGINSHARE);

        // get shared mem
        if (!PrfQueryProfileData(hIni,
                                 "CustomOptionsData",
                                 szReadName,
                                 pPluginData,
                                 &szData))
        {
            // if shared mem is not available, use defaults!
            if (DosGetNamedSharedMem((PPVOID) & pPluginData, szShareName, PAG_READ) != NO_ERROR)
            {
                //use some defaults?
                strcpy(pPluginData->szActiveFile, "NONE");
                strcpy(pPluginData->szActiveFileMask, "NONE");
                strcpy(pPluginData->szInactiveFile, "NONE");
                strcpy(pPluginData->szInactiveFileMask, "NONE");
                strcpy(pPluginData->szSelectedActiveFile, "NONE");
                strcpy(pPluginData->szSelectedActiveFileMask, "NONE");
                strcpy(pPluginData->szSelectedInactiveFile, "NONE");
                strcpy(pPluginData->szSelectedInactiveFileMask, "NONE");
                pPluginData->bActiveEnabled = FALSE;
                pPluginData->bInactiveEnabled = FALSE;

                if (!PrfWriteProfileData(hIni,
                                         "CustomOptionsData",
                                         szReadName,
                                         pPluginData,
                                         sizeof(PLUGINSHARE)))
                {
                    //not a fatal error
                }
            }
        }

    }                           //end if (szName > 0)

    else
    {
        //Load defaults!  No custom name specified.

        // if shared mem is not available, try allocating it!
        if (DosGetNamedSharedMem((PPVOID) & pPluginShare, szShareName, PAG_READ) != NO_ERROR)
        {
            //try Allocating the SharedMem space for this plugin!
            if (DosAllocSharedMem((PPVOID) & pPluginShare,
                                  szShareName,
                                  sizeof(PLUGINSHARE),
                             PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
                return (FALSE); //ignore this plugin... It must be broken?

            // clear it
            memset(pPluginShare, 0, sizeof(PLUGINSHARE));

            szData = sizeof(PLUGINSHARE);

            // get shared mem
            strcpy(szReadName, szClass);
            strcat(szReadName, "_CHKIMAGEPlugin");
            if (!PrfQueryProfileData(hIni,
                                     "UserOptionsData",
                                     szReadName,
                                     pPluginShare,
                                     &szData))
            {
                strcpy(pPluginShare->szActiveFile, "NONE");
                strcpy(pPluginShare->szActiveFileMask, "NONE");
                strcpy(pPluginShare->szInactiveFile, "NONE");
                strcpy(pPluginShare->szInactiveFileMask, "NONE");
                strcpy(pPluginShare->szSelectedActiveFile, "NONE");
                strcpy(pPluginShare->szSelectedActiveFileMask, "NONE");
                strcpy(pPluginShare->szSelectedInactiveFile, "NONE");
                strcpy(pPluginShare->szSelectedInactiveFileMask, "NONE");
                pPluginShare->bActiveEnabled = FALSE;
                pPluginShare->bInactiveEnabled = FALSE;

                if (!PrfWriteProfileData(hIni,
                                         "UserOptionsData",
                                         szReadName,
                                         pPluginShare,
                                         sizeof(PLUGINSHARE)))
                {
                    //not a fatal error.
                }
            }
        }                       //end if

        //copy from shared memory to each windows window words.
        strcpy(pPluginData->szActiveFile, pPluginShare->szActiveFile);
        strcpy(pPluginData->szActiveFileMask, pPluginShare->szActiveFileMask);
        strcpy(pPluginData->szInactiveFile, pPluginShare->szInactiveFile);
        strcpy(pPluginData->szInactiveFileMask, pPluginShare->szInactiveFileMask);
        strcpy(pPluginData->szSelectedActiveFile, pPluginShare->szSelectedActiveFile);
        strcpy(pPluginData->szSelectedActiveFileMask, pPluginShare->szSelectedActiveFileMask);
        strcpy(pPluginData->szSelectedInactiveFile, pPluginShare->szSelectedInactiveFile);
        strcpy(pPluginData->szSelectedInactiveFileMask, pPluginShare->szSelectedInactiveFileMask);
        pPluginData->bActiveEnabled = pPluginShare->bActiveEnabled;
        pPluginData->bInactiveEnabled = pPluginShare->bInactiveEnabled;
    } //end else

    return TRUE;
}

//change current settings.
BOOL CBZApply(char szClass[], KEY_VALUE * kv, int count, int enabledState, PVOID pData)
{
    int i;
    PLUGINSHARE *pPluginShare;
    char szShareName[32];
    char szSaveName[32];
    BOOL bDefaultApply = FALSE;

    pPluginShare = (PLUGINSHARE *) pData;
    //if null, this is a default apply, so load shared memory.
    if (pPluginShare == NULL)
    {
        strcpy(szShareName, PLUGIN_SHARE);
        strcat(szShareName, szClass);

        // if shared mem is not available, try allocating temporary space!
        if (DosGetNamedSharedMem((PPVOID) & pPluginShare, szShareName, PAG_READ | PAG_WRITE) != NO_ERROR)
        {
            return FALSE;
        }
        bDefaultApply = TRUE;
    }

    if ((enabledState == 0) || (enabledState == 2))
        pPluginShare->bActiveEnabled = TRUE;
    else
        pPluginShare->bActiveEnabled = FALSE;

    if ((enabledState == 1) || (enabledState == 2))
        pPluginShare->bInactiveEnabled = TRUE;
    else
        pPluginShare->bInactiveEnabled = FALSE;

    //loop through each key/value pair and set appropriate parameters
    for (i = 0; i < count; i++)
    {
        if (pPluginShare->bActiveEnabled)
        {
            if (strcmp(kv[i].key, "ActiveImageFile") == 0)
            {
                strcpy(pPluginShare->szActiveFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveImageFileMask") == 0)
            {
                strcpy(pPluginShare->szActiveFileMask, kv[i].value);
            }
            else if (strcmp(kv[i].key, "SelectedActiveImageFile") == 0)
            {
                strcpy(pPluginShare->szSelectedActiveFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "SelectedActiveImageFileMask") == 0)
            {
                strcpy(pPluginShare->szSelectedActiveFileMask, kv[i].value);
            }
        } //end bInactive if condition

        if (pPluginShare->bInactiveEnabled)
        {
            if (strcmp(kv[i].key, "InactiveImageFile") == 0)
            {
                strcpy(pPluginShare->szInactiveFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveImageFileMask") == 0)
            {
                strcpy(pPluginShare->szInactiveFileMask, kv[i].value);
            }
            else if (strcmp(kv[i].key, "SelectedInactiveImageFile") == 0)
            {
                strcpy(pPluginShare->szSelectedInactiveFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "SelectedInactiveImageFileMask") == 0)
            {
                strcpy(pPluginShare->szSelectedInactiveFileMask, kv[i].value);
            }
        }  //end bInactive if condition
    }  //end for loop

    if (bDefaultApply)
        DosFreeMem(pPluginShare);
    return TRUE;
}

//save current settings to the given ini file
BOOL CBZSave(HINI hIni, char szClass[], PVOID pData, char szCustomName[])
{
    PLUGINSHARE *pPluginShare;
    char szShareName[32];
    char szSaveName[128];

    pPluginShare = (PLUGINSHARE *) pData;
    //if null, this is a default apply, so load shared memory.
    if (pPluginShare == NULL)
    {
        strcpy(szShareName, PLUGIN_SHARE);
        strcat(szShareName, szClass);

        // if shared mem is not available, try allocating temporary space!
        if (DosGetNamedSharedMem((PPVOID) & pPluginShare, szShareName, PAG_READ | PAG_WRITE) != NO_ERROR)
        {
            //try Allocating temporary space for the plugin data so we can write to the ini file
            if (DosAllocMem((PPVOID) & pPluginShare,
                            sizeof(PLUGINSHARE),
                            PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
                return (FALSE);     //No Good... can't save.

        }

        //write pPluginShare to the ini file!
        strcpy(szSaveName, szClass);
        strcat(szSaveName, "_CHKIMAGEPlugin");
        PrfWriteProfileData(hIni,
                            "UserOptionsData",
                            szSaveName,
                            pPluginShare,
                            sizeof(PLUGINSHARE));

        DosFreeMem(pPluginShare);
    }
    else
    {
        strcpy(szSaveName, szCustomName);
        strcat(szSaveName, "_");
        strcat(szSaveName, szClass);
        strcat(szSaveName, "_CHKIMAGEPlugin");

        PrfWriteProfileData(hIni,
                            "CustomOptionsData",
                            szSaveName,
                            pPluginShare,
                            sizeof(PLUGINSHARE));

    }

    return (TRUE);
}

//cleanup resources generated by CBZInit
BOOL CBZDestroy(HWND hwnd, PVOID pData)
{
    PLUGINSHARE *pPluginData;

    pPluginData = (PLUGINSHARE *) pData;

    if (pPluginData->hbmActive != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmActive);
    if (pPluginData->hbmActiveMask != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmActiveMask);
    if (pPluginData->hbmInactive != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmInactive);
    if (pPluginData->hbmInactiveMask != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmInactiveMask);

    if (pPluginData->hbmSelectedActive != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmSelectedActive);
    if (pPluginData->hbmSelectedActiveMask != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmSelectedActiveMask);
    if (pPluginData->hbmSelectedInactive != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmSelectedInactive);
    if (pPluginData->hbmSelectedInactiveMask != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmSelectedInactiveMask);

    DosFreeMem(pPluginData);
    return (TRUE);
}

//Draw the Plugin effect into the given presentation space
BOOL CBZPluginRender(PSPAINT * pPaint, PVOID pData, short sActive)
{
    PLUGINSHARE *pPluginData;
    pPluginData = (PLUGINSHARE *) pData;

    if (pPluginData == NULL)
        return (FALSE);

    if (( ((sActive == 1) || (sActive == 3))   && !(pPluginData->bActiveEnabled)) ||
            ( ((sActive == 0) || (sActive == 2)) && !(pPluginData->bInactiveEnabled)))
        return (TRUE);

    if ( (sActive == 1) && (pPluginData->hbmActive == NULLHANDLE) ) {
      pPluginData->hbmActive = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szActiveFile, pPaint->rectlWindow.yTop);
      if (pPluginData->hbmActiveMask == NULLHANDLE)
        pPluginData->hbmActiveMask = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szActiveFileMask, pPaint->rectlWindow.yTop);
    }
    else if ( (sActive == 0) && (pPluginData->hbmInactive == NULLHANDLE) ) {
        pPluginData->hbmInactive = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szInactiveFile, pPaint->rectlWindow.yTop);
        if (pPluginData->hbmInactiveMask == NULLHANDLE)
          pPluginData->hbmInactiveMask = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szInactiveFileMask, 
                                                             pPaint->rectlWindow.yTop);
    }
    else if ( (sActive == 3) && (pPluginData->hbmSelectedActive == NULLHANDLE) ) {
        pPluginData->hbmSelectedActive = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szSelectedActiveFile,
                                                             pPaint->rectlWindow.yTop);
        if ( pPluginData->hbmSelectedActiveMask == NULLHANDLE)
          pPluginData->hbmSelectedActiveMask = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szSelectedActiveFileMask,
                                                                   pPaint->rectlWindow.yTop);
    }
    else if ( (sActive == 2) && (pPluginData->hbmSelectedInactive == NULLHANDLE) ) {
        pPluginData->hbmSelectedInactive = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szSelectedInactiveFile,
                                                               pPaint->rectlWindow.yTop);
        if (pPluginData->hbmSelectedInactiveMask == NULLHANDLE)
        pPluginData->hbmSelectedInactiveMask = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szSelectedInactiveFileMask,
                                                               pPaint->rectlWindow.yTop);
    }

    //make sure no error occured.
    if ( (sActive == 1) && (pPluginData->hbmActive != NULLHANDLE) )
    {
        DrawImage(pPaint, pPluginData->hbmActive, pPluginData->hbmActiveMask);
    }
    else if ( (sActive == 0) && (pPluginData->hbmInactive != NULLHANDLE) )
    {
        DrawImage(pPaint, pPluginData->hbmInactive, pPluginData->hbmInactiveMask);
    }
    else if ( (sActive == 3) && (pPluginData->hbmSelectedActive != NULLHANDLE) )
    {
        DrawImage(pPaint, pPluginData->hbmSelectedActive, pPluginData->hbmSelectedActiveMask);
    }
    else if ( (sActive == 2) && (pPluginData->hbmSelectedInactive != NULLHANDLE) )
    {
        DrawImage(pPaint, pPluginData->hbmSelectedInactive, pPluginData->hbmSelectedInactiveMask);
    }

    return (TRUE);
}

//Called before the default wnd procedure if you wish to provide additional
//  functionality.  (i.e. modify what happens with mouse clicks, drag/drop, etc...
BOOL CBZPluginWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2,
                        PVOID pData, PFNWP pfnOrigWndProc, MRESULT *rc)
{
    switch (msg)
    {
    }
    return FALSE; //false indicates that we haven't processed the message!
}

//A Dlg procedure if the plugin has selectable settings.
MRESULT EXPENTRY CBZPluginSetupDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    short sControlId;
    PLUGINSHARE *pPluginData;
    FILEDLG filedlg;
    HWND hwndDlg;
    static char chrBmpPath[CCHMAXPATH]={0};

    switch (msg)
    {
        case WM_INITDLG:
        {
            if ((pPluginData = (PLUGINSHARE *) mp2) == NULL)
            {
                //Error msg..
                WinDismissDlg(hwnd, DID_ERROR);
                return (MRFROMLONG(FALSE));
            }
            WinSetWindowPtr(hwnd, QWL_USER, pPluginData);  // store window words

            WinSendMsg(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGEMASK), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);

            WinSetWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), pPluginData->szActiveFile);
            WinSetWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), pPluginData->szActiveFileMask);
            WinSetWindowText(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGE), pPluginData->szSelectedActiveFile);
            WinSetWindowText(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGEMASK), pPluginData->szSelectedActiveFileMask);

            WinSendMsg(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGEMASK), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);

            WinSetWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), pPluginData->szInactiveFile);
            WinSetWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), pPluginData->szInactiveFileMask);
            WinSetWindowText(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGE), pPluginData->szSelectedInactiveFile);
            WinSetWindowText(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGEMASK), pPluginData->szSelectedInactiveFileMask);

            if (pPluginData->bActiveEnabled)
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGEMASK), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGEMASK), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDACTIVEIMAGEMASK), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGEMASK), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGEMASK), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDACTIVEIMAGEMASK), FALSE);
            }
            if (pPluginData->bInactiveEnabled)
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGEMASK), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGEMASK), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDINACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDINACTIVEIMAGE), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGEMASK), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGEMASK), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDINACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDINACTIVEIMAGEMASK), FALSE);
            }

        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
                case PBID_CHANGEACTIVEIMAGE:
                case PBID_CHANGEACTIVEIMAGEMASK:
                case PBID_CHANGEINACTIVEIMAGE:
                case PBID_CHANGEINACTIVEIMAGEMASK:
                case PBID_CHANGESELECTEDACTIVEIMAGE:
                case PBID_CHANGESELECTEDACTIVEIMAGEMASK:
                case PBID_CHANGESELECTEDINACTIVEIMAGE:
                case PBID_CHANGESELECTEDINACTIVEIMAGEMASK:
                {
             
                    //popup file change dialog!
                    memset(&filedlg, 0, sizeof(FILEDLG));  // init filedlg struct

                    filedlg.cbSize = sizeof(FILEDLG);
                    filedlg.fl = FDS_OPEN_DIALOG;
                    filedlg.pszTitle = "Choose an image";
                    
                    if(strlen(chrBmpPath))
                      strcpy(filedlg.szFullFile, chrBmpPath);
                    else
                      strcpy(filedlg.szFullFile, "*.*");

                    if ((hwndDlg = WinFileDlg(HWND_DESKTOP, hwnd, &filedlg)) == NULLHANDLE)
                    {
                        return (FALSE);
                    }

                    if (filedlg.lReturn != DID_OK)  // selected one

                    {
                        break;
                    }

                    if (sControlId == PBID_CHANGEACTIVEIMAGE)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_CHANGEACTIVEIMAGEMASK)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_CHANGEINACTIVEIMAGE)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_CHANGEINACTIVEIMAGEMASK)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_CHANGESELECTEDACTIVEIMAGE)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGE), filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_CHANGESELECTEDACTIVEIMAGEMASK)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGEMASK), filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_CHANGESELECTEDINACTIVEIMAGE)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGE), filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_CHANGESELECTEDINACTIVEIMAGEMASK)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGEMASK), filedlg.szFullFile);
                    }
                    strcpy(chrBmpPath,filedlg.szFullFile);

                }
                break;


                case PBID_OK:
                {
                    if ((pPluginData = (PLUGINSHARE *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        //error message..
                        break;
                    }

                    WinQueryWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), CCHMAXPATH, pPluginData->szActiveFile);
                    WinQueryWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), CCHMAXPATH, pPluginData->szActiveFileMask);
                    WinQueryWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), CCHMAXPATH, pPluginData->szInactiveFile);
                    WinQueryWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), CCHMAXPATH, pPluginData->szInactiveFileMask);

                    WinQueryWindowText(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGE), CCHMAXPATH,
                                       pPluginData->szSelectedActiveFile);
                    WinQueryWindowText(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGEMASK), CCHMAXPATH,
                                       pPluginData->szSelectedActiveFileMask);
                    WinQueryWindowText(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGE), CCHMAXPATH,
                                       pPluginData->szSelectedInactiveFile);
                    WinQueryWindowText(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGEMASK), CCHMAXPATH,
                                       pPluginData->szSelectedInactiveFileMask);

                    if ( WinQueryButtonCheckstate(hwnd, CBID_ACTIVEENABLED) )
                        pPluginData->bActiveEnabled = TRUE;
                    else
                        pPluginData->bActiveEnabled = FALSE;

                    if ( WinQueryButtonCheckstate(hwnd, CBID_INACTIVEENABLED) )
                        pPluginData->bInactiveEnabled = TRUE;
                    else
                        pPluginData->bInactiveEnabled = FALSE;


                    //update!
                    WinDismissDlg(hwnd, PBID_OK);
                }
                break;

                case PBID_CANCEL:
                {
                    //don't update shared Memory!
                    WinDismissDlg(hwnd, PBID_CANCEL);
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
                case CBID_ACTIVEENABLED:
                {
                    // if Enabled
                    if (!WinQueryButtonCheckstate(hwnd, CBID_ACTIVEENABLED))
                    {
                        // check button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGEMASK), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGEMASK), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDACTIVEIMAGE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDACTIVEIMAGEMASK), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGEMASK), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDACTIVEIMAGEMASK), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDACTIVEIMAGEMASK), FALSE);
                    }
                }
                break;

                case CBID_INACTIVEENABLED:
                {
                    // if Enabled
                    if (!WinQueryButtonCheckstate(hwnd, CBID_INACTIVEENABLED))
                    {
                        // check button
                        WinCheckButton(hwnd, CBID_INACTIVEENABLED, TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGEMASK), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGEMASK), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDINACTIVEIMAGE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDINACTIVEIMAGEMASK), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGEMASK), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_SELECTEDINACTIVEIMAGEMASK), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDINACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGESELECTEDINACTIVEIMAGEMASK), FALSE);
                    }
                }
                break;

            }
            return ((MRESULT) 0);
        }
        break;

    }
    return (WinDefDlgProc(hwnd, msg, mp1, mp2));
}

BOOL CBZWriteAttributes(char *retStr, PVOID pData)
{
    PLUGINSHARE *pPluginData;
    char tmpText[33];

    pPluginData = (PLUGINSHARE *) pData;
    if (pPluginData == NULL)
        return FALSE;

    if (pPluginData->bActiveEnabled)
    {
        if (pPluginData->bInactiveEnabled)
            AddEnabledAttribute(retStr, 2);
        else
            AddEnabledAttribute(retStr, 0);
    }
    else if (pPluginData->bInactiveEnabled)
        AddEnabledAttribute(retStr, 1);
    else
        return FALSE;

    if (pPluginData->bActiveEnabled)
    {
        AddStringAttribute(retStr, "ActiveImageFile", pPluginData->szActiveFile);
        AddStringAttribute(retStr, "ActiveImageFileMask", pPluginData->szActiveFileMask);
        AddStringAttribute(retStr, "SelectedActiveImageFile", pPluginData->szSelectedActiveFile);
        AddStringAttribute(retStr, "SelectedActiveImageFileMask", pPluginData->szSelectedActiveFileMask);
    }

    if (pPluginData->bInactiveEnabled)
    {
        AddStringAttribute(retStr, "InactiveImageFile", pPluginData->szInactiveFile);
        AddStringAttribute(retStr, "InactiveImageFileMask", pPluginData->szInactiveFileMask);
        AddStringAttribute(retStr, "SelectedInactiveImageFile", pPluginData->szSelectedInactiveFile);
        AddStringAttribute(retStr, "SelectedInactiveImageFileMask", pPluginData->szSelectedInactiveFileMask);
    }

    return (TRUE);
};

HBITMAP SetupBitmapFromFile(HWND hwnd, char *pszFileName, long lVertical)
{
    HAB hab = WinQueryAnchorBlock(hwnd);
    PSTBFILE *pFile;
    HBITMAP hbm;

    if (DosAllocMem((PPVOID) & pFile, sizeof(PSTBFILE), PAG_READ | PAG_WRITE | PAG_COMMIT)
            != NO_ERROR)
    {
        return (NULLHANDLE);
    }
    memset(pFile, 0, sizeof(PSTBFILE));

    if (PSMMFillImageFormat(pszFileName, &(pFile->mmFormat), &(pFile->mmioInfo)) != TRUE)
    {
        DosFreeMem(pFile);
        return (NULLHANDLE);
    }
    if ((pFile->hmmio = PSMMInitializeImageFile(pszFileName,
                                                &(pFile->mmioInfo),
                                         &(pFile->mmImgHdr))) == NULLHANDLE)
    {
        DosFreeMem(pFile);
        return (NULLHANDLE);
    }
    pFile->cbRow = ((pFile->mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cx *
      pFile->mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cBitCount + 31) / 32) * 4;
    pFile->cbData = pFile->cbRow * pFile->mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cy;

    if (DosAllocMem(&(pFile->pvImage), pFile->cbData, PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        PSMMCloseImageFile(pFile->hmmio);
        DosFreeMem(pFile);
        return (NULLHANDLE);
    }
    if (PSMMReadImageFile(pFile->hmmio, pFile->pvImage, pFile->cbData) != TRUE)
    {
        DosFreeMem(pFile->pvImage);
        PSMMCloseImageFile(pFile->hmmio);
        DosFreeMem(pFile);
    }
    PSMMCloseImageFile(pFile->hmmio);

    // convert data to system bitcount
    pFile->hdc = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE);
    pFile->slHPS.cx = pFile->mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cx;
    pFile->slHPS.cy = pFile->mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cy;
    pFile->hps = GpiCreatePS(hab,
                             pFile->hdc,
                             &(pFile->slHPS),
                          PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
    pFile->hbm = GpiCreateBitmap(pFile->hps,
                             &(pFile->mmImgHdr.mmXDIBHeader.BMPInfoHeader2),
                                 0L,
                                 NULL,
                                 NULL);
    GpiSetBitmap(pFile->hps, pFile->hbm);
    GpiSetBitmapBits(pFile->hps,
                     0L,
                     (long) pFile->slHPS.cy,
                     (PBYTE) pFile->pvImage,
            (PBITMAPINFO2) & (pFile->mmImgHdr.mmXDIBHeader.BMPInfoHeader2));

    hbm = pFile->hbm;
/*
   hbm = PSBMBLTToSize( hab,
   pFile->hps,
   pFile->hbm,
   ( lVertical * 1.0 ) /
   ( WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN ) * 1.0 ),
   SCALE_VERTICAL );
 */
    GpiDeleteBitmap(pFile->hbm);

    // free everything up
    GpiSetBitmap(pFile->hps, NULLHANDLE);
    GpiDestroyPS(pFile->hps);
    DevCloseDC(pFile->hdc);
    DosFreeMem(pFile->pvImage);
    DosFreeMem(pFile);

    return (hbm);
}

// tiles an image across preview window
BOOL DrawImage(PSPAINT * pPaint, HBITMAP hbm, HBITMAP hbmMask)
{
    SIZEL slHps;
    BITMAPINFOHEADER2 bif2;
    BITMAPINFOHEADER2 bif;
    POINTL aptl[3];
    HDC hdc;
    HPS hps;
    HAB hab = WinQueryAnchorBlock(pPaint->hwnd);
    long starty;
    RECTL rectl;

    rectl.xLeft = pPaint->rectlWindow.xLeft;

    /*
      if ( pPaint->rectlWindow.xRight < (pPaint->rectlWindow.xLeft + 16) )
        rectl.xRight = pPaint->rectlWindow.xRight;
        else
        rectl.xRight = pPaint->rectlWindow.xLeft + 16;
        */
   
  

    if ( (pPaint->rectlWindow.yTop - pPaint->rectlWindow.yBottom) < 16)
    {
        rectl.yTop = pPaint->rectlWindow.yTop;
        rectl.yBottom = pPaint->rectlWindow.yBottom;
    }
    else
    {
        rectl.yBottom = ((pPaint->rectlWindow.yTop + pPaint->rectlWindow.yBottom + 1) / 2) - 8;
        rectl.yTop = rectl.yBottom + 16;
    }


    // setup source bitmap
    hdc = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE);
    // get bitmap info
    memset(&bif2, 0, sizeof(bif2));
    bif2.cbFix = sizeof(bif2);
    GpiQueryBitmapInfoHeader(hbm, &bif2);
    // create source hps
    slHps.cx = bif2.cx;
    slHps.cy = bif2.cy;
    hps = GpiCreatePS(hab,
                      hdc,
                      &slHps,
                      PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);


    if ( pPaint->rectlWindow.xRight < (pPaint->rectlWindow.xLeft + slHps.cx) )
      rectl.xRight = pPaint->rectlWindow.xRight;
    else
      rectl.xRight = pPaint->rectlWindow.xLeft + slHps.cx;
   
    // initial blt location
    aptl[0].x = rectl.xLeft;
    aptl[0].y = rectl.yBottom;
    aptl[1].x = rectl.xRight;
    aptl[1].y = rectl.yTop;
    aptl[2].x = 0;
    aptl[2].y = 0;


    if(hbmMask) {
      // set mask bmp into hps
      GpiSetBitmap(hps, hbmMask);
      GpiBitBlt(pPaint->hps, hps, 3L, aptl, ROP_MERGEPAINT, BBO_IGNORE);
      // set bmp into hps
      GpiSetBitmap(hps, hbm);
      GpiBitBlt(pPaint->hps, hps, 3L, aptl, ROP_SRCAND, BBO_IGNORE);
   }
    else {      
      // set bmp into hps
      GpiSetBitmap(hps, hbm);
      GpiBitBlt(pPaint->hps, hps, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE);
    }

    // clean up
    GpiSetBitmap(hps, NULLHANDLE);
    GpiDestroyPS(hps);
    DevCloseDC(hdc);

    return (TRUE);
}



