/* $Id: plugin.c,v 1.3 2000/09/09 11:00:22 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "plugin.h"

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pInfo)
{
    strcpy(pInfo->PluginName, "CandyBarZ Growable Images");
    strcpy(pInfo->PluginAuthor, "CandyBarZ Team @ OS/2 Netlabs");
    strcpy(pInfo->PluginDate, "10 Sep, 2000");
    pInfo->lWindowTypes = CBZ_TITLEBAR | CBZ_FRAME | CBZ_PUSHBUTTON;
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
        strcat(szReadName, "_SZEIMAGEPlugin");
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
                strcpy(pPluginData->szInactiveFile, "NONE");
                strcpy(pPluginData->szActiveFileMask, "NONE");
                strcpy(pPluginData->szInactiveFileMask, "NONE");

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
            strcat(szReadName, "_SZEIMAGEPlugin");
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
        strcpy(pPluginData->szInactiveFile, pPluginShare->szInactiveFile);
        strcpy(pPluginData->szActiveFileMask, pPluginShare->szActiveFileMask);
        strcpy(pPluginData->szInactiveFileMask, pPluginShare->szInactiveFileMask);
        pPluginData->bActiveEnabled = pPluginShare->bActiveEnabled;
        pPluginData->bInactiveEnabled = pPluginShare->bInactiveEnabled;

        pPluginData->sActiveTopOffset = pPluginShare->sActiveTopOffset;
        pPluginData->sActiveBottomOffset = pPluginShare->sActiveBottomOffset;
        pPluginData->sActiveLeftOffset = pPluginShare->sActiveLeftOffset;
        pPluginData->sActiveRightOffset = pPluginShare->sActiveRightOffset;

        pPluginData->sInactiveTopOffset = pPluginShare->sInactiveTopOffset;
        pPluginData->sInactiveBottomOffset = pPluginShare->sInactiveBottomOffset;
        pPluginData->sInactiveLeftOffset = pPluginShare->sInactiveLeftOffset;
        pPluginData->sInactiveRightOffset = pPluginShare->sInactiveRightOffset;

    }                           //end else

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

    strcpy(pPluginShare->szActiveFileMask, "");
    strcpy(pPluginShare->szInactiveFileMask, "");
    //loop through each key/value pair and set appropriate parameters
    for (i = 0; i < count; i++)
    {
        if (pPluginShare->bActiveEnabled)
        {
         
            if (strcmp(kv[i].key, "ActiveImageFile") == 0)
            {
                strcpy(pPluginShare->szActiveFile, kv[i].value);
            }
            
            else if (strcmp(kv[i].key, "ActiveTopOffset") == 0)
            {
                pPluginShare->sActiveTopOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveBottomOffset") == 0)
            {
                pPluginShare->sActiveBottomOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveLeftOffset") == 0)
            {
                pPluginShare->sActiveLeftOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveRightOffset") == 0)
            {
                pPluginShare->sActiveRightOffset = (short)StringToLong(kv[i].value);
            }
            else 
              if  (strcmp(kv[i].key, "ActiveImageFileMask") == 0) {
                strcpy(pPluginShare->szActiveFileMask, kv[i].value);
              }
            
            

        }                       //end bInactive if condition

        if (pPluginShare->bInactiveEnabled)
        {

            if (strcmp(kv[i].key, "InactiveImageFile") == 0)
            {
                strcpy(pPluginShare->szInactiveFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveTopOffset") == 0)
            {
                pPluginShare->sInactiveTopOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveBottomOffset") == 0)
            {
                pPluginShare->sInactiveBottomOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveLeftOffset") == 0)
            {
                pPluginShare->sInactiveLeftOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveRightOffset") == 0)
            {
                pPluginShare->sInactiveRightOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveImageFileMask") == 0)
              {
                strcpy(pPluginShare->szInactiveFileMask, kv[i].value);
              }



        }                       //end bInactive if condition



    }                           //end for loop

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
        strcat(szSaveName, "_SZEIMAGEPlugin");
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
        strcat(szSaveName, "_SZEIMAGEPlugin");

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

    /* Free bitmaps */
    if (pPluginData->hbmActive != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmActive);
    if (pPluginData->hbmInactive != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmInactive);
    /* Free mask bitmaps */
    if (pPluginData->hbmActiveMask != NULLHANDLE)
      GpiDeleteBitmap(pPluginData->hbmActiveMask);
    if (pPluginData->hbmInactiveMask != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmInactiveMask);

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

    if (( (sActive == 1) && !(pPluginData->bActiveEnabled)) ||
            ( (sActive == 0) && !(pPluginData->bInactiveEnabled)))
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

    //make sure no error occured.
    if ( (sActive == 1) && (pPluginData->hbmActive != NULLHANDLE) )
    {
        DrawFile(pPaint, pPluginData->hbmActive, pPluginData->hbmActiveMask, pPluginData->sActiveTopOffset,
                    pPluginData->sActiveBottomOffset, pPluginData->sActiveLeftOffset,
                    pPluginData->sActiveRightOffset);
    }
    else if ( (sActive == 0) && (pPluginData->hbmInactive != NULLHANDLE) )
    {
        DrawFile(pPaint, pPluginData->hbmInactive, pPluginData->hbmInactiveMask, pPluginData->sInactiveTopOffset,
                    pPluginData->sInactiveBottomOffset, pPluginData->sInactiveLeftOffset,
                    pPluginData->sInactiveRightOffset);
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
    return FALSE; //false indicates that we haven't fully processed the message!
}

//A Dlg procedure if the plugin has selectable settings.
MRESULT EXPENTRY CBZPluginSetupDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    short sControlId;
    PLUGINSHARE *pPluginData;
    FILEDLG filedlg;
    HWND hwndDlg;

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
            WinSendMsg(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);

            WinSetWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), pPluginData->szActiveFile);
            WinSetWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), pPluginData->szActiveFileMask);
            WinSetWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), pPluginData->szInactiveFile);
            WinSetWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), pPluginData->szInactiveFileMask);

            if (pPluginData->bActiveEnabled)
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGEMASK), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVETOP), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVEBOTTOM), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVELEFT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVERIGHT), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGEMASK), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVETOP), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVEBOTTOM), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVELEFT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVERIGHT), FALSE);
            }
            if (pPluginData->bInactiveEnabled)
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGEMASK), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVETOP), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVEBOTTOM), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVELEFT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVERIGHT), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGEMASK), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVETOP), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVEBOTTOM), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVELEFT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVERIGHT), FALSE);

            }

            WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVETOP), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVETOP), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sActiveTopOffset,
                       (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVEBOTTOM), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVEBOTTOM), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sActiveBottomOffset,
                       (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVELEFT), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVELEFT), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sActiveLeftOffset,
                       (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVERIGHT), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVERIGHT), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sActiveRightOffset,
                       (MPARAM)NULL);

            WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVETOP), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVETOP), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sInactiveTopOffset,
                       (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVEBOTTOM), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVEBOTTOM), SPBM_SETCURRENTVALUE,
                       (MPARAM)pPluginData->sInactiveBottomOffset, (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVELEFT), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVELEFT), SPBM_SETCURRENTVALUE,
                       (MPARAM)pPluginData->sInactiveLeftOffset, (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVERIGHT), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVERIGHT), SPBM_SETCURRENTVALUE,
                       (MPARAM)pPluginData->sInactiveRightOffset, (MPARAM)NULL);

        }
        break;

    case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
            case PBID_CHANGEACTIVEIMAGE:
            case PBID_CHANGEINACTIVEIMAGE:
            case PBID_CHANGEACTIVEIMAGEMASK:
            case PBID_CHANGEINACTIVEIMAGEMASK:
                {
                    //popup file change dialog!
                    memset(&filedlg, 0, sizeof(FILEDLG));  // init filedlg struct

                    filedlg.cbSize = sizeof(FILEDLG);
                    filedlg.fl = FDS_OPEN_DIALOG;
                    filedlg.pszTitle = "Choose an image";
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

                }
                break;


            case PBID_OK:
                {
                    if ((pPluginData = (PLUGINSHARE *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        //error message here.
                        break;
                    }
                    /* Save filenames */
                        WinQueryWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), CCHMAXPATH, pPluginData->szActiveFile);
                        WinQueryWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), CCHMAXPATH, pPluginData->szActiveFileMask);
                        WinQueryWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), CCHMAXPATH, pPluginData->szInactiveFile);
                        WinQueryWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), CCHMAXPATH,
                                           pPluginData->szInactiveFileMask);
                        /* Save offsets */
                        WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVETOP), SPBM_QUERYVALUE,
                                   MPFROMP(&(pPluginData->sActiveTopOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                        WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVEBOTTOM), SPBM_QUERYVALUE,
                                   MPFROMP(&(pPluginData->sActiveBottomOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                        WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVELEFT), SPBM_QUERYVALUE,
                                   MPFROMP(&(pPluginData->sActiveLeftOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                        WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVERIGHT), SPBM_QUERYVALUE,
                                   MPFROMP(&(pPluginData->sActiveRightOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));

                        WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVETOP), SPBM_QUERYVALUE,
                                   MPFROMP(&(pPluginData->sInactiveTopOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                        WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVEBOTTOM), SPBM_QUERYVALUE,
                                   MPFROMP(&(pPluginData->sInactiveBottomOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                        WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVELEFT), SPBM_QUERYVALUE,
                                   MPFROMP(&(pPluginData->sInactiveLeftOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                        WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVERIGHT), SPBM_QUERYVALUE,
                                   MPFROMP(&(pPluginData->sInactiveRightOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));

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
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGEMASK), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVETOP), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVEBOTTOM), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVELEFT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVERIGHT), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGEMASK), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGEMASK), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVETOP), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVEBOTTOM), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVELEFT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVERIGHT), FALSE);
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
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGEMASK), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVETOP), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVEBOTTOM), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVELEFT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVERIGHT), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGEMASK), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGEMASK), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVETOP), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVEBOTTOM), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVELEFT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVERIGHT), FALSE);
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
        AddLongAttribute(retStr, "ActiveTopOffset", (long)(pPluginData->sActiveTopOffset));
        AddLongAttribute(retStr, "ActiveBottomOffset", (long)(pPluginData->sActiveBottomOffset));
        AddLongAttribute(retStr, "ActiveLeftOffset", (long)(pPluginData->sActiveLeftOffset));
        AddLongAttribute(retStr, "ActiveRightOffset", (long)(pPluginData->sActiveRightOffset));
    }

    if (pPluginData->bInactiveEnabled)
    {
        AddStringAttribute(retStr, "InactiveImageFile", pPluginData->szInactiveFile);
        AddStringAttribute(retStr, "InactiveImageFileMask", pPluginData->szInactiveFileMask);
        AddLongAttribute(retStr, "InactiveTopOffset", (long)(pPluginData->sInactiveTopOffset));
        AddLongAttribute(retStr, "InactiveBottomOffset", (long)(pPluginData->sInactiveBottomOffset));
        AddLongAttribute(retStr, "InactiveLeftOffset", (long)(pPluginData->sInactiveLeftOffset));
        AddLongAttribute(retStr, "InactiveRightOffset", (long)(pPluginData->sInactiveRightOffset));
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

    GpiDeleteBitmap(pFile->hbm);

    // free everything up
    GpiSetBitmap(pFile->hps, NULLHANDLE);
    GpiDestroyPS(pFile->hps);
    DevCloseDC(pFile->hdc);
    DosFreeMem(pFile->pvImage);
    DosFreeMem(pFile);

    return (hbm);
}

// draws an image
BOOL DrawFile(PSPAINT * pPaint, HBITMAP hbm,  HBITMAP hbmMask, short topOffset, short bottomOffset, short leftOffset, short rightOffset)
{
    SIZEL slHps;
    BITMAPINFOHEADER2 bif2;
    BITMAPINFOHEADER2 bif;
    POINTL aptl[3];
    POINTL aptl3[9][4];

    HDC hdc;
    HPS hps;
    HAB hab = WinQueryAnchorBlock(pPaint->hwnd);
    long starty;
    long lRop;

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

//lower left corner
    aptl3[0][0].x = pPaint->rectlWindow.xLeft;
    aptl3[0][0].y = pPaint->rectlWindow.yBottom;
    aptl3[0][1].x = aptl3[0][0].x + leftOffset;
    aptl3[0][1].y = aptl3[0][0].y + bottomOffset;
    aptl3[0][2].x = 0;
    aptl3[0][2].y = 0;
    aptl3[0][3].x = leftOffset;
    aptl3[0][3].y = bottomOffset;

//upper left corner
    aptl3[1][0].x = pPaint->rectlWindow.xLeft;
    aptl3[1][0].y = pPaint->rectlWindow.yTop - topOffset;
    aptl3[1][1].x = pPaint->rectlWindow.xLeft + leftOffset;
    aptl3[1][1].y = pPaint->rectlWindow.yTop;
    aptl3[1][2].x = 0;
    aptl3[1][2].y = bif2.cy - topOffset;
    aptl3[1][3].x = leftOffset;
    aptl3[1][3].y = bif2.cy;

//lower right corner
    aptl3[2][0].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl3[2][0].y = pPaint->rectlWindow.yBottom;
    aptl3[2][1].x = pPaint->rectlWindow.xRight;
    aptl3[2][1].y = pPaint->rectlWindow.yBottom + bottomOffset;
    aptl3[2][2].x = bif2.cx - rightOffset;
    aptl3[2][2].y = 0;
    aptl3[2][3].x = bif2.cx;
    aptl3[2][3].y = bottomOffset;

//upper right corner
    aptl3[3][0].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl3[3][0].y = pPaint->rectlWindow.yTop - topOffset;
    aptl3[3][1].x = pPaint->rectlWindow.xRight;
    aptl3[3][1].y = pPaint->rectlWindow.yTop;
    aptl3[3][2].x = bif2.cx  - rightOffset;
    aptl3[3][2].y = bif2.cy - topOffset;
    aptl3[3][3].x = bif2.cx;
    aptl3[3][3].y = bif2.cy;

//fill in and stretch the rest!
    aptl3[4][0].x = pPaint->rectlWindow.xLeft;
    aptl3[4][0].y = pPaint->rectlWindow.yBottom + bottomOffset;
    aptl3[4][1].x = pPaint->rectlWindow.xLeft + leftOffset;
    aptl3[4][1].y = pPaint->rectlWindow.yTop - topOffset;
    aptl3[4][2].x = 0;
    aptl3[4][2].y = bottomOffset;
    aptl3[4][3].x = leftOffset;
    aptl3[4][3].y = bif2.cy - topOffset;

    aptl3[5][0].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl3[5][0].y = pPaint->rectlWindow.yBottom + bottomOffset;
    aptl3[5][1].x = pPaint->rectlWindow.xRight;
    aptl3[5][1].y = pPaint->rectlWindow.yTop - topOffset;
    aptl3[5][2].x = bif2.cx - rightOffset;
    aptl3[5][2].y = bottomOffset;
    aptl3[5][3].x = bif2.cx;
    aptl3[5][3].y = bif2.cy - topOffset;

    aptl3[6][0].x = pPaint->rectlWindow.xLeft + leftOffset;
    aptl3[6][0].y = pPaint->rectlWindow.yTop - topOffset;
    aptl3[6][1].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl3[6][1].y = pPaint->rectlWindow.yTop;
    aptl3[6][2].x = leftOffset;
    aptl3[6][2].y = bif2.cy - topOffset;
    aptl3[6][3].x = bif2.cx - rightOffset;
    aptl3[6][3].y = bif2.cy;

    aptl3[7][0].x = pPaint->rectlWindow.xLeft + leftOffset;
    aptl3[7][0].y = pPaint->rectlWindow.yBottom;
    aptl3[7][1].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl3[7][1].y = pPaint->rectlWindow.yBottom + bottomOffset;
    aptl3[7][2].x = leftOffset;
    aptl3[7][2].y = 0;
    aptl3[7][3].x = bif2.cx - rightOffset;
    aptl3[7][3].y = bottomOffset;

    aptl3[8][0].x = pPaint->rectlWindow.xLeft + leftOffset;
    aptl3[8][0].y = pPaint->rectlWindow.yBottom + bottomOffset;
    aptl3[8][1].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl3[8][1].y = pPaint->rectlWindow.yTop - topOffset;
    aptl3[8][2].x = leftOffset;
    aptl3[8][2].y = bottomOffset;
    aptl3[8][3].x = bif2.cx - rightOffset;
    aptl3[8][3].y = bif2.cy - topOffset;

    /* Now blit it! */
    lRop=ROP_SRCCOPY;

    if(hbmMask) {
      // set mask bmp into hps
      GpiSetBitmap(hps, hbmMask);
      // blit mask
      //lower left corner
      GpiBitBlt(pPaint->hps, hps, 4L, aptl3[0], ROP_MERGEPAINT, BBO_IGNORE);
      //upper left corner
      GpiBitBlt(pPaint->hps, hps, 4L, aptl3[1], ROP_MERGEPAINT, BBO_IGNORE);
      //lower right corner
      GpiBitBlt(pPaint->hps, hps, 4L, aptl3[2], ROP_MERGEPAINT, BBO_IGNORE);
      //upper right corner
      GpiBitBlt(pPaint->hps, hps, 4L, aptl3[3], ROP_MERGEPAINT, BBO_IGNORE);
      //fill in and stretch the rest!
      GpiBitBlt(pPaint->hps, hps, 4L, aptl3[4], ROP_MERGEPAINT, BBO_IGNORE);
      GpiBitBlt(pPaint->hps, hps, 4L, aptl3[5], ROP_MERGEPAINT, BBO_IGNORE);
      GpiBitBlt(pPaint->hps, hps, 4L, aptl3[6], ROP_MERGEPAINT, BBO_IGNORE);
      GpiBitBlt(pPaint->hps, hps, 4L, aptl3[7], ROP_MERGEPAINT, BBO_IGNORE);
      GpiBitBlt(pPaint->hps, hps, 4L, aptl3[8], ROP_MERGEPAINT, BBO_IGNORE);
      
      lRop=ROP_SRCAND;
    }

    // set bmp into hps
    GpiSetBitmap(hps, hbm);
    // blit image
    //lower left corner
    GpiBitBlt(pPaint->hps, hps, 4L, aptl3[0], lRop, BBO_IGNORE);
    //upper left corner
    GpiBitBlt(pPaint->hps, hps, 4L, aptl3[1], lRop, BBO_IGNORE);
    //lower right corner
    GpiBitBlt(pPaint->hps, hps, 4L, aptl3[2], lRop, BBO_IGNORE);
    //upper right corner
    GpiBitBlt(pPaint->hps, hps, 4L, aptl3[3], lRop, BBO_IGNORE);
    //fill in and stretch the rest!
    GpiBitBlt(pPaint->hps, hps, 4L, aptl3[4], lRop, BBO_IGNORE);
    GpiBitBlt(pPaint->hps, hps, 4L, aptl3[5], lRop, BBO_IGNORE);
    GpiBitBlt(pPaint->hps, hps, 4L, aptl3[6], lRop, BBO_IGNORE);
    GpiBitBlt(pPaint->hps, hps, 4L, aptl3[7], lRop, BBO_IGNORE);
    GpiBitBlt(pPaint->hps, hps, 4L, aptl3[8], lRop, BBO_IGNORE);
    

#if 0

//lower left corner
    aptl2[0].x = pPaint->rectlWindow.xLeft;
    aptl2[0].y = pPaint->rectlWindow.yBottom;
    aptl2[1].x = aptl2[0].x + leftOffset;
    aptl2[1].y = aptl2[0].y + bottomOffset;
    aptl2[2].x = 0;
    aptl2[2].y = 0;
    aptl2[3].x = leftOffset;
    aptl2[3].y = bottomOffset;

    GpiBitBlt(pPaint->hps, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

//upper left corner
    aptl2[0].x = pPaint->rectlWindow.xLeft;
    aptl2[0].y = pPaint->rectlWindow.yTop - topOffset;
    aptl2[1].x = pPaint->rectlWindow.xLeft + leftOffset;
    aptl2[1].y = pPaint->rectlWindow.yTop;
    aptl2[2].x = 0;
    aptl2[2].y = bif2.cy - topOffset;
    aptl2[3].x = leftOffset;
    aptl2[3].y = bif2.cy;

    GpiBitBlt(pPaint->hps, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

//lower right corner
    aptl2[0].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl2[0].y = pPaint->rectlWindow.yBottom;
    aptl2[1].x = pPaint->rectlWindow.xRight;
    aptl2[1].y = pPaint->rectlWindow.yBottom + bottomOffset;
    aptl2[2].x = bif2.cx - rightOffset;
    aptl2[2].y = 0;
    aptl2[3].x = bif2.cx;
    aptl2[3].y = bottomOffset;

    GpiBitBlt(pPaint->hps, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

//upper right corner
    aptl2[0].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl2[0].y = pPaint->rectlWindow.yTop - topOffset;
    aptl2[1].x = pPaint->rectlWindow.xRight;
    aptl2[1].y = pPaint->rectlWindow.yTop;
    aptl2[2].x = bif2.cx  - rightOffset;
    aptl2[2].y = bif2.cy - topOffset;
    aptl2[3].x = bif2.cx;
    aptl2[3].y = bif2.cy;

    GpiBitBlt(pPaint->hps, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);


//fill in and stretch the rest!
    aptl2[0].x = pPaint->rectlWindow.xLeft;
    aptl2[0].y = pPaint->rectlWindow.yBottom + bottomOffset;
    aptl2[1].x = pPaint->rectlWindow.xLeft + leftOffset;
    aptl2[1].y = pPaint->rectlWindow.yTop - topOffset;
    aptl2[2].x = 0;
    aptl2[2].y = bottomOffset;
    aptl2[3].x = leftOffset;
    aptl2[3].y = bif2.cy - topOffset;

    GpiBitBlt(pPaint->hps, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

    aptl2[0].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl2[0].y = pPaint->rectlWindow.yBottom + bottomOffset;
    aptl2[1].x = pPaint->rectlWindow.xRight;
    aptl2[1].y = pPaint->rectlWindow.yTop - topOffset;
    aptl2[2].x = bif2.cx - rightOffset;
    aptl2[2].y = bottomOffset;
    aptl2[3].x = bif2.cx;
    aptl2[3].y = bif2.cy - topOffset;

    GpiBitBlt(pPaint->hps, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

    aptl2[0].x = pPaint->rectlWindow.xLeft + leftOffset;
    aptl2[0].y = pPaint->rectlWindow.yTop - topOffset;
    aptl2[1].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl2[1].y = pPaint->rectlWindow.yTop;
    aptl2[2].x = leftOffset;
    aptl2[2].y = bif2.cy - topOffset;
    aptl2[3].x = bif2.cx - rightOffset;
    aptl2[3].y = bif2.cy;

    GpiBitBlt(pPaint->hps, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

    aptl2[0].x = pPaint->rectlWindow.xLeft + leftOffset;
    aptl2[0].y = pPaint->rectlWindow.yBottom;
    aptl2[1].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl2[1].y = pPaint->rectlWindow.yBottom + bottomOffset;
    aptl2[2].x = leftOffset;
    aptl2[2].y = 0;
    aptl2[3].x = bif2.cx - rightOffset;
    aptl2[3].y = bottomOffset;

    GpiBitBlt(pPaint->hps, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

    aptl2[0].x = pPaint->rectlWindow.xLeft + leftOffset;
    aptl2[0].y = pPaint->rectlWindow.yBottom + bottomOffset;
    aptl2[1].x = pPaint->rectlWindow.xRight - rightOffset;
    aptl2[1].y = pPaint->rectlWindow.yTop - topOffset;
    aptl2[2].x = leftOffset;
    aptl2[2].y = bottomOffset;
    aptl2[3].x = bif2.cx - rightOffset;
    aptl2[3].y = bif2.cy - topOffset;

    GpiBitBlt(pPaint->hps, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);
#endif

    // clean up
    GpiSetBitmap(hps, NULLHANDLE);
    GpiDestroyPS(hps);
    DevCloseDC(hdc);

    return (TRUE);
}






