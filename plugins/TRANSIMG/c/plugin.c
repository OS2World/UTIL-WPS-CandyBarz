
/******************************************************************************

  Copyright Netlabs, 1998-2000 

  http://www.netlabs.org

  (C) E. Norman, C. Wohlgemuth

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


#include "plugin.h"
#include <errno.h>
#include <direct.h>
static BOOL bPicConverted=FALSE;

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pInfo)
{
    strcpy(pInfo->PluginName, "CandyBarZ Transparent Images");
    strcpy(pInfo->PluginAuthor, "CandyBarZ Team @ OS/2 Netlabs");
    strcpy(pInfo->PluginDate, "Sep. 30, 2000");
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
        strcat(szReadName, "_TRANSIMGPlugin");
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
    }   //end if (szName > 0)
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
            strcat(szReadName, "_TRANSIMGPlugin");
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
        pPluginData->ActiveRed = pPluginShare->ActiveRed ;
        pPluginData->ActiveGreen = pPluginShare->ActiveGreen ;
        pPluginData->ActiveBlue = pPluginShare->ActiveBlue ;
        pPluginData->InactiveRed = pPluginShare->InactiveRed ;
        pPluginData->InactiveGreen = pPluginShare->InactiveGreen ;
        pPluginData->InactiveBlue = pPluginShare->InactiveBlue ;
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

    pPluginShare->ActiveRed = SLIDERSCALE/2;
    pPluginShare->ActiveGreen = SLIDERSCALE/2;
    pPluginShare->ActiveBlue = SLIDERSCALE/2;
    pPluginShare->InactiveRed = SLIDERSCALE/2;
    pPluginShare->InactiveGreen = SLIDERSCALE/2;
    pPluginShare->InactiveBlue = SLIDERSCALE/2;

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

            else if (strcmp(kv[i].key, "ActiveRed") == 0)
            {
                pPluginShare->ActiveRed = (SHORT) StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveGreen") == 0)
              {
                pPluginShare->ActiveGreen = (SHORT) StringToLong(kv[i].value);
              }
            else if (strcmp(kv[i].key, "ActiveBlue") == 0)
              {
                pPluginShare->ActiveBlue = (SHORT) StringToLong(kv[i].value);
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

            else if (strcmp(kv[i].key, "InactiveRed") == 0)
            {
                pPluginShare->InactiveRed = (SHORT) StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveGreen") == 0)
              {
                pPluginShare->InactiveGreen = (SHORT) StringToLong(kv[i].value);
              }
            else if (strcmp(kv[i].key, "InactiveBlue") == 0)
              {
                pPluginShare->InactiveBlue = (SHORT) StringToLong(kv[i].value);
              }


        } //end bInactive if condition
    }//end for loop

    //if null, this is a default apply
    if (bDefaultApply)
    {
      char chrSetupString[CCHMAXPATH+100];
      HOBJECT hObject;
      /* We do not reach this if in theme builder */
      
      sprintf(chrSetupString,"BACKGROUND=%s,N,I",pPluginShare->szActiveFile);
      if((hObject=WinQueryObject("<WP_DESKTOP>"))!=NULLHANDLE)
        WinSetObjectData(hObject,chrSetupString); 

          
    }
 

#if 0
    /*   */
    if(!bPicConverted)
      {
        char instDir[CCHMAXPATH];
        char cmdParam[CCHMAXPATH*3];
        LONG lCx=WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
        LONG lCy=WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );

        if(GetInstallDir(instDir, sizeof(instDir)))
          {
            sprintf(cmdParam," %sgbmsize -w%d -h%d %s %s%s\\bg.bmp", instDir, lCx, lCy, pPluginShare->szActiveFile, instDir,TEMPDIRNAME);
        
            WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,cmdParam,"Title",1234,MB_OK);
            strcat(instDir,TEMPDIRNAME);

            if(mkdir(instDir))
              if(errno==ENOENT)
                return TRUE; /* Path is unknown */
            system(cmdParam);
            bPicConverted=TRUE;
          }
      }
#endif

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
        strcat(szSaveName, "_TRANSIMGPlugin");
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
        strcat(szSaveName, "_TRANSIMGPlugin");

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

#if 0       
    if(pPluginData->hDive) {
      DiveFreeImageBuffer(pPluginData->hDive,pPluginData->ulDiveBuffer);
      DiveClose(pPluginData->hDive);
            DosBeep(5000,600);
    }
#endif

    if(pPluginData->pSharedMem)
      DosFreeMem(pPluginData->pSharedMem);
    if(pPluginData->pBifSharedMem)
      DosFreeMem(pPluginData->pBifSharedMem);
    
    DosFreeMem(pPluginData);

    return (TRUE);
}

//BOOL drawTransparentImage(HPS targetPS, PSPAINT * pPaint, PLUGINSHARE *pPluginData);

//Draw the Plugin effect into the given presentation space
BOOL CBZPluginRender(PSPAINT * pPaint, PVOID pData, short sActive)
{
    BITMAPINFOHEADER2 bif2;
    PLUGINSHARE *pPluginData;
    BITMAPINFOHEADER2 *pbmih;
    SETUP_BLITTER setupBlitter;
    POINTL ptl, ptlSource;
    RECTL rclSource;


    pPluginData = (PLUGINSHARE *) pData;
    

    if (pPluginData == NULL)
        return (FALSE);

    if (( ((sActive == 1) || (sActive == 3))   && !(pPluginData->bActiveEnabled)) ||
            ( ((sActive == 0) || (sActive == 2)) && !(pPluginData->bInactiveEnabled)))
        return (TRUE);

    if ( (sActive == 1) && (pPluginData->hbmActive == NULLHANDLE) ) {
      pPluginData->hbmActive = SetupBitmapFromFile(pPaint->hwnd, pPluginData->szActiveFile, pPaint->rectlWindow.yTop, pPluginData);

      // get bitmap info
      memset(&bif2, 0, sizeof(bif2));
      bif2.cbFix = sizeof(bif2);
      GpiQueryBitmapInfoHeader(pPluginData->hbmActive, &bif2);

      pPluginData->ulCx=bif2.cx;
      pPluginData->ulCy=bif2.cy;
      pPluginData->lCxScreen=WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN );
      pPluginData->lCyScreen=WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN );

    }
    
    //make sure no error occured.
    if ( (sActive == 1) && (pPluginData->hbmActive != NULLHANDLE) )
      {
        
        ptl.x=pPaint->rectlUpdate.xLeft;
        ptl.y=pPaint->rectlUpdate.yBottom;
          
        rclSource=pPaint->rectlUpdate;
        WinMapWindowPoints(pPaint->hwnd,HWND_DESKTOP,(PPOINTL)&rclSource,2L);
        
        WinDrawBitmap(pPaint->hps, pPluginData->hbmActive, &rclSource, &ptl, 0L, 0L, DBM_NORMAL);
        
        //    drawTransparentImage( pPaint->hps, pPaint, pPluginData);     
    }
    return (TRUE);
}

//Called before the default wnd procedure if you wish to provide additional
//  functionality.  (i.e. modify what happens with mouse clicks, drag/drop, etc...
BOOL CBZPluginWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2,
                        PVOID pData, PFNWP pfnOrigWndProc, MRESULT *rc)
{
  PSWP pswp;
  RECTL rcl, rclClient;
  HWND hwndClient;
  
  PLUGINSHARE *pPluginData;
  
  pPluginData = (PLUGINSHARE *) pData;


    switch (msg)
    {

    case WM_WINDOWPOSCHANGED:
      pswp=PVOIDFROMMP(mp1);
      if(!pPluginData->bDontPaint) {
        if(pswp->fl & SWP_MOVE) {
          WinQueryWindowRect(hwnd,&rcl);
          hwndClient=WinWindowFromID(hwnd,FID_CLIENT);
          if(hwndClient) { /* We have got a Client */
            /* get client dimensions */
            WinQueryWindowRect(hwndClient,&rclClient);
            /* Check if the client covers almost the whole frame. If so do not paint the background. */
            /* That gives us a lot of performance */
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //Achtung fÅr den Vergleich wird die Framegrî·e gebraucht, nicht update!!
            //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            if(((rcl.xRight-rcl.xLeft)-(rclClient.xRight-rclClient.xLeft))<50) {/* 50 seems to be a good value */
              /* Set a marker into the framewindow data area to show we don't have a custom Background */
              /* Do not paint the BG for this frame in the future */
              pPluginData->bDontPaint=TRUE;
            }
          }/* end of if(hwndClient) */
          WinInvalidateRect(hwnd,&rcl, TRUE);
        }/* end of if(pswp->fl & SWP_MOVE) */
      }
      break;      
    default:
      break;
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
    SHORT sPos;
    SHORT sLen;
    MRESULT mr;
char text[100];

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

            WinSetWindowText(WinWindowFromID(hwnd, STID_ACTIVEIMAGE), pPluginData->szActiveFile);

            if (pPluginData->bActiveEnabled)
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVERED), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVEGREEN), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVEBLUE), TRUE);

            }
            else
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVERED), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVEGREEN), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVEBLUE), FALSE);

            }
            if (pPluginData->bInactiveEnabled)
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVERED), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVEGREEN), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVEBLUE), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVERED), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVEGREEN), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVEBLUE), FALSE);       
            }

            /* Inactive backgrounds currently not implemented!! */
            WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVEENABLED), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVERED), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVEGREEN), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVEBLUE), FALSE);       
            /* End of not implemented */
            
            WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVERED),SLM_SETTICKSIZE,MPFROM2SHORT(SMA_SETALLTICKS,8),NULL);
            WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVEGREEN),SLM_SETTICKSIZE,MPFROM2SHORT(SMA_SETALLTICKS,8),NULL);
            WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVEBLUE),SLM_SETTICKSIZE,MPFROM2SHORT(SMA_SETALLTICKS,8),NULL);

            WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVERED),SLM_SETTICKSIZE,MPFROM2SHORT(SMA_SETALLTICKS,8),NULL);
            WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVEGREEN),SLM_SETTICKSIZE,MPFROM2SHORT(SMA_SETALLTICKS,8),NULL);
            WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVEBLUE),SLM_SETTICKSIZE,MPFROM2SHORT(SMA_SETALLTICKS,8),NULL);


            mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVERED),SLM_QUERYSLIDERINFO,
                          MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),NULL);
 
           sLen=SHORT2FROMMR(mr)-1; /* Len of slidershaft in pixel */
         
#if 0           
           sprintf(text,"%d",(sLen*pPluginData->ActiveRed)/SLIDERSCALE);
           WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,text,"Title",1234,MB_OK);
#endif
           /* Set slider positions */
            mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVERED),SLM_SETSLIDERINFO,
                          MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),
                          MPFROMSHORT((SHORT)((sLen*pPluginData->ActiveRed)/SLIDERSCALE)));
            
            mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVEBLUE),SLM_SETSLIDERINFO,
                          MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),
                          MPFROMSHORT((SHORT)((sLen*pPluginData->ActiveBlue)/SLIDERSCALE)));
      
            mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVEGREEN),SLM_SETSLIDERINFO,
                          MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),
                          MPFROMSHORT((SHORT)((sLen*pPluginData->ActiveGreen)/SLIDERSCALE)));
 
            mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVERED),SLM_SETSLIDERINFO,
                          MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),
                          MPFROMSHORT((SHORT)((sLen*pPluginData->InactiveRed)/SLIDERSCALE)));
          
            mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVEBLUE),SLM_SETSLIDERINFO,
                          MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),
                          MPFROMSHORT((SHORT)((sLen*pPluginData->InactiveBlue)/SLIDERSCALE)));
            
            mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVEGREEN),SLM_SETSLIDERINFO,
                          MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),
                          MPFROMSHORT((SHORT)((sLen*pPluginData->InactiveGreen)/SLIDERSCALE)));
       

            WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVERED),SLM_SETSCALETEXT,MPFROMSHORT(0),"-");
            WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVERED),SLM_SETSCALETEXT,MPFROMSHORT(4),"0");
            WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVERED),SLM_SETSCALETEXT,MPFROMSHORT(8),"+");

            WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVERED),SLM_SETSCALETEXT,MPFROMSHORT(0),"-");
            WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVERED),SLM_SETSCALETEXT,MPFROMSHORT(4),"0");
            WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVERED),SLM_SETSCALETEXT,MPFROMSHORT(8),"+");

        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
                case PBID_CHANGEACTIVEIMAGE:
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

                    if ( WinQueryButtonCheckstate(hwnd, CBID_ACTIVEENABLED) )
                        pPluginData->bActiveEnabled = TRUE;
                    else
                        pPluginData->bActiveEnabled = FALSE;

                    if ( WinQueryButtonCheckstate(hwnd, CBID_INACTIVEENABLED) )
                        pPluginData->bInactiveEnabled = TRUE;
                    else
                        pPluginData->bInactiveEnabled = FALSE;

                   
                    mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVERED),SLM_QUERYSLIDERINFO,
                                  MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),NULL);
                    sPos=SHORT1FROMMR(mr);
                    sLen=SHORT2FROMMR(mr);
                    pPluginData->ActiveRed=sPos*SLIDERSCALE/sLen;

                    mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVEBLUE),SLM_QUERYSLIDERINFO,
                                  MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),NULL);
                    sPos=SHORT1FROMMR(mr);
                    sLen=SHORT2FROMMR(mr);
                    pPluginData->ActiveBlue=sPos*SLIDERSCALE/sLen;

                    mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_ACTIVEGREEN),SLM_QUERYSLIDERINFO,
                                  MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),NULL);
                    sPos=SHORT1FROMMR(mr);
                    sLen=SHORT2FROMMR(mr);
                    pPluginData->ActiveGreen=sPos*SLIDERSCALE/sLen;

                    mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVERED),SLM_QUERYSLIDERINFO,
                                  MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),NULL);
                    sPos=SHORT1FROMMR(mr);
                    sLen=SHORT2FROMMR(mr);
                    pPluginData->InactiveRed=sPos*SLIDERSCALE/sLen;

                    mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVEBLUE),SLM_QUERYSLIDERINFO,
                                  MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),NULL);
                    sPos=SHORT1FROMMR(mr);
                    sLen=SHORT2FROMMR(mr);
                    pPluginData->InactiveBlue=sPos*SLIDERSCALE/sLen;
                    mr=WinSendMsg(WinWindowFromID(hwnd, IDSL_INACTIVEGREEN),SLM_QUERYSLIDERINFO,
                                  MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),NULL);
                    sPos=SHORT1FROMMR(mr);
                    sLen=SHORT2FROMMR(mr);
                    pPluginData->InactiveGreen=sPos*SLIDERSCALE/sLen;



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
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVERED), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVEGREEN), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVEBLUE), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVERED), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVEGREEN), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_ACTIVEBLUE), FALSE);
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
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVERED), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVEGREEN), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVEBLUE), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVERED), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVEGREEN), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, IDSL_INACTIVEBLUE), FALSE);
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
    else 
      if (pPluginData->bInactiveEnabled)
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

    /* Save RGB values */
    AddLongAttribute(retStr, "ActiveRed", (LONG)pPluginData->ActiveRed);
    AddLongAttribute(retStr, "ActiveGreen", (LONG)pPluginData->ActiveGreen);
    AddLongAttribute(retStr, "ActiveBlue", (LONG)pPluginData->ActiveBlue);
    
    AddLongAttribute(retStr, "InactiveRed", (LONG)pPluginData->InactiveRed);
    AddLongAttribute(retStr, "InactiveGreen", (LONG)pPluginData->InactiveGreen);
    AddLongAttribute(retStr, "InactiveBlue", (LONG)pPluginData->InactiveBlue);
    
    return (TRUE);
};

HBITMAP SetupBitmapFromFile(HWND hwnd, char *pszFileName, long lVertical, PLUGINSHARE *pPluginData)
{
    HAB hab = WinQueryAnchorBlock(hwnd);
    PSTBFILE *pFile;
    HBITMAP hbm;
    void* pMem;
    void* pBifMem;
    int a, r,g,b;
    RGB2* rgb2;
    BITMAPINFOHEADER2 *ptr;
    HPS hps;
    HDC hdc;
    HBITMAP hbm2;
    SIZEL slHPS;

    /* Try to get the image data from shared mem */
    if(DosGetNamedSharedMem(&pMem,BMP_SHARE,PAG_READ | PAG_WRITE )!=NO_ERROR) {
      
      /* We are the first using this plugin so setup the data */
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

      if(8 != pFile->mmImgHdr.mmXDIBHeader.BMPInfoHeader2.cBitCount) {
        PSMMCloseImageFile(pFile->hmmio);
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
      
      pMem=NULL;
      pBifMem=NULL;
      
      /* Alloc shared mem for bmp data */
      if(DosAllocSharedMem(&pMem,BMP_SHARE, pFile->cbData,
                           PAG_COMMIT|PAG_READ|PAG_WRITE)==NO_ERROR)
        {
          if(DosAllocSharedMem(&pBifMem,BMPINFO_SHARE, sizeof(BITMAPINFOHEADER2),//+sizeof(pPluginData->rgb2),
                               PAG_COMMIT|PAG_READ|PAG_WRITE)==NO_ERROR)
            {
              /* Copy pic data to shared mem */
              memcpy(pMem,pFile->pvImage,pFile->cbData);
              /* Copy BMP-info and palette to shared mem */
              memcpy(pBifMem,& (pFile->mmImgHdr.mmXDIBHeader.BMPInfoHeader2),
                     sizeof(BITMAPINFOHEADER2)+sizeof(pPluginData->rgb2));
              /* Copy BMP-info and palette to plugin data */
              memcpy(&(pPluginData->bif2),& (pFile->mmImgHdr.mmXDIBHeader.BMPInfoHeader2),
                     sizeof(BITMAPINFOHEADER2)+sizeof(pPluginData->rgb2));
            }
          else {
            DosFreeMem(pMem);
            pMem=NULLHANDLE;
          }
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

      /* Adapt colors */
      ptr=&(pPluginData->bif2);
      ptr++;
      rgb2=(RGB2*) ptr;
      
      for(a=16;a<255;a++,rgb2++) {
        b=pPluginData->ActiveBlue-SLIDERSCALE/2;
        r=pPluginData->ActiveRed-SLIDERSCALE/2;
        g=pPluginData->ActiveGreen-SLIDERSCALE/2;
        
        
        b+=rgb2->bBlue;
        g+=rgb2->bGreen;
        r+=rgb2->bRed;
        if(r>255)
          r=255;
        if(r<0)
          r=0;
        rgb2->bRed=r;

        if(g>255)
          g=255;
        if(g<0)
          g=0;
        rgb2->bGreen=g;

        if(b>255)
          b=255;
        if(b<0)
          b=0;
        rgb2->bBlue=b;        

        // rgb2->bRed=(r<0 ? 0 : r & 0xFF);
        // rgb2->bGreen=(g<0 ? 0 : g & 0xFF);
        // rgb2->bBlue=(b<0 ? 0 : b & 0xFF);
      }

      pPluginData->bColorSetupDone=TRUE;
      
      GpiSetBitmapBits(pFile->hps,
                       0L,
                       (long) pFile->slHPS.cy,
                       (PBYTE) pFile->pvImage,   /* This contains the data */
                       (PBITMAPINFO2) & (pPluginData->bif2));
      
      hbm = pFile->hbm;
      pPluginData->pSharedMem=pMem;
      pPluginData->pBifSharedMem=pBifMem;
      
      GpiDeleteBitmap(pFile->hbm);
      
      // free everything up
      GpiSetBitmap(pFile->hps, NULLHANDLE);
      GpiDestroyPS(pFile->hps);
      DevCloseDC(pFile->hdc);
      DosFreeMem(pFile->pvImage);
      DosFreeMem(pFile);
   
      return (hbm);
    }/* end of if(DosGetNamedSharedMem(&pMem,BMP_SHARE,PAG_READ | PAG_WRITE )!=NO_ERROR) */
    else
      {
        /* We have shared mem with the image data */

        if(DosGetNamedSharedMem(&pBifMem,BMPINFO_SHARE,PAG_READ | PAG_WRITE )==NO_ERROR) {          
          // convert data to system bitcount
          hdc = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE);
          

          if(!pPluginData->bColorSetupDone) {
            /* Copy BMP-info and palette from shared mem */
            memcpy(&(pPluginData->bif2),pBifMem,
                   sizeof(BITMAPINFOHEADER2)+sizeof(pPluginData->rgb2));
            
            /* Adapt colors */
            ptr=&(pPluginData->bif2);
            ptr++;
            rgb2=(RGB2*) ptr;
            
            for(a=0;a<240;a++,rgb2++) {
              b=pPluginData->ActiveBlue-SLIDERSCALE/2;
              r=pPluginData->ActiveRed-SLIDERSCALE/2;
              g=pPluginData->ActiveGreen-SLIDERSCALE/2;
            
              b+=rgb2->bBlue;
              g+=rgb2->bGreen;
              r+=rgb2->bRed;
              if(r>255)
                r=255;
              if(r<0)
                r=0;
              rgb2->bRed=r;
              
              if(g>255)
                g=255;
              if(g<0)
                g=0;
              rgb2->bGreen=g;
              
              if(b>255)
                b=255;
              if(b<0)
                b=0;
              rgb2->bBlue=b;        
              // rgb2->bRed=(r<0 ? 0 : r & 0xFF);
              // rgb2->bGreen=(g<0 ? 0 : g & 0xFF);
              // rgb2->bBlue=(b<0 ? 0 : b & 0xFF);
            }/* end of for */

            pPluginData->bColorSetupDone=TRUE;
          }
          slHPS.cx = pPluginData->bif2.cx;
          slHPS.cy = pPluginData->bif2.cy;
          hps = GpiCreatePS(hab,
                            hdc,
                            &slHPS,
                            PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
          
          hbm2 = GpiCreateBitmap(hps,
                                 &(pPluginData->bif2),
                                 //pBifMem,
                                 0L,
                                 NULL,
                                 NULL);

          GpiSetBitmap(hps, hbm2);
            
          GpiSetBitmapBits(hps,
                           0L,
                           (long) slHPS.cy,
                           (PBYTE) pMem,   /* This contains the data */
                           (PBITMAPINFO2)&(pPluginData->bif2));
          
          hbm = hbm2;

          /* Save shared mem ptrs. Freed in pluginDestroy() */
          pPluginData->pSharedMem=pMem;
          pPluginData->pBifSharedMem=pBifMem;

          GpiDeleteBitmap(hbm2);
          
          // free everything up
          GpiSetBitmap(hps, NULLHANDLE);
          GpiDestroyPS(hps);
          DevCloseDC(hdc);
                
          return (hbm);
        }/* end of if(DosGetNamedSharedMem(&pBifMem,BMPINFO_SHARE,PAG_READ | PAG_WRITE )==NO_ERROR) */
        else 
          DosFreeMem(pMem);
      }/* end of else */
    return NULLHANDLE;

}

#if 0
BOOL drawTransparentImage(HPS targetPS, PSPAINT * pPaint, PLUGINSHARE *pPluginData)
{
  SIZEL slHPS;
  HAB hab = WinQueryAnchorBlock(pPaint->hwnd);
  HPS hps;
  HDC hdc;
  POINTL ptl[3];
  RECTL rclSource;

        /* We have shared mem with the image data */

  if(pPluginData->pSharedMem) {          
    // convert data to system bitcount
    hdc = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE);
    
    
    slHPS.cx = pPluginData->bif2.cx;
    slHPS.cy = pPluginData->bif2.cy;
    hps = GpiCreatePS(hab,
                      hdc,
                      &slHPS,
                      PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);

    //ptl.x=pPaint->rectlUpdate.xLeft;
    //ptl.y=pPaint->rectlUpdate.yBottom;
    
    rclSource=pPaint->rectlUpdate;
    WinMapWindowPoints(pPaint->hwnd,HWND_DESKTOP,(PPOINTL)&rclSource,2L);
    
    ptl[0].x=rclSource.xLeft;
    ptl[0].y=rclSource.yBottom;
    ptl[1].x=rclSource.xRight;
    ptl[0].y=rclSource.yTop;
    ptl[2]=ptl[0];

    DosBeep(5000,100);
    
    GpiBitBlt(targetPS,hps,3,ptl,ROP_SRCCOPY, BBO_IGNORE);


    GpiDestroyPS(hps);
    DevCloseDC(hdc);
  }    
}
#endif



