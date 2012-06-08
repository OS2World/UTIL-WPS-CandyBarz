/* $Id: plugin.c,v 1.2 1999/12/29 19:38:11 enorman Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "plugin.h"

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pInfo)
{
    strcpy(pInfo->PluginName, "CandyBarZ Border Images");
    strcpy(pInfo->PluginAuthor, "CandyBarZ Team @ OS/2 Netlabs");
    strcpy(pInfo->PluginDate, "11 March, 1999");
    pInfo->lWindowTypes = CBZ_FRAMEBRDR;
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
        strcat(szReadName, "_BRDRIMGPlugin");
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
                strcpy(pPluginData->szLeftFile, "NONE");
                strcpy(pPluginData->szRightFile, "NONE");
                strcpy(pPluginData->szTopFile, "NONE");
                strcpy(pPluginData->szBottomFile, "NONE");
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
            strcat(szReadName, "_BRDRIMGPlugin");
            if (!PrfQueryProfileData(hIni,
                                     "UserOptionsData",
                                     szReadName,
                                     pPluginShare,
                                     &szData))
            {
                strcpy(pPluginShare->szLeftFile, "NONE");
                strcpy(pPluginShare->szRightFile, "NONE");
                strcpy(pPluginShare->szTopFile, "NONE");
                strcpy(pPluginShare->szBottomFile, "NONE");
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
        strcpy(pPluginData->szLeftFile, pPluginShare->szLeftFile);
        strcpy(pPluginData->szRightFile, pPluginShare->szRightFile);
        strcpy(pPluginData->szTopFile, pPluginShare->szTopFile);
        strcpy(pPluginData->szBottomFile, pPluginShare->szBottomFile);
        pPluginData->bActiveEnabled = pPluginShare->bActiveEnabled;
        pPluginData->bInactiveEnabled = pPluginShare->bInactiveEnabled;


        pPluginData->sLeftTopOffset = pPluginShare->sLeftTopOffset;
        pPluginData->sLeftBottomOffset = pPluginShare->sLeftBottomOffset;
        pPluginData->sRightTopOffset = pPluginShare->sRightTopOffset;
        pPluginData->sRightBottomOffset = pPluginShare->sRightBottomOffset;
        pPluginData->sTopLeftOffset = pPluginShare->sTopLeftOffset;
        pPluginData->sTopRightOffset = pPluginShare->sTopRightOffset;
        pPluginData->sBottomLeftOffset = pPluginShare->sBottomLeftOffset;
        pPluginData->sBottomRightOffset = pPluginShare->sBottomRightOffset;

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

    //loop through each key/value pair and set appropriate parameters
    for (i = 0; i < count; i++)
    {
//        if (pPluginShare->bActiveEnabled)
        {
            if (strcmp(kv[i].key, "LeftImageFile") == 0)
            {
                strcpy(pPluginShare->szLeftFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "LeftTopOffset") == 0)
            {
                pPluginShare->sLeftTopOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "LeftBottomOffset") == 0)
            {
                pPluginShare->sLeftBottomOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "RightImageFile") == 0)
            {
                strcpy(pPluginShare->szRightFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "RightTopOffset") == 0)
            {
                pPluginShare->sRightTopOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "RightBottomOffset") == 0)
            {
                pPluginShare->sRightBottomOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "TopImageFile") == 0)
            {
                strcpy(pPluginShare->szTopFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "TopLeftOffset") == 0)
            {
                pPluginShare->sTopLeftOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "TopRightOffset") == 0)
            {
                pPluginShare->sTopRightOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "BottomImageFile") == 0)
            {
                strcpy(pPluginShare->szBottomFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "BottomLeftOffset") == 0)
            {
                pPluginShare->sBottomLeftOffset = (short)StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "BottomRightOffset") == 0)
            {
                pPluginShare->sBottomRightOffset = (short)StringToLong(kv[i].value);
            }



        }                       //end bInactive if condition
/*
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
        }                       //end bInactive if condition

*/

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
        strcat(szSaveName, "_BRDRIMGPlugin");
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
        strcat(szSaveName, "_BRDRIMGPlugin");

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

    if (pPluginData->hbmLeft != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmLeft);
    if (pPluginData->hbmRight != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmRight);
    if (pPluginData->hbmTop != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmTop);
    if (pPluginData->hbmBottom != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmBottom);

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
/*
    if (( (sActive == 1) && !(pPluginData->bActiveEnabled)) ||
            ( (sActive == 0) && !(pPluginData->bInactiveEnabled)))
        return (TRUE);
*/
    if ( pPluginData->hbmLeft == NULLHANDLE )
        pPluginData->hbmLeft = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szLeftFile, pPaint->rectlWindow.yTop);
    if ( pPluginData->hbmRight == NULLHANDLE )
        pPluginData->hbmRight = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szRightFile, pPaint->rectlWindow.yTop);
    if ( pPluginData->hbmTop == NULLHANDLE )
        pPluginData->hbmTop = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szTopFile, pPaint->rectlWindow.yTop);
    if ( pPluginData->hbmBottom == NULLHANDLE )
        pPluginData->hbmBottom = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szBottomFile, pPaint->rectlWindow.yTop);


    //make sure no error occured.
    if ( pPluginData->hbmLeft != NULLHANDLE )
    {
        DrawFile(pPaint, pPluginData->hbmLeft, LEFT, 0,0,pPluginData->sLeftTopOffset,
                    pPluginData->sLeftBottomOffset, sActive );
    }
    if ( pPluginData->hbmRight != NULLHANDLE )
    {
        DrawFile(pPaint, pPluginData->hbmRight, RIGHT, 0,0,pPluginData->sRightTopOffset,
                    pPluginData->sRightBottomOffset, sActive );
    }
    if ( pPluginData->hbmTop != NULLHANDLE )
    {
        DrawFile(pPaint, pPluginData->hbmTop, TOP, pPluginData->sTopLeftOffset,
                    pPluginData->sTopRightOffset, 0,0, sActive );
    }
    if ( pPluginData->hbmBottom != NULLHANDLE )
    {
        DrawFile(pPaint, pPluginData->hbmBottom, BOTTOM, pPluginData->sBottomLeftOffset,
                    pPluginData->sBottomRightOffset,0,0, sActive );
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
    return FALSE; //return FALSE indicates we haven't processed the message!
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

            WinSendMsg(WinWindowFromID(hwnd, EFID_LEFTIMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, EFID_RIGHTIMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, EFID_TOPIMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, EFID_BOTTOMIMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);

            WinSetWindowText(WinWindowFromID(hwnd, EFID_LEFTIMAGE), pPluginData->szLeftFile);
            WinSetWindowText(WinWindowFromID(hwnd, EFID_RIGHTIMAGE), pPluginData->szRightFile);
            WinSetWindowText(WinWindowFromID(hwnd, EFID_TOPIMAGE), pPluginData->szTopFile);
            WinSetWindowText(WinWindowFromID(hwnd, EFID_BOTTOMIMAGE), pPluginData->szBottomFile);

            if (pPluginData->bActiveEnabled)
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
            else
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
            if (pPluginData->bInactiveEnabled)
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, TRUE);
            else
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);

            WinSendMsg(WinWindowFromID(hwnd, SPBID_LEFTTOP), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_LEFTTOP), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sLeftTopOffset, (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_LEFTBOTTOM), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_LEFTBOTTOM), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sLeftBottomOffset, (MPARAM)NULL);

            WinSendMsg(WinWindowFromID(hwnd, SPBID_RIGHTTOP), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_RIGHTTOP), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sRightTopOffset, (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_RIGHTBOTTOM), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_RIGHTBOTTOM), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sRightBottomOffset, (MPARAM)NULL);

            WinSendMsg(WinWindowFromID(hwnd, SPBID_TOPLEFT), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_TOPLEFT), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sTopLeftOffset, (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_TOPRIGHT), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_TOPRIGHT), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sTopRightOffset, (MPARAM)NULL);

            WinSendMsg(WinWindowFromID(hwnd, SPBID_BOTTOMLEFT), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_BOTTOMLEFT), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sBottomLeftOffset, (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_BOTTOMRIGHT), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_BOTTOMRIGHT), SPBM_SETCURRENTVALUE, (MPARAM)pPluginData->sBottomRightOffset, (MPARAM)NULL);
        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
                case PBID_LEFTCHANGEIMAGE:
                case PBID_RIGHTCHANGEIMAGE:
                case PBID_TOPCHANGEIMAGE:
                case PBID_BOTTOMCHANGEIMAGE:
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

                    if (sControlId == PBID_LEFTCHANGEIMAGE)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, EFID_LEFTIMAGE), filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_RIGHTCHANGEIMAGE)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, EFID_RIGHTIMAGE), filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_TOPCHANGEIMAGE)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, EFID_TOPIMAGE), filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_BOTTOMCHANGEIMAGE)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, EFID_BOTTOMIMAGE), filedlg.szFullFile);
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

                    WinQueryWindowText(WinWindowFromID(hwnd, EFID_LEFTIMAGE), CCHMAXPATH, pPluginData->szLeftFile);
                    WinQueryWindowText(WinWindowFromID(hwnd, EFID_RIGHTIMAGE), CCHMAXPATH, pPluginData->szRightFile);
                    WinQueryWindowText(WinWindowFromID(hwnd, EFID_TOPIMAGE), CCHMAXPATH, pPluginData->szTopFile);
                    WinQueryWindowText(WinWindowFromID(hwnd, EFID_BOTTOMIMAGE), CCHMAXPATH, pPluginData->szBottomFile);

                    WinSendMsg(WinWindowFromID(hwnd, SPBID_LEFTTOP), SPBM_QUERYVALUE, MPFROMP(&(pPluginData->sLeftTopOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                    WinSendMsg(WinWindowFromID(hwnd, SPBID_LEFTBOTTOM), SPBM_QUERYVALUE, MPFROMP(&(pPluginData->sLeftBottomOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                    WinSendMsg(WinWindowFromID(hwnd, SPBID_RIGHTTOP), SPBM_QUERYVALUE, MPFROMP(&(pPluginData->sRightTopOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                    WinSendMsg(WinWindowFromID(hwnd, SPBID_RIGHTBOTTOM), SPBM_QUERYVALUE, MPFROMP(&(pPluginData->sRightBottomOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));

                    WinSendMsg(WinWindowFromID(hwnd, SPBID_TOPLEFT), SPBM_QUERYVALUE, MPFROMP(&(pPluginData->sTopLeftOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                    WinSendMsg(WinWindowFromID(hwnd, SPBID_TOPRIGHT), SPBM_QUERYVALUE, MPFROMP(&(pPluginData->sTopRightOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                    WinSendMsg(WinWindowFromID(hwnd, SPBID_BOTTOMLEFT), SPBM_QUERYVALUE, MPFROMP(&(pPluginData->sBottomLeftOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                    WinSendMsg(WinWindowFromID(hwnd, SPBID_BOTTOMRIGHT), SPBM_QUERYVALUE, MPFROMP(&(pPluginData->sBottomRightOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));

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
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
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
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
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

//    if (pPluginData->bActiveEnabled)
    {
        AddStringAttribute(retStr, "LeftImageFile", pPluginData->szLeftFile);
        AddLongAttribute(retStr, "LeftTopOffset", (long)(pPluginData->sLeftTopOffset));
        AddLongAttribute(retStr, "LeftBottomOffset", (long)(pPluginData->sLeftBottomOffset));

        AddStringAttribute(retStr, "RightImageFile", pPluginData->szRightFile);
        AddLongAttribute(retStr, "RightTopOffset", (long)(pPluginData->sRightTopOffset));
        AddLongAttribute(retStr, "RightBottomOffset", (long)(pPluginData->sRightBottomOffset));

        AddStringAttribute(retStr, "TopImageFile", pPluginData->szTopFile);
        AddLongAttribute(retStr, "TopLeftOffset", (long)(pPluginData->sTopLeftOffset));
        AddLongAttribute(retStr, "TopRightOffset", (long)(pPluginData->sTopRightOffset));

        AddStringAttribute(retStr, "BottomImageFile", pPluginData->szBottomFile);
        AddLongAttribute(retStr, "BottomLeftOffset", (long)(pPluginData->sBottomLeftOffset));
        AddLongAttribute(retStr, "BottomRightOffset", (long)(pPluginData->sBottomRightOffset));
    }

/*    if (pPluginData->bInactiveEnabled)
    {
    }
*/

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

// draws an image
BOOL DrawFile(PSPAINT * pPaint, HBITMAP hbm, short sSide, short leftOffset, short rightOffset, short topOffset, short bottomOffset, BOOL bActive)
{
    SIZEL slHps;
    BITMAPINFOHEADER2 bif2;
    BITMAPINFOHEADER2 bif;
    POINTL aptl[3], aptl2[4];
    HDC hdc;
    HPS hps;
    HAB hab = WinQueryAnchorBlock(pPaint->hwnd);
    long starty;
    RECTL rectlSide;
    POINTL border;

    WinSendMsg(pPaint->hwnd, WM_QUERYBORDERSIZE, MPFROMP(&border), 0);

    if ( sSide == LEFT )
    {
        rectlSide.xLeft     = pPaint->rectlWindow.xLeft;
        rectlSide.xRight    = pPaint->rectlWindow.xLeft + border.x;
        rectlSide.yBottom   = pPaint->rectlWindow.yBottom;
        rectlSide.yTop      = pPaint->rectlWindow.yTop;
    }
    else if ( sSide == RIGHT )
    {
        rectlSide.xLeft     = pPaint->rectlWindow.xRight - border.x;
        rectlSide.xRight    = pPaint->rectlWindow.xRight;
        rectlSide.yBottom   = pPaint->rectlWindow.yBottom;
        rectlSide.yTop      = pPaint->rectlWindow.yTop;
    }
    else if ( sSide == BOTTOM )
    {
        rectlSide.xLeft     = pPaint->rectlWindow.xLeft + border.x;
        rectlSide.xRight    = pPaint->rectlWindow.xRight - border.x;
        rectlSide.yBottom   = pPaint->rectlWindow.yBottom;
        rectlSide.yTop      = pPaint->rectlWindow.yBottom + border.y;
    }
    else if ( sSide == TOP )
    {
        rectlSide.xLeft     = pPaint->rectlWindow.xLeft + border.x;
        rectlSide.xRight    = pPaint->rectlWindow.xRight - border.x;
        rectlSide.yBottom   = pPaint->rectlWindow.yTop - border.y;
        rectlSide.yTop      = pPaint->rectlWindow.yTop;
    }
    else
        return FALSE;


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
    // set bmp into hps
    GpiSetBitmap(hps, hbm);



    if ( (sSide == TOP) || (sSide == BOTTOM) ) //top or bottom image
    {
        //center fill
        aptl2[0].x = rectlSide.xLeft + leftOffset;
        aptl2[0].y = rectlSide.yBottom;
        aptl2[1].x = rectlSide.xRight - rightOffset;
        aptl2[1].y = rectlSide.yTop;
        if (!bActive)
        {
            aptl2[2].x = leftOffset;
            aptl2[2].y = bif2.cy / 2;
            aptl2[3].x = bif2.cx - rightOffset;
            aptl2[3].y = bif2.cy;
        }
        else
        {
            aptl2[2].x = leftOffset;
            aptl2[2].y = 0;
            aptl2[3].x = bif2.cx - rightOffset;
            aptl2[3].y = bif2.cy / 2;
        }

        GpiBitBlt(pPaint->hpsWin, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

        //left corner
        aptl2[0].x = rectlSide.xLeft;
        aptl2[0].y = rectlSide.yBottom;
        aptl2[1].x = aptl2[0].x + leftOffset;
        aptl2[1].y = rectlSide.yTop;
        if (!bActive)
        {
            aptl2[2].x = 0;
            aptl2[2].y = bif2.cy / 2;
            aptl2[3].x = leftOffset;
            aptl2[3].y = bif2.cy;
        }
        else
        {
            aptl2[2].x = 0;
            aptl2[2].y = 0;
            aptl2[3].x = leftOffset;
            aptl2[3].y = bif2.cy / 2;
        }

        GpiBitBlt(pPaint->hpsWin, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

        //right corner
        aptl2[0].x = rectlSide.xRight - rightOffset;
        aptl2[0].y = rectlSide.yBottom;
        aptl2[1].x = rectlSide.xRight;
        aptl2[1].y = rectlSide.yTop;
        if (!bActive)
        {
            aptl2[2].x = bif2.cx - rightOffset;
            aptl2[2].y = bif2.cy / 2;
            aptl2[3].x = bif2.cx;
            aptl2[3].y = bif2.cy;
        }
        else
        {
            aptl2[2].x = bif2.cx - rightOffset;
            aptl2[2].y = 0;
            aptl2[3].x = bif2.cx;
            aptl2[3].y = bif2.cy / 2;
        }
        GpiBitBlt(pPaint->hpsWin, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);


    }
    else
    {
        //center fill
        aptl2[0].x = rectlSide.xLeft;
        aptl2[0].y = rectlSide.yBottom + bottomOffset;
        aptl2[1].x = rectlSide.xRight;
        aptl2[1].y = rectlSide.yTop - topOffset;
        if (!bActive)
        {
            aptl2[2].x = 0;
            aptl2[2].y = bottomOffset;
            aptl2[3].x = bif2.cx / 2;
            aptl2[3].y = bif2.cy - topOffset;
        }
        else
        {
            aptl2[2].x = bif2.cx / 2;
            aptl2[2].y = bottomOffset;
            aptl2[3].x = bif2.cx;
            aptl2[3].y = bif2.cy - topOffset;
        }
        GpiBitBlt(pPaint->hpsWin, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

        //bottom corner
        aptl2[0].x = rectlSide.xLeft;
        aptl2[0].y = rectlSide.yBottom;
        aptl2[1].x = rectlSide.xRight;
        aptl2[1].y = aptl2[0].y + bottomOffset;
        if (!bActive)
        {
            aptl2[2].x = 0;
            aptl2[2].y = 0;
            aptl2[3].x = bif2.cx / 2;
            aptl2[3].y = bottomOffset;
        }
        else
        {
            aptl2[2].x = bif2.cx / 2;
            aptl2[2].y = 0;
            aptl2[3].x = bif2.cx;
            aptl2[3].y = bottomOffset;
        }
        GpiBitBlt(pPaint->hpsWin, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

        //top corner
        aptl2[0].x = rectlSide.xLeft;
        aptl2[0].y = rectlSide.yTop - topOffset;
        aptl2[1].x = rectlSide.xRight;
        aptl2[1].y = rectlSide.yTop;
        if (!bActive)
        {
            aptl2[2].x = 0;
            aptl2[2].y = bif2.cy - topOffset;
            aptl2[3].x = bif2.cx / 2;
            aptl2[3].y = bif2.cy;
        }
        else
        {
            aptl2[2].x = bif2.cx / 2;
            aptl2[2].y = bif2.cy - topOffset;
            aptl2[3].x = bif2.cx;
            aptl2[3].y = bif2.cy;
        }
        GpiBitBlt(pPaint->hpsWin, hps, 4L, aptl2, ROP_SRCCOPY, BBO_IGNORE);

    }
    // clean up
    GpiSetBitmap(hps, NULLHANDLE);
    GpiDestroyPS(hps);
    DevCloseDC(hdc);

    return (TRUE);
}
