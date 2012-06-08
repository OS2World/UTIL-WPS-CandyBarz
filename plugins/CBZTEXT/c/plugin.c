/* $Id: plugin.c,v 1.5 2000/10/04 19:50:35 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "plugin.h"

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pluginInfo)
{
    strcpy(pluginInfo->PluginName, "CandyBarZ Title Text");
    strcpy(pluginInfo->PluginAuthor, "CandyBarZ Team @ OS/2 Netlabs");
    strcpy(pluginInfo->PluginDate, "3 Sep 2000");
    pluginInfo->lWindowTypes = CBZ_TITLEBAR | CBZ_PUSHBUTTON | CBZ_RADIOBUTTON | CBZ_CHECKBOX;
    return;
}

//called to allocate memory and setup initial settings
BOOL CBZInit(HINI hIni, char szName[], char szClass[], PVOID * pData)
{
    PLUGINSHARE *pPluginShare;
    PLUGINSHARE *pPluginData;
    char szReadName[64];
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
        strcat(szReadName, "_CBZTEXTPlugin");
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
                pPluginData->lActiveTextColor = 0;
                pPluginData->lActiveShadowColor = 0;
                pPluginData->sActiveTextAlignment = LEFT_TEXT;
                pPluginData->flActive = FLAG_DEF_FONT;
                pPluginData->lInactiveTextColor = 0;
                pPluginData->lInactiveShadowColor = 0;
                pPluginData->sInactiveTextAlignment = LEFT_TEXT;
                pPluginData->flInactive = FLAG_DEF_FONT;
                pPluginData->bActiveEnabled = TRUE;
                pPluginData->bInactiveEnabled = TRUE;

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
            strcat(szReadName, "_CBZTEXTPlugin");
            if (!PrfQueryProfileData(hIni,
                                     "UserOptionsData",
                                     szReadName,
                                     pPluginShare,
                                     &szData))
            {
                pPluginShare->lActiveTextColor = 0;
                pPluginShare->lActiveShadowColor = 0;
                pPluginShare->sActiveTextAlignment = LEFT_TEXT;
                pPluginShare->flActive = FLAG_DEF_FONT;
                pPluginShare->lInactiveTextColor = 0;
                pPluginShare->lInactiveShadowColor = 0;
                pPluginShare->sInactiveTextAlignment = LEFT_TEXT;
                pPluginShare->flInactive = FLAG_DEF_FONT;
                pPluginShare->bActiveEnabled = TRUE;
                pPluginShare->bInactiveEnabled = TRUE;

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
        pPluginData->lActiveTextColor = pPluginShare->lActiveTextColor;
        pPluginData->lActiveShadowColor = pPluginShare->lActiveShadowColor;
        pPluginData->sActiveTextAlignment = pPluginShare->sActiveTextAlignment;
        pPluginData->flActive = pPluginShare->flActive;
        pPluginData->lInactiveTextColor = pPluginShare->lInactiveTextColor;
        pPluginData->lInactiveShadowColor = pPluginShare->lInactiveShadowColor;
        pPluginData->sInactiveTextAlignment = pPluginShare->sInactiveTextAlignment;
        pPluginData->flInactive = pPluginShare->flInactive;
        pPluginData->bActiveEnabled = pPluginShare->bActiveEnabled;
        pPluginData->bInactiveEnabled = pPluginShare->bInactiveEnabled;
        pPluginData->lActiveTextOffset = pPluginShare->lActiveTextOffset;
        pPluginData->lInactiveTextOffset = pPluginShare->lInactiveTextOffset;
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

    //initialize the plugin's data to zeros.
    memset(pPluginShare, 0, sizeof(PLUGINSHARE));


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
            if (strcmp(kv[i].key, "ActiveTextColor") == 0)
            {
                //StringToLong() returns 0L on error...which is as good as any value for an error condition?
                pPluginShare->lActiveTextColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveShadowColor") == 0)
            {
                pPluginShare->lActiveShadowColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveShadowEnabled") == 0)
            {
                if (strcmp(kv[i].value, "TRUE") == 0)
                    pPluginShare->flActive |= FLAG_3DTEXT;
                else
                    pPluginShare->flActive &= ~FLAG_3DTEXT;
            }
            else if (strcmp(kv[i].key, "ActiveAlignment") == 0)
            {
                if (strcmp(kv[i].value, "manual") == 0)
                    pPluginShare->sActiveTextAlignment = MANUAL_TEXT;
                else if (strcmp(kv[i].value, "center") == 0)
                    pPluginShare->sActiveTextAlignment = CENTER_TEXT;
                else if (strcmp(kv[i].value, "right") == 0)
                    pPluginShare->sActiveTextAlignment = RIGHT_TEXT;
                else
                    pPluginShare->sActiveTextAlignment = LEFT_TEXT;
            }
            else if (strcmp(kv[i].key, "ActiveTextOffset") == 0)
            {
                pPluginShare->lActiveTextOffset = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveCustomTextColor") == 0)
            {

              if (strcmp(kv[i].value, "TRUE") == 0) {
                pPluginShare->flActive |= FLAG_CUSTOM_COLOR;
              }           
              else
                pPluginShare->flActive &= ~FLAG_CUSTOM_COLOR;
            }

        }                       //end bActive if condition

        if (pPluginShare->bInactiveEnabled)
        {
            if (strcmp(kv[i].key, "InactiveTextColor") == 0)
            {
                //StringToLong() returns 0L on error...which is as good as any value for an error condition?
                pPluginShare->lInactiveTextColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveShadowColor") == 0)
            {
                pPluginShare->lInactiveShadowColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveShadowEnabled") == 0)
            {
                if (strcmp(kv[i].value, "TRUE") == 0)
                    pPluginShare->flInactive |= FLAG_3DTEXT;
                else
                    pPluginShare->flInactive &= ~FLAG_3DTEXT;
            }
            else if (strcmp(kv[i].key, "InactiveAlignment") == 0)
            {
                if (strcmp(kv[i].value, "manual") == 0)
                    pPluginShare->sInactiveTextAlignment = MANUAL_TEXT;
                else if (strcmp(kv[i].value, "center") == 0)
                    pPluginShare->sInactiveTextAlignment = CENTER_TEXT;
                else if (strcmp(kv[i].value, "right") == 0)
                    pPluginShare->sInactiveTextAlignment = RIGHT_TEXT;
                else
                    pPluginShare->sInactiveTextAlignment = LEFT_TEXT;
            }
            else if (strcmp(kv[i].key, "InactiveTextOffset") == 0)
            {
                pPluginShare->lInactiveTextOffset = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveCustomTextColor") == 0)
            {
              if (strcmp(kv[i].value, "TRUE") == 0)
                pPluginShare->flInactive |= FLAG_CUSTOM_COLOR;
              else
                pPluginShare->flInactive &= ~FLAG_CUSTOM_COLOR;
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
        strcat(szSaveName, "_CBZTEXTPlugin");
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
        strcat(szSaveName, "_CBZTEXTPlugin");

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

    DosFreeMem(pPluginData);
    return (TRUE);
}

//Draw the Plugin effect into the given presentation space
BOOL CBZPluginRender(PSPAINT * pPaint, PVOID pData, short sActive)
{
    PLUGINSHARE *pPluginData;
    PLUGINPAINT *pPluginPaint;

    // alloc paint struct
    if (DosAllocMem((PPVOID) & pPluginPaint,
                    sizeof(PLUGINPAINT),
                    PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        return (FALSE);
    }
    memset(pPluginPaint, 0, sizeof(PLUGINPAINT));

    pPluginPaint->hab = WinQueryAnchorBlock(pPaint->hwnd);
    pPluginPaint->hpsWin = WinGetPS(pPaint->hwnd);

    pPluginData = (PLUGINSHARE *) pData;

    if (pPluginData == NULL)
    {
        WinReleasePS(pPluginPaint->hpsWin);
        DosFreeMem(pPluginPaint);

        return (FALSE);
    }

    if (((sActive == TRUE) && !(pPluginData->bActiveEnabled)) ||
            ((sActive == FALSE) && !(pPluginData->bInactiveEnabled)))
    {
        WinReleasePS(pPluginPaint->hpsWin);
        DosFreeMem(pPluginPaint);

        return (TRUE);
    }

   
    // get font information for placing text
    memset(&(pPluginPaint->font), 0, sizeof(pPluginPaint->font));
    GpiQueryFontMetrics(pPluginPaint->hpsWin, sizeof(pPluginPaint->font), &(pPluginPaint->font));
    GpiQueryCharBox(pPluginPaint->hpsWin, &(pPluginPaint->sizef));
    
    // default font for ps created with GpiCreatePS is system proportional, so
    // change it to whatever is current font in hpsWin (pPaint->font is filled
    // in CandyBarZPaintProc
    pPluginPaint->lcid = 1;
    memset(&(pPluginPaint->fattrs), 0, sizeof(pPluginPaint->fattrs));
    pPluginPaint->fattrs.usRecordLength = sizeof(pPluginPaint->fattrs);
    pPluginPaint->fattrs.fsSelection = pPluginPaint->font.fsSelection;
    pPluginPaint->fattrs.lMatch = pPluginPaint->font.lMatch;
    strcpy(pPluginPaint->fattrs.szFacename, pPluginPaint->font.szFacename);
    pPluginPaint->fattrs.idRegistry = pPluginPaint->font.idRegistry;
    pPluginPaint->fattrs.usCodePage = pPluginPaint->font.usCodePage;
    pPluginPaint->fattrs.lMaxBaselineExt = pPluginPaint->font.lMaxBaselineExt;
    pPluginPaint->fattrs.lAveCharWidth = pPluginPaint->font.lAveCharWidth;
    pPluginPaint->fattrs.fsType = pPluginPaint->font.fsType;
    pPluginPaint->fattrs.fsFontUse = 0;
    
    GpiCreateLogFont(pPaint->hps, (PSTR8) NULL, pPluginPaint->lcid, &pPluginPaint->fattrs);
    GpiSetCharSet(pPaint->hps, pPluginPaint->lcid);
    GpiSetCharBox(pPaint->hps, &(pPluginPaint->sizef));
    
    
    GpiQueryTextBox(pPaint->hps,
                    strlen(pPaint->szTitleText),
                    pPaint->szTitleText,
                    TXTBOX_COUNT,
                    pPluginPaint->aptl);

    pPaint->rectlText.xLeft = 0;  //pPaint->font.lAveCharWidth;// + 4;

    pPaint->rectlText.yBottom = pPaint->rectlWindow.yBottom;
    pPaint->rectlText.xRight = pPluginPaint->aptl[TXTBOX_TOPRIGHT].x;  // + pPaint->font.lAveCharWidth;// + 4;

    pPaint->rectlText.yTop = pPaint->rectlWindow.yTop;

    if (sActive == TRUE)
    {
        if (pPluginData->sActiveTextAlignment == MANUAL_TEXT)
        {
            WinOffsetRect(pPluginPaint->hab,
                          &(pPaint->rectlText),
                          pPluginData->lActiveTextOffset,
                          0);
        }
        else if (pPluginData->sActiveTextAlignment == CENTER_TEXT)
        {
            WinOffsetRect(pPluginPaint->hab,
                          &(pPaint->rectlText),
                (pPaint->rectlWindow.xRight - pPaint->rectlText.xRight) / 2,
                          0);
        }
        else if (pPluginData->sActiveTextAlignment == RIGHT_TEXT)
        {
            WinOffsetRect(pPluginPaint->hab,
                          &(pPaint->rectlText),
                          (pPaint->rectlWindow.xRight -
                           pPaint->rectlText.xRight -
                           pPluginPaint->font.lAveCharWidth),
                          0);
        }
        else
        {
            WinOffsetRect(pPluginPaint->hab, &(pPaint->rectlText), pPluginPaint->font.lAveCharWidth, 0);
        }
    }
    else
    {
        if (pPluginData->sInactiveTextAlignment == MANUAL_TEXT)
        {
            WinOffsetRect(pPluginPaint->hab,
                          &(pPaint->rectlText),
                          pPluginData->lInactiveTextOffset,
                          0);
        }
        else if (pPluginData->sInactiveTextAlignment == CENTER_TEXT)
        {
            WinOffsetRect(pPluginPaint->hab,
                          &(pPaint->rectlText),
                (pPaint->rectlWindow.xRight - pPaint->rectlText.xRight) / 2,
                          0);
        }
        else if (pPluginData->sInactiveTextAlignment == RIGHT_TEXT)
        {
            WinOffsetRect(pPluginPaint->hab,
                          &(pPaint->rectlText),
                          (pPaint->rectlWindow.xRight -
                           pPaint->rectlText.xRight -
                           pPluginPaint->font.lAveCharWidth),
                          0);
        }
        else
        {
            WinOffsetRect(pPluginPaint->hab,
                          &(pPaint->rectlText),
                          pPluginPaint->font.lAveCharWidth, 0);
        }
    }
    // set color table into rgb mode.  otherwise colors between 0 and
    // 15 are interpreted as indexes
    GpiCreateLogColorTable(pPaint->hps, 0, LCOLF_RGB, 0, 0, NULL);

    if (sActive == TRUE)
    {
        if (pPluginData->flActive & FLAG_3DTEXT)
        {
            //move the text rectangle right and down 1
            pPaint->rectlText.xLeft++;
            pPaint->rectlText.yTop--;
            pPaint->rectlText.yBottom--;

            //draw the shadow text
            WinDrawText(pPaint->hps,
                        strlen(pPaint->szTitleText),
                        pPaint->szTitleText,
                        &(pPaint->rectlText),
                        pPluginData->lActiveShadowColor,
                        0,
                        DT_LEFT | DT_VCENTER | DT_MNEMONIC );

            //return the text rectangle to it's original position.
            pPaint->rectlText.xLeft--;
            pPaint->rectlText.yTop++;
            pPaint->rectlText.yBottom++;
        }
        if(pPluginData->flActive & FLAG_CUSTOM_COLOR)
          // draw foreground text
          WinDrawText(pPaint->hps,
                      strlen(pPaint->szTitleText),
                      pPaint->szTitleText,
                      &(pPaint->rectlText),
                      pPluginData->lActiveTextColor,
                      0,
                      DT_LEFT | DT_VCENTER | DT_MNEMONIC );
        else {
          WinDrawText(pPaint->hps,
                      strlen(pPaint->szTitleText),
                      pPaint->szTitleText,
                      &(pPaint->rectlText),
                      0,
                      0,
                      DT_LEFT | DT_VCENTER | DT_TEXTATTRS | DT_MNEMONIC );
        }
    }
    else
    {
        if (pPluginData->flInactive & FLAG_3DTEXT)
        {
            //move the text rectangle right and down 1
            pPaint->rectlText.xLeft++;
            pPaint->rectlText.yTop--;
            pPaint->rectlText.yBottom--;

            //draw the shadow text
            WinDrawText(pPaint->hps,
                        strlen(pPaint->szTitleText),
                        pPaint->szTitleText,
                        &(pPaint->rectlText),
                        pPluginData->lInactiveShadowColor,
                        0,
                        DT_LEFT | DT_VCENTER | DT_MNEMONIC );

            //return the text rectangle to it's original position.
            pPaint->rectlText.xLeft--;
            pPaint->rectlText.yTop++;
            pPaint->rectlText.yBottom++;

        }
        if(pPluginData->flInactive & FLAG_CUSTOM_COLOR)
          // draw foreground text
          WinDrawText(pPaint->hps,
                      strlen(pPaint->szTitleText),
                      pPaint->szTitleText,
                      &(pPaint->rectlText),
                      pPluginData->lInactiveTextColor,
                      0,
                      DT_LEFT | DT_VCENTER | DT_MNEMONIC );
        else
          WinDrawText(pPaint->hps,
                      strlen(pPaint->szTitleText),
                      pPaint->szTitleText,
                      &(pPaint->rectlText),
                      0,
                      0,
                      DT_LEFT | DT_VCENTER | DT_TEXTATTRS | DT_MNEMONIC );
        
    }
    
    
    WinReleasePS(pPluginPaint->hpsWin);
    DosFreeMem(pPluginPaint);
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
    return FALSE;//false indicates that we haven't processed the message!
}

//A Dlg procedure if the plugin has selectable settings.
MRESULT EXPENTRY CBZPluginSetupDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    short sControlId;
    PLUGINSHARE *pPluginData;
 	HOBJECT  hObject;

    switch (msg)
    {
    case WM_INITDLG:
        {
            if ((pPluginData = (PLUGINSHARE *) mp2) == NULL)
            {
                WinDismissDlg(hwnd, DID_ERROR);
                return (MRFROMLONG(FALSE));
            }
            WinSetWindowPtr(hwnd, QWL_USER, pPluginData);  // store window words

            /* Text alignment */
            if (pPluginData->sActiveTextAlignment == CENTER_TEXT)
                WinCheckButton(hwnd, RBID_ACTIVECENTERTEXT, TRUE);
            else if (pPluginData->sActiveTextAlignment == RIGHT_TEXT)
                WinCheckButton(hwnd, RBID_ACTIVERIGHTTEXT, TRUE);
            else                //left text
                WinCheckButton(hwnd, RBID_ACTIVELEFTTEXT, TRUE);

            if (pPluginData->sInactiveTextAlignment == CENTER_TEXT)
                WinCheckButton(hwnd, RBID_INACTIVECENTERTEXT, TRUE);
            else if (pPluginData->sInactiveTextAlignment == RIGHT_TEXT)
                WinCheckButton(hwnd, RBID_INACTIVERIGHTTEXT, TRUE);
            else                //left text
                WinCheckButton(hwnd, RBID_INACTIVELEFTTEXT, TRUE);

            if (pPluginData->flActive & FLAG_DEF_FONT)
                WinCheckButton(hwnd, CBID_ACTIVEFONT, TRUE);
            if (pPluginData->flActive & FLAG_CUSTOM_COLOR) {
              WinCheckButton(hwnd, CBID_ACTIVETEXTCOLOR, TRUE);
              WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVETEXTCOLOR), TRUE);
            }
            else
              WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVETEXTCOLOR), FALSE);
            if (pPluginData->flActive & FLAG_3DTEXT)
                WinCheckButton(hwnd, CBID_ACTIVE3DTEXT, TRUE);


            if (pPluginData->flInactive & FLAG_DEF_FONT)
                WinCheckButton(hwnd, CBID_INACTIVEFONT, TRUE);
            if (pPluginData->flInactive & FLAG_CUSTOM_COLOR) {
                WinCheckButton(hwnd, CBID_INACTIVETEXTCOLOR, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVETEXTCOLOR), TRUE);
            }
            else
              WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVETEXTCOLOR), FALSE);
            if (pPluginData->flInactive & FLAG_3DTEXT)
                WinCheckButton(hwnd, CBID_INACTIVE3DTEXT, TRUE);

            //disable these for now.... they aren't implemented yet!          
            WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEFONT), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVEFONT), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEFONT), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVEFONT), FALSE);

            WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lActiveShadowColor),
                            (PVOID) & (pPluginData->lActiveShadowColor));
            WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR),
                            PP_BACKGROUNDCOLOR,
                          (ULONG) sizeof(pPluginData->lInactiveShadowColor),
                            (PVOID) & (pPluginData->lInactiveShadowColor));

            WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVETEXTCOLOR),
                            PP_BACKGROUNDCOLOR,
                          (ULONG) sizeof(pPluginData->lActiveTextColor),
                            (PVOID) & (pPluginData->lActiveTextColor));

            WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVETEXTCOLOR),
                            PP_BACKGROUNDCOLOR,
                          (ULONG) sizeof(pPluginData->lInactiveTextColor),
                            (PVOID) & (pPluginData->lInactiveTextColor));

            if (pPluginData->bActiveEnabled)
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVE3DTEXT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, GBID_ACTIVETEXTALIGNMENT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVELEFTTEXT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVECENTERTEXT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVERIGHTTEXT), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVE3DTEXT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, GBID_ACTIVETEXTALIGNMENT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVELEFTTEXT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVECENTERTEXT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVERIGHTTEXT), FALSE);
            }
            if (pPluginData->bInactiveEnabled)
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVE3DTEXT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, GBID_INACTIVETEXTALIGNMENT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVELEFTTEXT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVECENTERTEXT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVERIGHTTEXT), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVE3DTEXT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, GBID_INACTIVETEXTALIGNMENT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVELEFTTEXT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVECENTERTEXT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVERIGHTTEXT), FALSE);
            }

            WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVETEXTOFFSET), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVETEXTOFFSET), SPBM_SETCURRENTVALUE, 
                       (MPARAM)pPluginData->lActiveTextOffset, (MPARAM)NULL);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVETEXTOFFSET), SPBM_SETLIMITS, (MPARAM)99, (MPARAM)0);
            WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVETEXTOFFSET), SPBM_SETCURRENTVALUE, 
                       (MPARAM)pPluginData->lActiveTextOffset, (MPARAM)NULL);

            if  (pPluginData->sActiveTextAlignment == MANUAL_TEXT)
            {
                WinCheckButton(hwnd, CBID_ACTIVETEXTOFFSET, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVETEXTOFFSET), TRUE);

                WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVELEFTTEXT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVECENTERTEXT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVERIGHTTEXT), FALSE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_ACTIVETEXTOFFSET, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVETEXTOFFSET), FALSE);
            }
            if  (pPluginData->sInactiveTextAlignment == MANUAL_TEXT)
            {
                WinCheckButton(hwnd, CBID_INACTIVETEXTOFFSET, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVETEXTOFFSET), TRUE);

                WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVELEFTTEXT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVECENTERTEXT), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVERIGHTTEXT), FALSE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_INACTIVETEXTOFFSET, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVETEXTOFFSET), FALSE);
            }

        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
            case PBID_ACTIVESHADOWCOLOR:
            case PBID_INACTIVESHADOWCOLOR:
            case PBID_ACTIVETEXTCOLOR:
            case PBID_INACTIVETEXTCOLOR:
                {
                  // Open the colorpalette 
                  if((hObject=WinQueryObject("<WP_HIRESCLRPAL>"))!=NULLHANDLE) {
                    WinOpenObject(hObject, OPEN_DEFAULT ,TRUE);
                  }
                  else {//Error, can't open the palette
                    /*  Show an error msg.						   */
                    WinMessageBox(HWND_DESKTOP,
                                  hwnd,         
                                  "Can't open color palette",          
                                  "Gradient plugin",                      
                                  12345,            /* Window ID */
                                  MB_OK |
                                  MB_MOVEABLE |
                                  MB_ICONEXCLAMATION |
                                  MB_DEFBUTTON1);                  /* Style     */
                  }


                }
                break;

                case PBID_OK:
                {
                    ULONG attrFound;

                    if ((pPluginData = (PLUGINSHARE *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        //error message..
                        WinDismissDlg(hwnd, PBID_CANCEL);
                    }

                    /* Save the selected colors */
                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lActiveShadowColor), QPF_PURERGBCOLOR);

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lInactiveShadowColor), QPF_PURERGBCOLOR);

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_ACTIVETEXTCOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lActiveTextColor), QPF_PURERGBCOLOR);

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_INACTIVETEXTCOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                      &(pPluginData->lInactiveTextColor), QPF_PURERGBCOLOR);

                    /* Save offset settings */
                    if ( WinQueryButtonCheckstate(hwnd, CBID_ACTIVETEXTOFFSET) )
                    {
                        pPluginData->sActiveTextAlignment = MANUAL_TEXT;
                        WinSendMsg(WinWindowFromID(hwnd, SPBID_ACTIVETEXTOFFSET), SPBM_QUERYVALUE, 
                                   MPFROMP(&(pPluginData->lActiveTextOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                    }
                    else if ( WinQueryButtonCheckstate(hwnd, RBID_ACTIVECENTERTEXT) )
                        pPluginData->sActiveTextAlignment = CENTER_TEXT;
                    else if ( WinQueryButtonCheckstate(hwnd, RBID_ACTIVERIGHTTEXT) )
                        pPluginData->sActiveTextAlignment = RIGHT_TEXT;
                    else
                        pPluginData->sActiveTextAlignment = LEFT_TEXT;

                    if ( WinQueryButtonCheckstate(hwnd, CBID_INACTIVETEXTOFFSET) )
                    {
                        pPluginData->sInactiveTextAlignment = MANUAL_TEXT;
                        WinSendMsg(WinWindowFromID(hwnd, SPBID_INACTIVETEXTOFFSET), SPBM_QUERYVALUE, 
                                   MPFROMP(&(pPluginData->lInactiveTextOffset)), MPFROM2SHORT(0, SPBQ_DONOTUPDATE));
                    }
                    else if ( WinQueryButtonCheckstate(hwnd, RBID_INACTIVECENTERTEXT) )
                        pPluginData->sInactiveTextAlignment = CENTER_TEXT;
                    else if ( WinQueryButtonCheckstate(hwnd, RBID_INACTIVERIGHTTEXT) )
                        pPluginData->sInactiveTextAlignment = RIGHT_TEXT;
                    else
                        pPluginData->sInactiveTextAlignment = LEFT_TEXT;


                    if ( WinQueryButtonCheckstate(hwnd, CBID_ACTIVE3DTEXT) )
                        pPluginData->flActive = FLAG_3DTEXT;
                    else
                        pPluginData->flActive = 0;

                    if ( WinQueryButtonCheckstate(hwnd, CBID_INACTIVE3DTEXT) )
                        pPluginData->flInactive = FLAG_3DTEXT;
                    else
                        pPluginData->flInactive = 0;

                    if ( WinQueryButtonCheckstate(hwnd, CBID_ACTIVETEXTCOLOR) )
                      pPluginData->flActive |= FLAG_CUSTOM_COLOR;
                
                    if ( WinQueryButtonCheckstate(hwnd, CBID_INACTIVETEXTCOLOR) )
                      pPluginData->flInactive |= FLAG_CUSTOM_COLOR;
                
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
            sControlId = SHORT1FROMMP(mp1);
            switch (sControlId)
            {
                case CBID_ACTIVETEXTOFFSET:
                case CBID_INACTIVETEXTOFFSET:
                {
                    if (sControlId == CBID_ACTIVETEXTOFFSET)
                    {
                        // if 3dtext
                        if (!WinQueryButtonCheckstate(hwnd, CBID_ACTIVETEXTOFFSET))
                        {
                            // and check checkbox
                            WinCheckButton(hwnd, CBID_ACTIVETEXTOFFSET, TRUE);
                            WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVETEXTOFFSET), TRUE);

                            WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVELEFTTEXT), FALSE);
                            WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVECENTERTEXT), FALSE);
                            WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVERIGHTTEXT), FALSE);

                        }
                        else    //no 3dtext
                        {
                            // and uncheck button
                            WinCheckButton(hwnd, CBID_ACTIVETEXTOFFSET, FALSE);
                            WinEnableWindow(WinWindowFromID(hwnd, SPBID_ACTIVETEXTOFFSET), FALSE);

                            WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVELEFTTEXT), TRUE);
                            WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVECENTERTEXT), TRUE);
                            WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVERIGHTTEXT), TRUE);

                        }
                    }
                    else
                    {
                        // if 3dtext
                        if (!WinQueryButtonCheckstate(hwnd, CBID_INACTIVETEXTOFFSET))
                        {
                            // and check checkbox
                            WinCheckButton(hwnd, CBID_INACTIVETEXTOFFSET, TRUE);
                            WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVETEXTOFFSET), TRUE);

                            WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVELEFTTEXT), FALSE);
                            WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVECENTERTEXT), FALSE);
                            WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVERIGHTTEXT), FALSE);

                        }
                        else    //no 3dtext
                        {
                            // and uncheck button
                            WinCheckButton(hwnd, CBID_INACTIVETEXTOFFSET, FALSE);
                            WinEnableWindow(WinWindowFromID(hwnd, SPBID_INACTIVETEXTOFFSET), FALSE);

                            WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVELEFTTEXT), TRUE);
                            WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVECENTERTEXT), TRUE);
                            WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVERIGHTTEXT), TRUE);

                        }
                    }
                }
                break;

                case CBID_ACTIVE3DTEXT:
                case CBID_INACTIVE3DTEXT:
                case CBID_ACTIVETEXTCOLOR:              
                case CBID_INACTIVETEXTCOLOR: 
                {
                  if (sControlId == CBID_ACTIVETEXTCOLOR)
                    {
                        // if custom color
                        if (!WinQueryButtonCheckstate(hwnd, CBID_ACTIVETEXTCOLOR))
                        {
                            // and check checkbox
                            WinCheckButton(hwnd, CBID_ACTIVETEXTCOLOR, TRUE);
                            WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVETEXTCOLOR), TRUE);
                        }
                        else    //custom color
                        {
                            // and uncheck button
                            WinCheckButton(hwnd, CBID_ACTIVETEXTCOLOR, FALSE);
                            WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVETEXTCOLOR), FALSE);
                        }
                    }
                  else if (sControlId == CBID_INACTIVETEXTCOLOR)
                    {
                        // if default color
                        if (!WinQueryButtonCheckstate(hwnd, CBID_INACTIVETEXTCOLOR))
                        {
                            // and check checkbox
                            WinCheckButton(hwnd, CBID_INACTIVETEXTCOLOR, TRUE);
                            WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVETEXTCOLOR), TRUE);
                        }
                        else    //custom color
                        {
                            // and uncheck button
                            WinCheckButton(hwnd, CBID_INACTIVETEXTCOLOR, FALSE);
                            WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVETEXTCOLOR), FALSE);
                        }
                    }
                    else if (sControlId == CBID_ACTIVE3DTEXT)
                    {
                        // if 3dtext
                        if (!WinQueryButtonCheckstate(hwnd, CBID_ACTIVE3DTEXT))
                        {
                            // and check checkbox
                            WinCheckButton(hwnd, CBID_ACTIVE3DTEXT, TRUE);
                        }
                        else    //no 3dtext
                        {
                            // and uncheck button
                            WinCheckButton(hwnd, CBID_ACTIVE3DTEXT, FALSE);
                        }
                    }
                    else
                    {
                        // if 3dtext
                        if (!WinQueryButtonCheckstate(hwnd, CBID_INACTIVE3DTEXT))
                        {
                            // and check checkbox
                            WinCheckButton(hwnd, CBID_INACTIVE3DTEXT, TRUE);
                        }
                        else    //no 3dtext
                        {
                            // and uncheck button
                            WinCheckButton(hwnd, CBID_INACTIVE3DTEXT, FALSE);
                        }
                    }
                }
                break;

                case CBID_ACTIVEENABLED:
                {
                    // if Enabled
                    if (!WinQueryButtonCheckstate(hwnd, CBID_ACTIVEENABLED))
                    {
                        // check button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);

                        WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVETEXTCOLOR), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVETEXTCOLOR), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVE3DTEXT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR), TRUE);
                        //WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVEFONT), TRUE);
                        //WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEFONT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, GBID_ACTIVETEXTALIGNMENT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVELEFTTEXT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVECENTERTEXT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVERIGHTTEXT), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVETEXTCOLOR), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVETEXTCOLOR), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVE3DTEXT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR), FALSE);
                        //WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVEFONT), FALSE);
                        //WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEFONT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, GBID_ACTIVETEXTALIGNMENT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVELEFTTEXT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVECENTERTEXT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_ACTIVERIGHTTEXT), FALSE);
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

                        WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVETEXTCOLOR), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVETEXTCOLOR), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVE3DTEXT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR), TRUE);
                        //WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVEFONT), TRUE);
                        //WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEFONT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, GBID_INACTIVETEXTALIGNMENT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVELEFTTEXT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVECENTERTEXT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVERIGHTTEXT), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVETEXTCOLOR), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVETEXTCOLOR), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVE3DTEXT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR), FALSE);
                        //WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVEFONT), FALSE);
                        //WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEFONT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, GBID_INACTIVETEXTALIGNMENT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVELEFTTEXT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVECENTERTEXT), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, RBID_INACTIVERIGHTTEXT), FALSE);
                    }
                }
                break;

            }                   //end switch

            return ((MRESULT) 0);
        }
    }                           //end switch

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
        return FALSE; //not enabled for any state

    if (pPluginData->bActiveEnabled)
    {
        AddLongAttribute(retStr, "ActiveTextColor", pPluginData->lActiveTextColor);
        AddLongAttribute(retStr, "ActiveShadowColor", pPluginData->lActiveShadowColor);

        if (pPluginData->flActive & FLAG_3DTEXT)
            AddStringAttribute(retStr, "ActiveShadowEnabled", "TRUE");
        else
            AddStringAttribute(retStr, "ActiveShadowEnabled", "FALSE");

        if (pPluginData->flActive & FLAG_CUSTOM_COLOR)
            AddStringAttribute(retStr, "ActiveCustomTextColor", "TRUE");
        else
            AddStringAttribute(retStr, "ActiveCustomTextColor", "FALSE");

        if (pPluginData->sActiveTextAlignment == MANUAL_TEXT)
        {
            AddStringAttribute(retStr, "ActiveAlignment", "manual");
            AddLongAttribute(retStr, "ActiveTextOffset", pPluginData->lActiveTextOffset);
        }
        else if (pPluginData->sActiveTextAlignment == CENTER_TEXT)
            AddStringAttribute(retStr, "ActiveAlignment", "center");
        else if (pPluginData->sActiveTextAlignment == RIGHT_TEXT)
            AddStringAttribute(retStr, "ActiveAlignment", "right");
        else
            AddStringAttribute(retStr, "ActiveAlignment", "left");
    }

    if (pPluginData->bInactiveEnabled)
    {
        AddLongAttribute(retStr, "InactiveTextColor", pPluginData->lInactiveTextColor);
        AddLongAttribute(retStr, "InactiveShadowColor", pPluginData->lInactiveShadowColor);

        if (pPluginData->flInactive & FLAG_3DTEXT)
            AddStringAttribute(retStr, "InactiveShadowEnabled", "TRUE");
        else
            AddStringAttribute(retStr, "InactiveShadowEnabled", "FALSE");

        if (pPluginData->flInactive & FLAG_CUSTOM_COLOR)
          AddStringAttribute(retStr, "InactiveCustomTextColor", "TRUE");
        else
          AddStringAttribute(retStr, "InactiveCustomTextColor", "FALSE");

        if (pPluginData->sInactiveTextAlignment == MANUAL_TEXT)
        {
            AddStringAttribute(retStr, "InactiveAlignment", "manual");
            AddLongAttribute(retStr, "InactiveTextOffset", pPluginData->lInactiveTextOffset);
        }
        else if (pPluginData->sInactiveTextAlignment == CENTER_TEXT)
            AddStringAttribute(retStr, "InactiveAlignment", "center");
        else if (pPluginData->sInactiveTextAlignment == RIGHT_TEXT)
            AddStringAttribute(retStr, "InactiveAlignment", "right");
        else
            AddStringAttribute(retStr, "InactiveAlignment", "left");
    }

    return (TRUE);
};


