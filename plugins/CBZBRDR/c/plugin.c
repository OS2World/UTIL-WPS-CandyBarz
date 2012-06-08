/* $Id: plugin.c,v 1.3 2000/04/15 14:55:55 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "plugin.h"

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pluginInfo)
{
    strcpy(pluginInfo->PluginName, "CandyBarZ 3D-Borders");
    strcpy(pluginInfo->PluginAuthor, "CandyBarZ Team @ OS/2 Netlabs");
    strcpy(pluginInfo->PluginDate, "1 April 1999");
    pluginInfo->lWindowTypes = CBZ_TITLEBAR | CBZ_PUSHBUTTON;
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
        strcat(szReadName, "_CBZBRDRPlugin");
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
                pPluginData->lActiveDarkColor = 121 * 65536 + 127;
                pPluginData->lActiveLightColor = 255 * (65536 + 1);
                pPluginData->lInactiveDarkColor = 100 * (65536 + 256 + 1);
                pPluginData->lInactiveLightColor = 204 * (65536 + 256 + 1);
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
            strcat(szReadName, "_CBZBRDRPlugin");
            if (!PrfQueryProfileData(hIni,
                                     "UserOptionsData",
                                     szReadName,
                                     pPluginShare,
                                     &szData))
            {
                pPluginShare->lActiveDarkColor = 121 * 65536 + 127;
                pPluginShare->lActiveLightColor = 255 * (65536 + 1);
                pPluginShare->lInactiveDarkColor = 100 * (65536 + 256 + 1);
                pPluginShare->lInactiveLightColor = 204 * (65536 + 256 + 1);
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
        pPluginData->lActiveDarkColor = pPluginShare->lActiveDarkColor;
        pPluginData->lActiveLightColor = pPluginShare->lActiveLightColor;
        pPluginData->lInactiveDarkColor = pPluginShare->lInactiveDarkColor;
        pPluginData->lInactiveLightColor = pPluginShare->lInactiveLightColor;
        pPluginData->bActiveEnabled = pPluginShare->bActiveEnabled;
        pPluginData->bInactiveEnabled = pPluginShare->bInactiveEnabled;
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
            if (strcmp(kv[i].key, "ActiveDarkColor") == 0)
            {
                //StringToLong() returns 0L on error...which is as good as any value for an error condition?
                pPluginShare->lActiveDarkColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveLightColor") == 0)
            {
                pPluginShare->lActiveLightColor = StringToLong(kv[i].value);
            }
        }                       //end bInactive if condition

        if (pPluginShare->bInactiveEnabled)
        {
            if (strcmp(kv[i].key, "InactiveDarkColor") == 0)
            {
                pPluginShare->lInactiveDarkColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveLightColor") == 0)
            {
                pPluginShare->lInactiveLightColor = StringToLong(kv[i].value);
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
        strcat(szSaveName, "_CBZBRDRPlugin");
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
        strcat(szSaveName, "_CBZBRDRPlugin");

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
    HAB hab = WinQueryAnchorBlock(pPaint->hwnd);
    PLUGINSHARE *pPluginData;
    RECTL rectlBorder, rectlIntersect;

    pPluginData = (PLUGINSHARE *) pData;

    if (pPluginData == NULL)
        return (FALSE);

    if (((sActive == TRUE) && !(pPluginData->bActiveEnabled)) ||
            ((sActive == FALSE) && !(pPluginData->bInactiveEnabled)))
        return (TRUE);

    // if we're drawing the border, set border rectangle and update if necessary
    rectlBorder.xLeft = pPaint->rectlWindow.xLeft;
    rectlBorder.xRight = pPaint->rectlWindow.xRight;
    rectlBorder.yTop = pPaint->rectlWindow.yTop;
    rectlBorder.yBottom = pPaint->rectlWindow.yTop - 1;
    if (WinIntersectRect(hab,
                         &(rectlIntersect),
                         &(rectlBorder),
                         &(pPaint->rectlUpdate)))
    {
        if (sActive == TRUE)
        {
            if (!WinFillRect(pPaint->hps, &(rectlIntersect), pPluginData->lActiveDarkColor))
                return (FALSE);
        }
        else
        {
            if (!WinFillRect(pPaint->hps, &(rectlIntersect), pPluginData->lInactiveDarkColor))
                return (FALSE);
        }
    }
    rectlBorder.xLeft = pPaint->rectlWindow.xRight - 1;
    rectlBorder.yBottom = pPaint->rectlWindow.yBottom;
    if (WinIntersectRect(hab,
                         &(rectlIntersect),
                         &(rectlBorder),
                         &(pPaint->rectlUpdate)))
    {
        if (sActive == TRUE)
        {
            if (!WinFillRect(pPaint->hps, &(rectlIntersect), pPluginData->lActiveLightColor))
                return (FALSE);
        }
        else
        {
            if (!WinFillRect(pPaint->hps, &(rectlIntersect), pPluginData->lInactiveLightColor))
                return (FALSE);
        }
    }
    rectlBorder.xLeft = pPaint->rectlWindow.xLeft;
    rectlBorder.yTop = pPaint->rectlWindow.yBottom + 1;
    if (WinIntersectRect(hab,
                         &(rectlIntersect),
                         &(rectlBorder),
                         &(pPaint->rectlUpdate)))
    {
        if (sActive == TRUE)
        {
            if (!WinFillRect(pPaint->hps, &(rectlIntersect), pPluginData->lActiveLightColor))
                return (FALSE);
        }
        else
        {
            if (!WinFillRect(pPaint->hps, &(rectlIntersect), pPluginData->lInactiveLightColor))
                return (FALSE);
        }
    }
    rectlBorder.xRight = pPaint->rectlWindow.xLeft + 1;
    rectlBorder.yTop = pPaint->rectlWindow.yTop;
    if (WinIntersectRect(hab,
                         &(rectlIntersect),
                         &(rectlBorder),
                         &(pPaint->rectlUpdate)))
    {
        if (sActive == TRUE)
        {
            if (!WinFillRect(pPaint->hps, &(rectlIntersect), pPluginData->lActiveDarkColor))
                return (FALSE);
        }
        else
        {
            if (!WinFillRect(pPaint->hps, &(rectlIntersect), pPluginData->lInactiveDarkColor))
                return (FALSE);
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
    return FALSE;  //returning false indicates we haven't processed the message!
}

//A Dlg procedure if the plugin has selectable settings.
MRESULT EXPENTRY CBZPluginSetupDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    short sControlId;
    PLUGINSHARE *pPluginData;
//   CLRCHANGE      clrCh;
 	HOBJECT  hObject;

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

            WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVEDARKCOLOR),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lActiveDarkColor),
                            (PVOID) & (pPluginData->lActiveDarkColor));
            WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVELIGHTCOLOR),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lActiveLightColor),
                            (PVOID) & (pPluginData->lActiveLightColor));
            WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVEDARKCOLOR),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lInactiveDarkColor),
                            (PVOID) & (pPluginData->lInactiveDarkColor));
            WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVELIGHTCOLOR),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lInactiveLightColor),
                            (PVOID) & (pPluginData->lInactiveLightColor));

            if (pPluginData->bActiveEnabled)
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEDARKCOLOR), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVELIGHTCOLOR), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEDARKCOLOR), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVELIGHTCOLOR), FALSE);
            }
            if (pPluginData->bInactiveEnabled)
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEDARKCOLOR), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVELIGHTCOLOR), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEDARKCOLOR), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVELIGHTCOLOR), FALSE);
            }

        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
                case PBID_ACTIVEDARKCOLOR:
                case PBID_ACTIVELIGHTCOLOR:
                case PBID_INACTIVEDARKCOLOR:
                case PBID_INACTIVELIGHTCOLOR:
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
                  /*
                    long lColor;
                    //Popup Color Change Dialog to get new color!
                    if (sControlId == PBID_ACTIVEDARKCOLOR)
                    {
                    if (CBZGetColor(hwnd, &lColor))
                    {
                    WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVEDARKCOLOR),
                    PP_BACKGROUNDCOLOR,
                    (ULONG) sizeof(lColor),
                    (PVOID) & lColor);
                    }
                    }
                    else if (sControlId == PBID_ACTIVELIGHTCOLOR)
                    {
                    if (CBZGetColor(hwnd, &lColor))
                    {
                    WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVELIGHTCOLOR),
                    PP_BACKGROUNDCOLOR,
                    (ULONG) sizeof(lColor),
                    (PVOID) & lColor);
                    }
                    }
                    else if (sControlId == PBID_INACTIVEDARKCOLOR)
                    {
                    if (CBZGetColor(hwnd, &lColor))
                    {
                    WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVEDARKCOLOR),
                    PP_BACKGROUNDCOLOR,
                    (ULONG) sizeof(lColor),
                    (PVOID) & lColor);
                    }
                    }
                    else if (sControlId == PBID_INACTIVELIGHTCOLOR)
                    {
                    if (CBZGetColor(hwnd, &lColor))
                    {
                    WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVELIGHTCOLOR),
                    PP_BACKGROUNDCOLOR,
                    (ULONG) sizeof(lColor),
                    (PVOID) & lColor);
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

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_ACTIVEDARKCOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lActiveDarkColor), QPF_PURERGBCOLOR);


                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_ACTIVELIGHTCOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lActiveLightColor), QPF_PURERGBCOLOR);

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_INACTIVEDARKCOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lInactiveDarkColor), QPF_PURERGBCOLOR);

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_INACTIVELIGHTCOLOR),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lInactiveLightColor), QPF_PURERGBCOLOR);


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
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEDARKCOLOR), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVELIGHTCOLOR), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEDARKCOLOR), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVELIGHTCOLOR), FALSE);
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
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEDARKCOLOR), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVELIGHTCOLOR), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEDARKCOLOR), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVELIGHTCOLOR), FALSE);
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
        return FALSE; //Not enabled for any state

    if (pPluginData->bActiveEnabled)
    {
        AddLongAttribute(retStr, "ActiveDarkColor", pPluginData->lActiveDarkColor);
        AddLongAttribute(retStr, "ActiveLightColor", pPluginData->lActiveLightColor);
    }

    if (pPluginData->bInactiveEnabled)
    {
        AddLongAttribute(retStr, "InactiveDarkColor", pPluginData->lInactiveDarkColor);
        AddLongAttribute(retStr, "InactiveLightColor", pPluginData->lInactiveLightColor);
    }

    return (TRUE);
};


