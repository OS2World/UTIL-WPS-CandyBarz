/* $Id: plugin.c,v 1.4 2000/08/29 18:10:37 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "plugin.h"

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pluginInfo)
{
    strcpy(pluginInfo->PluginName, "Mac-Like Lines");
    strcpy(pluginInfo->PluginAuthor, "Eric Norman <warped@cs.montana.edu>");
    strcpy(pluginInfo->PluginDate, "Jan. 27, 1999");
    pluginInfo->lWindowTypes = CBZ_TITLEBAR;
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
        strcat(szReadName, "_CBZLINESPlugin");
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
                pPluginData->lActiveLineColor = 121 * 65536 + 127;
                pPluginData->lActiveShadowColor = 255 * (65536 + 1);
                pPluginData->lInactiveLineColor = 100 * (65536 + 256 + 1);
                pPluginData->lInactiveShadowColor = 204 * (65536 + 256 + 1);
                pPluginData->bActiveEnabled = TRUE;
                pPluginData->bInactiveEnabled = TRUE;
                pPluginData->lLineStyle = 7;

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
            strcat(szReadName, "_CBZLINESPlugin");
            if (!PrfQueryProfileData(hIni,
                                     "UserOptionsData",
                                     szReadName,
                                     pPluginShare,
                                     &szData))
            {
                pPluginShare->lActiveLineColor = 121 * 65536 + 127;
                pPluginShare->lActiveShadowColor = 255 * (65536 + 1);
                pPluginShare->lInactiveLineColor = 100 * (65536 + 256 + 1);
                pPluginShare->lInactiveShadowColor = 204 * (65536 + 256 + 1);
                pPluginShare->bActiveEnabled = TRUE;
                pPluginShare->bInactiveEnabled = TRUE;
                pPluginShare->lLineStyle = 7;

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
        pPluginData->lActiveLineColor = pPluginShare->lActiveLineColor;
        pPluginData->lActiveShadowColor = pPluginShare->lActiveShadowColor;
        pPluginData->lInactiveLineColor = pPluginShare->lInactiveLineColor;
        pPluginData->lInactiveShadowColor = pPluginShare->lInactiveShadowColor;
        pPluginData->bActiveEnabled = pPluginShare->bActiveEnabled;
        pPluginData->bInactiveEnabled = pPluginShare->bInactiveEnabled;
        pPluginData->lLineStyle = pPluginShare->lLineStyle;
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
            if (strcmp(kv[i].key, "ActiveLineColor") == 0)
            {
                //StringToLong() returns 0L on error...which is as good as any value for an error condition?
                pPluginShare->lActiveLineColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveShadowColor") == 0)
            {
                pPluginShare->lActiveShadowColor = StringToLong(kv[i].value);
            }
        }                       //end bInactive if condition

        if (pPluginShare->bInactiveEnabled)
        {
            if (strcmp(kv[i].key, "InactiveLineColor") == 0)
            {
                //StringToLong() returns 0L on error...which is as good as any value for an error condition?
                pPluginShare->lInactiveLineColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveShadowColor") == 0)
            {
                pPluginShare->lInactiveShadowColor = StringToLong(kv[i].value);
            }
        }                       //end bInactive if condition

        if (strcmp(kv[i].key, "LineStyle") == 0)
        {
            //StringToLong() returns 0L on error...which is as good as any value for an error condition?
            pPluginShare->lLineStyle = StringToLong(kv[i].value);
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
        strcat(szSaveName, "_CBZLINESPlugin");
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
        strcat(szSaveName, "_CBZLINESPlugin");

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
    POINTL point;               //should allocate all this data dynamically, to keep it off

    int i, y;                   // the stack!!!

    FONTMETRICS font;
    PLUGINSHARE *pPluginData;

    pPluginData = (PLUGINSHARE *) pData;

    if (pPluginData == NULL)
        return (FALSE);

    if (((sActive == TRUE) && !(pPluginData->bActiveEnabled)) ||
            ((sActive == FALSE) && !(pPluginData->bInactiveEnabled)))
        return (TRUE);

    GpiQueryFontMetrics(pPaint->hpsWin, sizeof(font), &(font));

    GpiSetLineType(pPaint->hps, pPluginData->lLineStyle);

    // if no text
    if (pPaint->rectlText.xLeft == pPaint->rectlText.xRight)
    {
        for (y = 4; y < pPaint->rectlWindow.yTop - 4; y += 3)
        {
            if (sActive == TRUE)
                GpiSetColor(pPaint->hps, pPluginData->lActiveShadowColor);
            else
                GpiSetColor(pPaint->hps, pPluginData->lInactiveShadowColor);
            point.x = pPaint->rectlWindow.xLeft + 6;
            point.y = y;
            GpiMove(pPaint->hps, &point);
            point.x = pPaint->rectlWindow.xRight - 5;
            GpiLine(pPaint->hps, &point);

            if (sActive == TRUE)
                GpiSetColor(pPaint->hps, pPluginData->lActiveLineColor);
            else
                GpiSetColor(pPaint->hps, pPluginData->lInactiveLineColor);
            point.x = pPaint->rectlWindow.xLeft + 5;
            point.y++;
            GpiMove(pPaint->hps, &point);
            point.x = pPaint->rectlWindow.xRight - 6;
            GpiLine(pPaint->hps, &point);
        }
    }
    else                        //text exists... draw around it.
    {
        if (pPaint->rectlWindow.xRight > (pPaint->rectlText.xRight + font.lAveCharWidth))
        {
            for (y = 4; y < pPaint->rectlWindow.yTop - 4; y += 3)
            {
                if (sActive == TRUE)
                    GpiSetColor(pPaint->hps, pPluginData->lActiveShadowColor);
                else
                    GpiSetColor(pPaint->hps, pPluginData->lInactiveShadowColor);
                point.x = pPaint->rectlText.xRight + 6;
                point.y = y;
                GpiMove(pPaint->hps, &point);
                point.x = pPaint->rectlWindow.xRight - 5;
                GpiLine(pPaint->hps, &point);

                if (sActive == TRUE)
                    GpiSetColor(pPaint->hps, pPluginData->lActiveLineColor);
                else
                    GpiSetColor(pPaint->hps, pPluginData->lInactiveLineColor);
                point.x = pPaint->rectlText.xRight + 5;
                point.y++;
                GpiMove(pPaint->hps, &point);
                point.x = pPaint->rectlWindow.xRight - 6;
                GpiLine(pPaint->hps, &point);
            }
        }

        if ((pPaint->rectlWindow.xLeft + font.lAveCharWidth) < (pPaint->rectlText.xLeft))
        {
            for (y = 4; y < pPaint->rectlWindow.yTop - 4; y += 3)
            {
                if (sActive == TRUE)
                    GpiSetColor(pPaint->hps, pPluginData->lActiveShadowColor);
                else
                    GpiSetColor(pPaint->hps, pPluginData->lInactiveShadowColor);
                point.x = pPaint->rectlWindow.xLeft + 6;
                point.y = y;
                GpiMove(pPaint->hps, &point);
                point.x = pPaint->rectlText.xLeft - 5;
                GpiLine(pPaint->hps, &point);

                if (sActive == TRUE)
                    GpiSetColor(pPaint->hps, pPluginData->lActiveLineColor);
                else
                    GpiSetColor(pPaint->hps, pPluginData->lInactiveLineColor);
                point.x = pPaint->rectlWindow.xLeft + 5;
                point.y++;
                GpiMove(pPaint->hps, &point);
                point.x = pPaint->rectlText.xLeft - 6;
                GpiLine(pPaint->hps, &point);
            }
        }
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
    return FALSE;//false indicates we haven't processed the message
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
            short sInsertItem;/*to insert in ListBox*/
            CHAR	*szLineStyles [] =
                  {"dotted","short-dashed","dash-dot",
                  "double-dotted","long-dashed","dash-double-dot",
                  "solid","invisible","alternate pels" };

            if ((pPluginData = (PLUGINSHARE *) mp2) == NULL)
            {
                //Error message..
                WinDismissDlg(hwnd, DID_ERROR);
                return (MRFROMLONG(FALSE));
            }
            WinSetWindowPtr(hwnd, QWL_USER, pPluginData);  // store window words

            WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVELINECOLOR),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lActiveLineColor),
                            (PVOID) & (pPluginData->lActiveLineColor));
            WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lActiveShadowColor),
                            (PVOID) & (pPluginData->lActiveShadowColor));
            WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVELINECOLOR),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lInactiveLineColor),
                            (PVOID) & (pPluginData->lInactiveLineColor));
            WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR),
                            PP_BACKGROUNDCOLOR,
                          (ULONG) sizeof(pPluginData->lInactiveShadowColor),
                            (PVOID) & (pPluginData->lInactiveShadowColor));

            if (pPluginData->bActiveEnabled)
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVELINECOLOR), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVELINECOLOR), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR), FALSE);
            }
            if (pPluginData->bInactiveEnabled)
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVELINECOLOR), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVELINECOLOR), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR), FALSE);
            }
            /*there are 9 line styles*/
            for (sInsertItem=0; sInsertItem < 9; sInsertItem++)
            {
            WinSendDlgItemMsg (hwnd, LBID_LINESTYLE, LM_INSERTITEM, MPFROMSHORT (LIT_END), szLineStyles [sInsertItem]);
            }
            WinSendDlgItemMsg (hwnd, LBID_LINESTYLE, LM_SELECTITEM, MPFROMSHORT(pPluginData->lLineStyle-1), MPFROMSHORT(TRUE));

        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
                case PBID_ACTIVELINECOLOR:
                case PBID_ACTIVESHADOWCOLOR:
                case PBID_INACTIVELINECOLOR:
                case PBID_INACTIVESHADOWCOLOR:
                {
                  // Open the colorpalette 
                  hObject=WinQueryObject("<WP_HIRESCLRPAL>");
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
                  
                  /*    long lColor;
                        
                        if (sControlId == PBID_ACTIVELINECOLOR)
                        {
                        if (CBZGetColor(hwnd, &lColor))
                        {
                        WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVELINECOLOR),
                        PP_BACKGROUNDCOLOR,
                        (ULONG) sizeof(lColor),
                        (PVOID) &lColor );
                        }
                        }
                        else if (sControlId == PBID_ACTIVESHADOWCOLOR)
                        {
                        if (CBZGetColor(hwnd, &lColor))
                        {
                        WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR),
                        PP_BACKGROUNDCOLOR,
                        (ULONG) sizeof(lColor),
                        (PVOID) &lColor );
                        }
                        }
                        else if (sControlId == PBID_INACTIVELINECOLOR)
                        {
                        if (CBZGetColor(hwnd, &lColor))
                        {
                        WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVELINECOLOR),
                        PP_BACKGROUNDCOLOR,
                        (ULONG) sizeof(lColor),
                        (PVOID) &lColor );
                        }
                        }
                        else if (sControlId == PBID_INACTIVESHADOWCOLOR)
                        {
                        if (CBZGetColor(hwnd, &lColor))
                        {
                        WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR),
                        PP_BACKGROUNDCOLOR,
                        (ULONG) sizeof(lColor),
                        (PVOID) &lColor );
                        }
                        }*/
                }
                break;
                
                case PBID_OK:
                {
                    ULONG attrFound;

                    if ((pPluginData = (PLUGINSHARE *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        //error message here.
                        break;
                    }

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_ACTIVELINECOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lActiveLineColor), QPF_PURERGBCOLOR);


                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lActiveShadowColor), QPF_PURERGBCOLOR);

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_INACTIVELINECOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lInactiveLineColor), QPF_PURERGBCOLOR);

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lInactiveShadowColor), QPF_PURERGBCOLOR);


                    if ( WinQueryButtonCheckstate(hwnd, CBID_ACTIVEENABLED) )
                        pPluginData->bActiveEnabled = TRUE;
                    else
                        pPluginData->bActiveEnabled = FALSE;

                    if ( WinQueryButtonCheckstate(hwnd, CBID_INACTIVEENABLED) )
                        pPluginData->bInactiveEnabled = TRUE;
                    else
                        pPluginData->bInactiveEnabled = FALSE;
                    
	                 pPluginData->lLineStyle = 1 + (LONG) WinSendDlgItemMsg (hwnd, LBID_LINESTYLE, LM_QUERYSELECTION, (MPARAM) 0, NULL);

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
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVELINECOLOR), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVELINECOLOR), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVESHADOWCOLOR), FALSE);
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
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVELINECOLOR), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVELINECOLOR), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVESHADOWCOLOR), FALSE);
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
        return FALSE; //not enabled for any state!

    if (pPluginData->bActiveEnabled)
    {
        AddLongAttribute(retStr, "ActiveLineColor", pPluginData->lActiveLineColor);
        AddLongAttribute(retStr, "ActiveShadowColor", pPluginData->lActiveShadowColor);
    }

    if (pPluginData->bInactiveEnabled)
    {
        AddLongAttribute(retStr, "InactiveLineColor", pPluginData->lInactiveLineColor);
        AddLongAttribute(retStr, "InactiveShadowColor", pPluginData->lInactiveShadowColor);
    }
    AddLongAttribute(retStr, "LineStyle", pPluginData->lLineStyle);
    return (TRUE);
};


