/* $Id: plugin.c,v 1.5 2000/10/04 19:58:05 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "plugin.h"

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pInfo)
{
    strcpy(pInfo->PluginName, "Min/Max FrameControl Images");
    strcpy(pInfo->PluginAuthor, "CandyBarZ Team @ OS/2 Netlabs");
    strcpy(pInfo->PluginDate, "15 Sep, 2000");
    pInfo->lWindowTypes = CBZ_MINMAX;
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
        strcat(szReadName, "_MINMAXPlugin");
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
                strcpy(pPluginData->szImageFile, "NONE");
                pPluginData->sEnabledStates = 0;

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
            strcat(szReadName, "_MINMAXPlugin");
            if (!PrfQueryProfileData(hIni,
                                     "UserOptionsData",
                                     szReadName,
                                     pPluginShare,
                                     &szData))
            {
                strcpy(pPluginShare->szImageFile, "NONE");
                pPluginShare->sEnabledStates = 0;

                if (!PrfWriteProfileData(hIni,
                                         "UserOptionsData",
                                         szReadName,
                                         pPluginShare,
                                         sizeof(PLUGINSHARE)))
                {
                    //not a fatal error.
                }
            }
        }    //end if

        //copy from shared memory to each windows window words.
        strcpy(pPluginData->szImageFile, pPluginShare->szImageFile);
        pPluginData->sEnabledStates = pPluginShare->sEnabledStates;
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

    pPluginShare->sEnabledStates = enabledState;

    //loop through each key/value pair and set appropriate parameters
    for (i = 0; i < count; i++)
    {
        if (strcmp(kv[i].key, "ImageFile") == 0)
        {
            strcpy(pPluginShare->szImageFile, kv[i].value);
        }
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
        strcat(szSaveName, "_MINMAXPlugin");
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
        strcat(szSaveName, "_MINMAXPlugin");

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

    //Restore the Original Images.
#if 0
    WinSendMsg(hwnd, MM_SETITEMHANDLE, MPFROMSHORT(SC_RESTORE), MPFROMLONG(pPluginData->hbmOldRestore) );
    WinSendMsg(hwnd, MM_SETITEMHANDLE, MPFROMSHORT(SC_MAXIMIZE), MPFROMLONG(pPluginData->hbmOldMaximize) );
    WinSendMsg(hwnd, MM_SETITEMHANDLE, MPFROMSHORT(SC_HIDE), MPFROMLONG(pPluginData->hbmOldHide) );
    WinSendMsg(hwnd, MM_SETITEMHANDLE, MPFROMSHORT(SC_MINIMIZE), MPFROMLONG(pPluginData->hbmOldMinimize) );
    WinSendMsg(hwnd, MM_SETITEMHANDLE, MPFROMSHORT(SC_CLOSE), MPFROMLONG(pPluginData->hbmOldClose) );
#endif

    if (pPluginData->hbmActiveMinimize != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmActiveMinimize);
    if (pPluginData->hbmInactiveMinimize != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmInactiveMinimize);
    if (pPluginData->hbmActiveMaximize != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmActiveMaximize);
    if (pPluginData->hbmInactiveMaximize != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmInactiveMaximize);
    if (pPluginData->hbmActiveRestore != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmActiveRestore);
    if (pPluginData->hbmInactiveRestore != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmInactiveRestore);
    if (pPluginData->hbmActiveClose != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmActiveClose);
    if (pPluginData->hbmInactiveClose != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmInactiveClose);
    if (pPluginData->hbmActiveHide != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmActiveHide);
    if (pPluginData->hbmInactiveHide != NULLHANDLE)
        GpiDeleteBitmap(pPluginData->hbmInactiveHide);

    DosFreeMem(pPluginData);

    return (TRUE);
}

//Draw the Plugin effect into the given presentation space
BOOL CBZPluginRender(PSPAINT * pPaint, PVOID pData, short sActive )
{
  
  BITMAPINFOHEADER2 bif2;  
  PLUGINSHARE *pPluginData;
  short sState=sActive;
  pPluginData = (PLUGINSHARE *) pData;


  if (pPluginData == NULL)
    return (FALSE);
    
  if ( pPluginData->hbmActiveMinimize == NULLHANDLE ) {
    if (!CreateImages(pPluginData))
      return FALSE;
  }
  
  //make sure no error occured.
  if (pPluginData->hbmActiveMinimize != NULLHANDLE)
    {
      
      /* SC_CLOSE_ menu */
      if (sState == 0)
        {
          DrawImage(pPaint, pPluginData->hbmActiveClose, sActive, pPluginData);
        }
      else if (sState == 1)
        {
          DrawImage(pPaint, pPluginData->hbmInactiveClose, sActive, pPluginData);
        }
      else if (sState == 2)
        {
          DrawImage(pPaint, pPluginData->hbmActiveMinimize, sActive, pPluginData);
        }
      else if (sState == 3)
        {
          DrawImage(pPaint, pPluginData->hbmInactiveMinimize, sActive, pPluginData);
        }
      else if (sState == 4)
        {
          DrawImage(pPaint, pPluginData->hbmActiveMaximize, sActive, pPluginData);
        }
      else if (sState == 5)
        {
          DrawImage(pPaint, pPluginData->hbmInactiveMaximize, sActive, pPluginData);
        }
      else if (sState == 6)
        {
          DrawImage(pPaint, pPluginData->hbmActiveRestore, sActive, pPluginData);
        }
      else if (sState == 7)
        {
          DrawImage(pPaint, pPluginData->hbmInactiveRestore, sActive, pPluginData);
        }
      else if (sState == 8)
        {
          DrawImage(pPaint, pPluginData->hbmActiveHide, sActive, pPluginData);
        }
      else if (sState == 9)
        {
          DrawImage(pPaint, pPluginData->hbmInactiveHide, sActive, pPluginData);
        }
      else
        {
          return FALSE;
        }
    }


    return (TRUE);
}

//Called before the default wnd procedure if you wish to provide additional
//  functionality.  (i.e. modify what happens with mouse clicks, drag/drop, etc...
BOOL CBZPluginWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2,
                        PVOID pData, PFNWP pfnOrigWndProc, MRESULT *rc)
{
  USHORT      mId;
  PLUGINSHARE *pPluginData = (PLUGINSHARE *)pData;
  


  switch (msg)
    {
    case WM_PAINT:
        {
          PLUGINSHARE *pCBZData = (PLUGINSHARE *)pData;

            BOOL    bActive = FALSE;
            PSPAINT psP;
 
            mId = WinQueryWindowUShort(hwnd, QWS_ID);

            //paint the MinMax menu!
            if (mId == FID_MINMAX)
            {
              HPS hps;
              RECTL rcl;
         
              hps = WinBeginPaint(hwnd, NULLHANDLE,&rcl);


              if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_CLOSE,FALSE),&rcl))
                {
                  if(pPluginData->hbmActiveClose==NULLHANDLE)
                    if (!CreateImages(pPluginData))
                      return FALSE;
                  // rcl.yBottom-=1;    
                  memset(&psP,0,sizeof(psP));
                  psP.hwnd=hwnd;
                  psP.hps=hps;
                  psP.hpsWin=hps;
                  psP.rectlWindow=rcl;
                  psP.rectlUpdate=rcl;
                  
                  DrawImage(&psP, pPluginData->hbmActiveClose, 0, pPluginData);              
                }

                if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_MINIMIZE,FALSE),&rcl))
                {
                    //draw MinimizeButton

                  if(pPluginData->hbmActiveMinimize==NULLHANDLE)
                    if (!CreateImages(pPluginData))
                      return FALSE;
                  memset(&psP,0,sizeof(psP));
                  psP.hwnd=hwnd;
                  psP.hps=hps;
                  psP.hpsWin=hps;
                  psP.rectlWindow=rcl;
                  psP.rectlUpdate=rcl;
                  
                  DrawImage(&psP, pPluginData->hbmActiveMinimize, 0, pPluginData);
                }

                if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_MAXIMIZE,FALSE),&rcl))
                {
                    //draw MaximizeButton
                  if(pPluginData->hbmActiveMaximize==NULLHANDLE)
                    if (!CreateImages(pPluginData))
                      return FALSE;
                  //  rcl.yBottom-=1;    
                  memset(&psP,0,sizeof(psP));
                  psP.hwnd=hwnd;
                  psP.hps=hps;
                  psP.hpsWin=hps;
                  psP.rectlWindow=rcl;
                  psP.rectlUpdate=rcl;
                  
                  DrawImage(&psP, pPluginData->hbmActiveMaximize, 0, pPluginData);
                }

                if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_RESTORE,FALSE),&rcl))
                {
                    //draw RestoreButton
                  if(pPluginData->hbmActiveRestore==NULLHANDLE)
                    if (!CreateImages(pPluginData))
                      return FALSE;
                  //  rcl.yBottom-=1;    
                  memset(&psP,0,sizeof(psP));
                  psP.hwnd=hwnd;
                  psP.hps=hps;
                  psP.hpsWin=hps;
                  psP.rectlWindow=rcl;
                  psP.rectlUpdate=rcl;
         
                  DrawImage(&psP, pPluginData->hbmActiveRestore, 0, pPluginData);

                }

                if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_HIDE,FALSE),&rcl))
                {
                    //draw HideButton
                  if(pPluginData->hbmActiveHide==NULLHANDLE)
                    if (!CreateImages(pPluginData))
                      return FALSE;
                  // rcl.yBottom-=1;    
                  memset(&psP,0,sizeof(psP));
                  psP.hwnd=hwnd;
                  psP.hps=hps;
                  psP.hpsWin=hps;
                  psP.rectlWindow=rcl;
                  psP.rectlUpdate=rcl;
         
                  DrawImage(&psP, pPluginData->hbmActiveHide, 0, pPluginData);
                }
                WinEndPaint(hps);
                return TRUE;              
            
            }
        }

      return FALSE;
    case MM_INSERTITEM:
      {
        PMENUITEM pmi;
        MENUITEM mi;

        pmi=(PMENUITEM) PVOIDFROMMP(mp1);
        
        *rc = pfnOrigWndProc(hwnd, msg, mp1, mp2);
        
        if (pmi->id == SC_RESTORE)
          {
            if (pPluginData->hbmActiveRestore != NULLHANDLE) {
              pmi->afStyle|=MIS_OWNERDRAW;
              WinSendMsg(hwnd,MM_SETITEM,MPFROM2SHORT(0,FALSE),pmi);
            }
          }
        else if (pmi->id == SC_MAXIMIZE)
          {
            if (pPluginData->hbmActiveMaximize != NULLHANDLE) {
              pmi->afStyle|=MIS_OWNERDRAW;
              WinSendMsg(hwnd,MM_SETITEM,MPFROM2SHORT(0,FALSE),pmi);
            }
          }
        else if (pmi->id == SC_HIDE)
          {
            if (pPluginData->hbmActiveHide != NULLHANDLE) {
              pmi->afStyle|=MIS_OWNERDRAW;
              WinSendMsg(hwnd,MM_SETITEM,MPFROM2SHORT(0,FALSE),pmi);
            }
          }
        else if (pmi->id == SC_MINIMIZE)
          {
            if (pPluginData->hbmActiveMinimize != NULLHANDLE) {
              pmi->afStyle|=MIS_OWNERDRAW;
              WinSendMsg(hwnd,MM_SETITEM,MPFROM2SHORT(0,FALSE),pmi);
            }
          }
        else if (pmi->id == SC_CLOSE)
          {
            if (pPluginData->hbmActiveClose != NULLHANDLE) {
              pmi->afStyle|=MIS_OWNERDRAW;
              WinSendMsg(hwnd,MM_SETITEM,MPFROM2SHORT(0,FALSE),pmi);
            }
          }        
        return TRUE;
      }
      break;
    default:
      break;
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

            WinSendMsg(WinWindowFromID(hwnd, EFID_IMAGE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);

            WinSetWindowText(WinWindowFromID(hwnd, EFID_IMAGE), pPluginData->szImageFile);

            /* This is temprary. At the moment only all or no frame control may be drawn */
            WinCheckButton(hwnd, CBID_CLOSEENABLED, TRUE);
            WinCheckButton(hwnd, CBID_MINIMIZEENABLED, TRUE);
            WinCheckButton(hwnd, CBID_MAXIMIZEENABLED, TRUE);
            WinCheckButton(hwnd, CBID_RESTOREENABLED, TRUE);
            WinCheckButton(hwnd, CBID_HIDEENABLED, TRUE);

            WinEnableWindow(WinWindowFromID(hwnd,CBID_CLOSEENABLED ), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, CBID_MINIMIZEENABLED), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, CBID_MAXIMIZEENABLED), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, CBID_RESTOREENABLED), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, CBID_HIDEENABLED), FALSE);

        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
                case PBID_IMAGE:
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

                    WinSetWindowText(WinWindowFromID(hwnd, EFID_IMAGE), filedlg.szFullFile);

                }
                break;


                case PBID_OK:
                {
                    if ((pPluginData = (PLUGINSHARE *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        //error message..
                        break;
                    }

                    WinQueryWindowText(WinWindowFromID(hwnd, EFID_IMAGE), CCHMAXPATH, pPluginData->szImageFile);

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

    AddEnabledAttribute(retStr, 5);
    AddStringAttribute(retStr, "ImageFile", pPluginData->szImageFile);

    return (TRUE);
};


HBITMAP SetupBitmapFromFile(HWND hwnd, char *pszFileName /*, long lVertical2*/)
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

// tiles an image across preview window
BOOL DrawImage(PSPAINT * pPaint, HBITMAP hbm, short sState, PLUGINSHARE* pPluginData)
{
    SIZEL slHps;
    POINTL aptl[4];
    HDC hdc;
    HPS hps;
    HAB hab = WinQueryAnchorBlock(pPaint->hwnd);


    // setup source bitmap
    hdc = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE);

    // create source hps
    slHps.cx = pPluginData->ulCx/2;
    slHps.cy = pPluginData->ulCy/5;
    hps = GpiCreatePS(hab,
                      hdc,
                      &slHps,
                      PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);

    // set bmp into hps
    GpiSetBitmap(hps, hbm);

    // initial blt location
    aptl[0].x = pPaint->rectlWindow.xLeft;
    //aptl[0].y = pPaint->rectlWindow.yBottom;
    aptl[0].y = pPaint->rectlWindow.yBottom-1;
    aptl[1].x = pPaint->rectlWindow.xRight;
    //aptl[1].y = pPaint->rectlWindow.yTop;
    aptl[1].y = pPaint->rectlWindow.yTop-1;
    aptl[2].x = 0;
    aptl[2].y = 0;
    aptl[3].x = slHps.cx;
    aptl[3].y = slHps.cy;
     // blt
     GpiBitBlt(pPaint->hps, hps, 4L, aptl, ROP_SRCCOPY, BBO_IGNORE);

    // clean up
    GpiSetBitmap(hps, NULLHANDLE);
    GpiDestroyPS(hps);
    DevCloseDC(hdc);

    return (TRUE);


}


BOOL CreateImages(PLUGINSHARE *pPluginData)
{
    int     i;
    HBITMAP hbmTemp;
    HAB     hab = WinQueryAnchorBlock(HWND_DESKTOP);
    HDC     hdcSrc, hdcDest;
    HPS     hpsSrc, hpsDest;
    SIZEL   slHPS;
    BITMAPINFOHEADER2 bif2Src, bif2Dest;
    POINTL aptl[4];

    if (pPluginData == NULL)
        return (FALSE);

    hbmTemp = SetupBitmapFromFile(HWND_DESKTOP, pPluginData->szImageFile/*, 22*/);

    if (hbmTemp == NULLHANDLE)
        return FALSE;

    memset(&bif2Src, 0, sizeof(bif2Src));
    bif2Src.cbFix = sizeof(bif2Src);
    GpiQueryBitmapInfoHeader(hbmTemp, &bif2Src);
    hdcSrc = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE);
    slHPS.cx = bif2Src.cx;
    slHPS.cy = bif2Src.cy;
    pPluginData->ulCx=slHPS.cx;
    pPluginData->ulCy=slHPS.cy;

    hpsSrc = GpiCreatePS(hab,
                      hdcSrc,
                      &slHPS,
                      PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
    GpiSetBitmap(hpsSrc, hbmTemp);

    hdcDest = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE);


    slHPS.cx = bif2Src.cx / 2;
    slHPS.cy = bif2Src.cy / 5;
    hpsDest = GpiCreatePS(hab,
                      hdcDest,
                      &slHPS,
                      PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);

    memset(&bif2Dest, 0, sizeof(bif2Dest));
    bif2Dest.cbFix = sizeof(bif2Dest);
    bif2Dest.cx = slHPS.cx;
    bif2Dest.cy = slHPS.cy;
    bif2Dest.cBitCount = 24;
    bif2Dest.cPlanes = 1;

//Close Button
    pPluginData->hbmActiveClose = GpiCreateBitmap(hpsDest,
                                    &bif2Dest,
                                    0L,
                                    NULL,
                                    NULL);

    GpiSetBitmap(hpsDest, pPluginData->hbmActiveClose);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slHPS.cx;
    aptl[1].y = slHPS.cy;
    aptl[2].x = 0;
    aptl[2].y = 0;
    aptl[3].x = slHPS.cx;
    aptl[3].y = slHPS.cy;
    GpiBitBlt(hpsDest, hpsSrc, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);

//Minimize Button
    pPluginData->hbmActiveMinimize = GpiCreateBitmap(hpsDest,
                                    &bif2Dest,
                                    0L,
                                    NULL,
                                    NULL);

    GpiSetBitmap(hpsDest, pPluginData->hbmActiveMinimize);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slHPS.cx;
    aptl[1].y = slHPS.cy;
    aptl[2].x = 0;
    aptl[2].y = slHPS.cy * 1;
    aptl[3].x = aptl[2].x + slHPS.cx;
    aptl[3].y = aptl[2].y + slHPS.cy;
    GpiBitBlt(hpsDest, hpsSrc, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);

//Maximize Button
    pPluginData->hbmActiveMaximize = GpiCreateBitmap(hpsDest,
                                    &bif2Dest,
                                    0L,
                                    NULL,
                                    NULL);

    GpiSetBitmap(hpsDest, pPluginData->hbmActiveMaximize);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slHPS.cx;
    aptl[1].y = slHPS.cy;
    aptl[2].x = 0;
    aptl[2].y = slHPS.cy * 2;
    aptl[3].x = aptl[2].x + slHPS.cx;
    aptl[3].y = aptl[2].y + slHPS.cy;
    GpiBitBlt(hpsDest, hpsSrc, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);

//Restore Button
    pPluginData->hbmActiveRestore = GpiCreateBitmap(hpsDest,
                                    &bif2Dest,
                                    0L,
                                    NULL,
                                    NULL);

    GpiSetBitmap(hpsDest, pPluginData->hbmActiveRestore);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slHPS.cx;
    aptl[1].y = slHPS.cy;
    aptl[2].x = 0;
    aptl[2].y = slHPS.cy * 3;
    aptl[3].x = aptl[2].x + slHPS.cx;
    aptl[3].y = aptl[2].y + slHPS.cy;
    GpiBitBlt(hpsDest, hpsSrc, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);

//Hide Button
    pPluginData->hbmActiveHide = GpiCreateBitmap(hpsDest,
                                    &bif2Dest,
                                    0L,
                                    NULL,
                                    NULL);

    GpiSetBitmap(hpsDest, pPluginData->hbmActiveHide);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slHPS.cx;
    aptl[1].y = slHPS.cy;
    aptl[2].x = 0;
    aptl[2].y = slHPS.cy * 4;
    aptl[3].x = aptl[2].x + slHPS.cx;
    aptl[3].y = aptl[2].y + slHPS.cy;
    GpiBitBlt(hpsDest, hpsSrc, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);



    /* Inactive... */

//Close Button
    pPluginData->hbmInactiveClose = GpiCreateBitmap(hpsDest,
                                    &bif2Dest,
                                    0L,
                                    NULL,
                                    NULL);

    GpiSetBitmap(hpsDest, pPluginData->hbmInactiveClose);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slHPS.cx;
    aptl[1].y = slHPS.cy;
    aptl[2].x = slHPS.cx ;
    aptl[2].y = 0;
    aptl[3].x = aptl[2].x + slHPS.cx;
    aptl[3].y = aptl[2].y + slHPS.cy;
    GpiBitBlt(hpsDest, hpsSrc, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);

//Minimize Button
    pPluginData->hbmInactiveMinimize = GpiCreateBitmap(hpsDest,
                                    &bif2Dest,
                                    0L,
                                    NULL,
                                    NULL);

    GpiSetBitmap(hpsDest, pPluginData->hbmInactiveMinimize);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slHPS.cx;
    aptl[1].y = slHPS.cy;
    aptl[2].x = slHPS.cx;
    aptl[2].y = slHPS.cy * 1;
    aptl[3].x = aptl[2].x + slHPS.cx;
    aptl[3].y = aptl[2].y + slHPS.cy;
    GpiBitBlt(hpsDest, hpsSrc, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);

//Maximize Button
    pPluginData->hbmInactiveMaximize = GpiCreateBitmap(hpsDest,
                                    &bif2Dest,
                                    0L,
                                    NULL,
                                    NULL);

    GpiSetBitmap(hpsDest, pPluginData->hbmInactiveMaximize);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slHPS.cx;
    aptl[1].y = slHPS.cy;
    aptl[2].x = slHPS.cx;
    aptl[2].y = slHPS.cy * 2;
    aptl[3].x = aptl[2].x + slHPS.cx;
    aptl[3].y = aptl[2].y + slHPS.cy;
    GpiBitBlt(hpsDest, hpsSrc, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);


//Restore Button
    pPluginData->hbmInactiveRestore = GpiCreateBitmap(hpsDest,
                                    &bif2Dest,
                                    0L,
                                    NULL,
                                    NULL);

    GpiSetBitmap(hpsDest, pPluginData->hbmInactiveRestore);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slHPS.cx;
    aptl[1].y = slHPS.cy;
    aptl[2].x = slHPS.cx;
    aptl[2].y = slHPS.cy * 3;
    aptl[3].x = aptl[2].x + slHPS.cx;
    aptl[3].y = aptl[2].y + slHPS.cy;
    GpiBitBlt(hpsDest, hpsSrc, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);


//Hide Button
    pPluginData->hbmInactiveHide = GpiCreateBitmap(hpsDest,
                                    &bif2Dest,
                                    0L,
                                    NULL,
                                    NULL);

    GpiSetBitmap(hpsDest, pPluginData->hbmInactiveHide);

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slHPS.cx;
    aptl[1].y = slHPS.cy;
    aptl[2].x = slHPS.cx;
    aptl[2].y = slHPS.cy * 4;
    aptl[3].x = aptl[2].x + slHPS.cx;
    aptl[3].y = aptl[2].y + slHPS.cy;
    GpiBitBlt(hpsDest, hpsSrc, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);

   GpiSetBitmap(hpsSrc, NULLHANDLE);

    if (hbmTemp != NULLHANDLE)
        GpiDeleteBitmap(hbmTemp);
    if (hpsSrc != NULLHANDLE)
        GpiDestroyPS(hpsSrc);
    if (hdcSrc != NULLHANDLE)
        DevCloseDC(hdcSrc);


    GpiSetBitmap(hpsDest, NULLHANDLE);
    if (hpsDest != NULLHANDLE)
        GpiDestroyPS(hpsDest);
    if (hdcDest != NULLHANDLE)
        DevCloseDC(hdcDest);

    return TRUE;
}

