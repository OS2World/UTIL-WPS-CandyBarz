/* $Id: plugin.c,v 1.3 2000/10/04 19:46:42 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "plugin.h"

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pInfo)
{
    strcpy(pInfo->PluginName, "CandyBarZ Images");
    strcpy(pInfo->PluginAuthor, "CandyBarZ Team @ OS/2 Netlabs");
    strcpy(pInfo->PluginDate, "Sep. 03, 2000");
    pInfo->lWindowTypes = CBZ_TITLEBAR | CBZ_FRAME | CBZ_PUSHBUTTON ;
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

    //custom name was specified, try to load the custom config for it.
    if (strlen(szName) > 0)
    {
        strcpy(szReadName, szName);
        strcat(szReadName, "_");
        strcat(szReadName, szClass);
        strcat(szReadName, "_CBZIMAGEPlugin");
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
        strcpy(szShareName, PLUGIN_SHARE);
        strcat(szShareName, szClass);

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
            strcat(szReadName, "_CBZIMAGEPlugin");
            if (!PrfQueryProfileData(hIni,
                                     "UserOptionsData",
                                     szReadName,
                                     pPluginShare,
                                     &szData))
            {
                strcpy(pPluginShare->szActiveFile, "NONE");
                strcpy(pPluginShare->szInactiveFile, "NONE");
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
        pPluginData->bActiveEnabled = pPluginShare->bActiveEnabled;
        pPluginData->bInactiveEnabled = pPluginShare->bInactiveEnabled;
        pPluginData->bActiveTile = pPluginShare->bActiveTile;
        pPluginData->bInactiveTile = pPluginShare->bInactiveTile;
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
        if (pPluginShare->bActiveEnabled)
        {
            if (strcmp(kv[i].key, "ActiveImageFile") == 0)
            {
                strcpy(pPluginShare->szActiveFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveTile") == 0)
            {
                if (strcmp(kv[i].value, "TRUE") == 0)
                    pPluginShare->bActiveTile = TRUE;
                else
                    pPluginShare->bActiveTile = FALSE;
            }
        }                       //end bInactive if condition

        if (pPluginShare->bInactiveEnabled)
        {
            if (strcmp(kv[i].key, "InactiveImageFile") == 0)
            {
                strcpy(pPluginShare->szInactiveFile, kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveTile") == 0)
            {
                if (strcmp(kv[i].value, "TRUE") == 0)
                    pPluginShare->bInactiveTile = TRUE;
                else
                    pPluginShare->bInactiveTile = FALSE;
            }
        }                       //end bInactive if condition
    }//end for loop

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
        strcat(szSaveName, "_CBZIMAGEPlugin");
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
        strcat(szSaveName, "_CBZIMAGEPlugin");

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
    if (pPluginData->hbmInactive != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmInactive);

    DosFreeMem(pPluginData);
    return (TRUE);
}

//Draw the Plugin effect into the given presentation space
BOOL CBZPluginRender(PSPAINT * pPaint, PVOID pData, short sActive)
{
    BITMAPINFOHEADER2 bif2;
    PLUGINSHARE *pPluginData;
    pPluginData = (PLUGINSHARE *) pData;

    if (pPluginData == NULL)
        return (FALSE);

    if (( ((sActive == 1) || (sActive == 3))   && !(pPluginData->bActiveEnabled)) ||
            ( ((sActive == 0) || (sActive == 2)) && !(pPluginData->bInactiveEnabled)))
        return (TRUE);

    if ( (sActive == 1) && (pPluginData->hbmActive == NULLHANDLE) ) {
      pPluginData->hbmActive = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szActiveFile, pPaint->rectlWindow.yTop);

      // get bitmap info
      memset(&bif2, 0, sizeof(bif2));
      bif2.cbFix = sizeof(bif2);
      GpiQueryBitmapInfoHeader(pPluginData->hbmActive, &bif2);

      pPluginData->ulCx=bif2.cx;
      pPluginData->ulCy=bif2.cy;
    }
    else if ( (sActive == 0) && (pPluginData->hbmInactive == NULLHANDLE) ) {
      pPluginData->hbmInactive = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szInactiveFile, pPaint->rectlWindow.yTop);

      // get bitmap info
      memset(&bif2, 0, sizeof(bif2));
      bif2.cbFix = sizeof(bif2);
      GpiQueryBitmapInfoHeader(pPluginData->hbmInactive, &bif2);
      
      pPluginData->ulInactiveCx=bif2.cx;
      pPluginData->ulInactiveCy=bif2.cy;
 
    }

    //make sure no error occured.
    if ( (sActive == 1) && (pPluginData->hbmActive != NULLHANDLE) )
    {
        if ( pPluginData->bActiveTile == TRUE)
            TileFile(pPaint, pPluginData->hbmActive, pPluginData, TRUE);
        else
            WinDrawBitmap(pPaint->hps, pPluginData->hbmActive, NULL, (PPOINTL)&(pPaint->rectlWindow), 0L, 0L, DBM_STRETCH);
    }
    else if ( (sActive == 0) && (pPluginData->hbmInactive != NULLHANDLE) )
    {
        if ( pPluginData->bInactiveTile == TRUE)
            TileFile(pPaint, pPluginData->hbmInactive, pPluginData, FALSE);
        else
            WinDrawBitmap(pPaint->hps, pPluginData->hbmInactive, NULL, (PPOINTL)&(pPaint->rectlWindow), 0L, 0L, DBM_STRETCH);
    }

    return (TRUE);
}

//Called before the default wnd procedure if you wish to provide additional
//  functionality.  (i.e. modify what happens with mouse clicks, drag/drop, etc...
BOOL CBZPluginWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2,
                        PVOID pData, PFNWP pfnOrigWndProc, MRESULT *rc)
{
  char * ptr;

    switch (msg)
    {
    }
    return FALSE;  //return false indicates we haven't processed the message!
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
            WinSendMsg(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);

            WinSetWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), pPluginData->szActiveFile);

            WinSetWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), pPluginData->szInactiveFile);

            if (pPluginData->bActiveEnabled)
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), FALSE);
            }
            if (pPluginData->bInactiveEnabled)
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), FALSE);
            }

            if (pPluginData->bActiveTile)
            {
                WinCheckButton(hwnd, CBID_ACTIVETILE, TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_ACTIVETILE, FALSE);
            }

            if (pPluginData->bInactiveTile)
            {
                WinCheckButton(hwnd, CBID_INACTIVETILE, TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_INACTIVETILE, FALSE);
            }


        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
                case PBID_CHANGEACTIVEIMAGE:
                case PBID_CHANGEINACTIVEIMAGE:
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
                        WinSetWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGE),filedlg.szFullFile);
                    }
                    else if (sControlId == PBID_CHANGEINACTIVEIMAGE)
                    {
                        WinSetWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), filedlg.szFullFile);
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

                    WinQueryWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), CCHMAXPATH, pPluginData->szActiveFile);
                    WinQueryWindowText(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), CCHMAXPATH, pPluginData->szInactiveFile);

                    if ( WinQueryButtonCheckstate(hwnd, CBID_ACTIVEENABLED) )
                        pPluginData->bActiveEnabled = TRUE;
                    else
                        pPluginData->bActiveEnabled = FALSE;

                    if ( WinQueryButtonCheckstate(hwnd, CBID_INACTIVEENABLED) )
                        pPluginData->bInactiveEnabled = TRUE;
                    else
                        pPluginData->bInactiveEnabled = FALSE;


                    if ( WinQueryButtonCheckstate(hwnd, CBID_ACTIVETILE) )
                        pPluginData->bActiveTile = TRUE;
                    else
                        pPluginData->bActiveTile = FALSE;

                    if ( WinQueryButtonCheckstate(hwnd, CBID_INACTIVETILE) )
                        pPluginData->bInactiveTile = TRUE;
                    else
                        pPluginData->bInactiveTile = FALSE;

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
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVETILE), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVETILE), FALSE);
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
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVETILE), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, STID_INACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CHANGEINACTIVEIMAGE), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVETILE), FALSE);
                    }
                }
                break;

                case CBID_ACTIVETILE:
                {
                    // if Enabled
                    if (!WinQueryButtonCheckstate(hwnd, CBID_ACTIVETILE))
                    {
                        // check button
                        WinCheckButton(hwnd, CBID_ACTIVETILE, TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVETILE, FALSE);
                    }
                }
                break;

                case CBID_INACTIVETILE:
                {
                    // if Enabled
                    if (!WinQueryButtonCheckstate(hwnd, CBID_INACTIVETILE))
                    {
                        // check button
                        WinCheckButton(hwnd, CBID_INACTIVETILE, TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVETILE, FALSE);
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
        if (pPluginData->bActiveTile == TRUE)
            AddStringAttribute(retStr, "ActiveTile", "TRUE");
        else
            AddStringAttribute(retStr, "ActiveTile", "FALSE");
    }

    if (pPluginData->bInactiveEnabled)
    {
        AddStringAttribute(retStr, "InactiveImageFile", pPluginData->szInactiveFile);
        if (pPluginData->bInactiveTile == TRUE)
            AddStringAttribute(retStr, "InactiveTile", "TRUE");
        else
            AddStringAttribute(retStr, "InactiveTile", "FALSE");
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
BOOL TileFile(PSPAINT * pPaint, HBITMAP hbm, PLUGINSHARE *pPluginData, BOOL bActive)
{
    SIZEL slHps;
    BITMAPINFOHEADER2 bif;
    POINTL aptl[3];
    HDC hdc;
    HPS hps;
    HAB hab = WinQueryAnchorBlock(pPaint->hwnd);
    long starty;
    register ULONG  ulCx;
    register ULONG  ulCy;


    // setup source bitmap
    hdc = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE);
    // create source hps
    if(bActive) {
      slHps.cx = pPluginData->ulCx;// Bitmap size
      slHps.cy = pPluginData->ulCy;
    }
    else {
      slHps.cx = pPluginData->ulInactiveCx;
      slHps.cy = pPluginData->ulInactiveCy;
    }

    hps = GpiCreatePS(hab,
                      hdc,
                      &slHps,
                      PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
    // set bmp into hps
    GpiSetBitmap(hps, hbm);

    // initial blt location
    aptl[0].x = pPaint->rectlWindow.xLeft;
    aptl[0].y = pPaint->rectlWindow.yBottom;
    aptl[1].x = pPaint->rectlWindow.xRight;
    aptl[1].y = pPaint->rectlWindow.yTop;
    aptl[2].x = 0;
    aptl[2].y = 0;

    starty = aptl[0].y;
    ulCx=slHps.cx;
    ulCy=slHps.cy;
    while (aptl[0].x < aptl[1].x)
    {
        aptl[0].y = starty;

        while (aptl[0].y < aptl[1].y)
        {
            // blt
          GpiBitBlt(pPaint->hps, hps, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE);

            // move
            aptl[0].y += ulCy;
        }
        aptl[0].x += ulCx;
    }

    // clean up
    GpiSetBitmap(hps, NULLHANDLE);
    GpiDestroyPS(hps);
    DevCloseDC(hdc);

    return (TRUE);
}
