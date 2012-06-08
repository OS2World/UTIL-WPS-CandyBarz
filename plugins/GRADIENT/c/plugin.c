/* $Id: plugin.c,v 1.3 2000/04/14 19:12:38 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#define INCL_WINWORKPLACE
#include "plugin.h"


//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pluginInfo)
{
    strcpy(pluginInfo->PluginName, "CandyBarZ Gradients");
    strcpy(pluginInfo->PluginAuthor, "CandyBarZ Team @ OS/2 Netlabs");
    strcpy(pluginInfo->PluginDate, "Jan. 27, 1999");
    pluginInfo->lWindowTypes = CBZ_TITLEBAR | CBZ_FRAME | CBZ_PUSHBUTTON;
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
        strcat(szReadName, "_GRADIENTPlugin");
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
                pPluginData->lActiveTopColor = 121 * 65536 + 127;
                pPluginData->lActiveBottomColor = 255 * (65536 + 1);
                pPluginData->lInactiveTopColor = 100 * (65536 + 256 + 1);
                pPluginData->lInactiveBottomColor = 204 * (65536 + 256 + 1);
                pPluginData->bActiveHorizontal = FALSE;
                pPluginData->bInactiveHorizontal = FALSE;
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
            strcat(szReadName, "_GRADIENTPlugin");
            if (!PrfQueryProfileData(hIni,
                                     "UserOptionsData",
                                     szReadName,
                                     pPluginShare,
                                     &szData))
            {
                pPluginShare->lActiveTopColor = 121 * 65536 + 127;
                pPluginShare->lActiveBottomColor = 255 * (65536 + 1);
                pPluginShare->lInactiveTopColor = 100 * (65536 + 256 + 1);
                pPluginShare->lInactiveBottomColor = 204 * (65536 + 256 + 1);
                pPluginShare->bActiveHorizontal = FALSE;
                pPluginShare->bInactiveHorizontal = FALSE;
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
        pPluginData->lActiveTopColor = pPluginShare->lActiveTopColor;
        pPluginData->lActiveBottomColor = pPluginShare->lActiveBottomColor;
        pPluginData->lInactiveTopColor = pPluginShare->lInactiveTopColor;
        pPluginData->lInactiveBottomColor = pPluginShare->lInactiveBottomColor;
        pPluginData->bActiveHorizontal = pPluginShare->bActiveHorizontal;
        pPluginData->bInactiveHorizontal = pPluginShare->bInactiveHorizontal;
        pPluginData->bActiveEnabled = pPluginShare->bActiveEnabled;
        pPluginData->bInactiveEnabled = pPluginShare->bInactiveEnabled;
    }                           //end else

    pPluginData->hbmActive = NULLHANDLE;
    pPluginData->hbmInactive = NULLHANDLE;
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
            if (strcmp(kv[i].key, "ActiveTopColor") == 0)
            {
                //StringToLong() returns 0 on error...which is as good as any value for an error condition?
                pPluginShare->lActiveTopColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveBottomColor") == 0)
            {
                pPluginShare->lActiveBottomColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "ActiveHorizontal") == 0)
            {
                if (strcmp(kv[i].value, "TRUE") == 0)
                    pPluginShare->bActiveHorizontal = TRUE;
                else
                    pPluginShare->bActiveHorizontal = FALSE;
            }
        }                       //end bInactive if condition

        if (pPluginShare->bInactiveEnabled)
        {
            if (strcmp(kv[i].key, "InactiveTopColor") == 0)
            {
                pPluginShare->lInactiveTopColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveBottomColor") == 0)
            {
                pPluginShare->lInactiveBottomColor = StringToLong(kv[i].value);
            }
            else if (strcmp(kv[i].key, "InactiveHorizontal") == 0)
            {
                if (strcmp(kv[i].value, "TRUE") == 0)
                    pPluginShare->bInactiveHorizontal = TRUE;
                else
                    pPluginShare->bInactiveHorizontal = FALSE;
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
        strcat(szSaveName, "_GRADIENTPlugin");
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
        strcat(szSaveName, "_GRADIENTPlugin");

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

    if (pPluginData == NULL)
        return (TRUE);

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
    PLUGINSHARE *pPluginData;

    pPluginData = (PLUGINSHARE *) pData;

    if (pPluginData == NULL)
        return (FALSE);

    if (((sActive == TRUE) && !(pPluginData->bActiveEnabled)) ||
            ((sActive == FALSE) && !(pPluginData->bInactiveEnabled)))
        return (TRUE);

    if (sActive == TRUE)
    {
        if (pPluginData->hbmActive == NULLHANDLE)
        {
            if (pPluginData->bActiveHorizontal)  //Horizontal Gradient.

                pPluginData->hbmActive = SetupBitmap(pPaint->hwnd, pPluginData->lActiveBottomColor,
                pPluginData->lActiveTopColor, pPaint->rectlWindow.xRight, 1);
            else                //Vertical Gradient

                pPluginData->hbmActive = SetupBitmap(pPaint->hwnd, pPluginData->lActiveTopColor,
                                                     pPluginData->lActiveBottomColor, 1, pPaint->rectlWindow.yTop);
        }
        //Draw gradient to the screen stretched, to fill the rectangle

        WinDrawBitmap(pPaint->hps, pPluginData->hbmActive, NULL, (PPOINTL)&(pPaint->rectlWindow), 0, 0, DBM_STRETCH);
//        TileGradient(pPaint, pPluginData->hbmActive);
    }
    else
    {
        if (pPluginData->hbmInactive == NULLHANDLE)
        {
            if (pPluginData->bInactiveHorizontal)  //Horizontal Gradient.

                pPluginData->hbmInactive = SetupBitmap(pPaint->hwnd, pPluginData->lInactiveBottomColor,
                                                       pPluginData->lInactiveTopColor, pPaint->rectlWindow.xRight, 1);
            else                //Vertical Gradient

                pPluginData->hbmInactive = SetupBitmap(pPaint->hwnd, pPluginData->lInactiveTopColor,
                                                       pPluginData->lInactiveBottomColor, 1, pPaint->rectlWindow.yTop);
        }
        //Draw gradient to the screen stretched, to fill the rectangle
        WinDrawBitmap(pPaint->hps, pPluginData->hbmInactive, NULL, (PPOINTL)&(pPaint->rectlWindow), 0, 0, DBM_STRETCH);
//        TileGradient(pPaint, pPluginData->hbmInactive);
    }

    return (TRUE);
}

//Called before the default wnd procedure if you wish to provide additional
//  functionality.  (i.e. modify what happens with mouse clicks, drag/drop, etc...
BOOL CBZPluginWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2,
                        PVOID pData, PFNWP pfnOrigWndProc, MRESULT *rc)
{
    PLUGINSHARE *pPluginData;

    pPluginData = (PLUGINSHARE *) pData;
    if (pPluginData == NULL)
        return FALSE;

    switch (msg)
    {
        case WM_WINDOWPOSCHANGED:
        {
            BITMAPINFOHEADER2 bif2;
            PSWP pSwp;

            pSwp = (PSWP)PVOIDFROMMP(mp1);

            if (pPluginData->hbmActive != NULLHANDLE)
            {
                GpiQueryBitmapInfoHeader(pPluginData->hbmActive, &bif2);

                if (bif2.cx < pSwp[0].cx)
                {
                    GpiDeleteBitmap(pPluginData->hbmActive);
                    pPluginData->hbmActive = NULLHANDLE;
                }
            }
            if (pPluginData->hbmInactive != NULLHANDLE)
            {
                GpiQueryBitmapInfoHeader(pPluginData->hbmInactive, &bif2);

                if (bif2.cx < pSwp[0].cx)
                {
                    GpiDeleteBitmap(pPluginData->hbmInactive);
                    pPluginData->hbmInactive = NULLHANDLE;
                }
            }
        }
        break;

    }
    return FALSE; //false indicates that we haven't processed the message!

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
                //Error msg..
                WinDismissDlg(hwnd, DID_ERROR);
                return (MRFROMLONG(FALSE));
            }
            WinSetWindowPtr(hwnd, QWL_USER, pPluginData);  // store window words

            WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVETOP),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lActiveTopColor),
                            (PVOID) & (pPluginData->lActiveTopColor));
            WinSetPresParam(WinWindowFromID(hwnd, PBID_ACTIVEBOTTOM),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lActiveBottomColor),
                            (PVOID) & (pPluginData->lActiveBottomColor));
            WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVETOP),
                            PP_BACKGROUNDCOLOR,
                            (ULONG) sizeof(pPluginData->lInactiveTopColor),
                            (PVOID) & (pPluginData->lInactiveTopColor));
            WinSetPresParam(WinWindowFromID(hwnd, PBID_INACTIVEBOTTOM),
                            PP_BACKGROUNDCOLOR,
                          (ULONG) sizeof(pPluginData->lInactiveBottomColor),
                            (PVOID) & (pPluginData->lInactiveBottomColor));

            if (pPluginData->bActiveEnabled)
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVETOP), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEBOTTOM), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVETOP), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEBOTTOM), FALSE);
            }
            if (pPluginData->bInactiveEnabled)
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVETOP), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEBOTTOM), TRUE);
            }
            else
            {
                WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVETOP), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEBOTTOM), FALSE);
            }

            WinCheckButton(hwnd, CBID_ACTIVEHORIZ, pPluginData->bActiveHorizontal);
            if (pPluginData->bActiveHorizontal)
            {
                WinSetWindowText(WinWindowFromID(hwnd, PBID_ACTIVETOP), "Left Color...");
                WinSetWindowText(WinWindowFromID(hwnd, PBID_ACTIVEBOTTOM), "Right Color...");
            }

            WinCheckButton(hwnd, CBID_INACTIVEHORIZ, pPluginData->bInactiveHorizontal);
            if (pPluginData->bInactiveHorizontal)
            {
                WinSetWindowText(WinWindowFromID(hwnd, PBID_INACTIVETOP), "Left Color...");
                WinSetWindowText(WinWindowFromID(hwnd, PBID_INACTIVEBOTTOM), "Right Color...");
            }

        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
              {
                case PBID_INACTIVEBOTTOM:
                case PBID_INACTIVETOP:
                case PBID_ACTIVEBOTTOM:
                case PBID_ACTIVETOP:
                {
                    long lColor;
   
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
                  break;
                }
                case PBID_OK:
                {
                    ULONG attrFound;

                    if ((pPluginData = (PLUGINSHARE *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        //Error message?
                        break;
                    }

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_ACTIVETOP),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lActiveTopColor), QPF_PURERGBCOLOR);

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_ACTIVEBOTTOM),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lActiveBottomColor), QPF_PURERGBCOLOR);

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_INACTIVETOP),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lInactiveTopColor), QPF_PURERGBCOLOR);

                    WinQueryPresParam(WinWindowFromID(hwnd, PBID_INACTIVEBOTTOM),
                                        PP_BACKGROUNDCOLOR, 0,
                                        &attrFound, sizeof(attrFound),
                                        &(pPluginData->lInactiveBottomColor), QPF_PURERGBCOLOR);


                    if ( WinQueryButtonCheckstate(hwnd, CBID_ACTIVEHORIZ) )
                        pPluginData->bActiveHorizontal = TRUE;
                    else
                        pPluginData->bActiveHorizontal = FALSE;

                    if ( WinQueryButtonCheckstate(hwnd, CBID_INACTIVEHORIZ) )
                        pPluginData->bInactiveHorizontal = TRUE;
                    else
                        pPluginData->bInactiveHorizontal = FALSE;

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

        case WM_CONTROL:
        {
            switch (SHORT1FROMMP(mp1))
            {
                case CBID_ACTIVEHORIZ:
                {
                    // if horizontal
                    if (!WinQueryButtonCheckstate(hwnd, CBID_ACTIVEHORIZ))
                    {
                        //check checkbox
                        WinCheckButton(hwnd, CBID_ACTIVEHORIZ, TRUE);
                        //update button text
                        WinSetWindowText(WinWindowFromID(hwnd, PBID_ACTIVETOP), "Left Color...");
                        WinSetWindowText(WinWindowFromID(hwnd, PBID_ACTIVEBOTTOM), "Right Color...");
                    }
                    else        // vertical
                    {
                        // and uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVEHORIZ, FALSE);
                        //update button text
                        WinSetWindowText(WinWindowFromID(hwnd, PBID_ACTIVETOP), "Top Color...");
                        WinSetWindowText(WinWindowFromID(hwnd, PBID_ACTIVEBOTTOM), "Bottom Color...");
                    }

                }               //end CHID_ACTIVEHORIZ

                break;

                case CBID_INACTIVEHORIZ:
                {
                    // if horizontal
                    if (!WinQueryButtonCheckstate(hwnd, CBID_INACTIVEHORIZ))
                    {
                        //check checkbox
                        WinCheckButton(hwnd, CBID_INACTIVEHORIZ, TRUE);
                        //update button text
                        WinSetWindowText(WinWindowFromID(hwnd, PBID_INACTIVETOP), "Left Color...");
                        WinSetWindowText(WinWindowFromID(hwnd, PBID_INACTIVEBOTTOM), "Right Color...");
                    }
                    else        // vertical

                    {
                        // and uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVEHORIZ, FALSE);
                        //update button text
                        WinSetWindowText(WinWindowFromID(hwnd, PBID_INACTIVETOP), "Top Color...");
                        WinSetWindowText(WinWindowFromID(hwnd, PBID_INACTIVEBOTTOM), "Bottom Color...");
                    }

                }               //end CHID_INACTIVEHORIZ

                break;

                case CBID_ACTIVEENABLED:
                {
                    // if Enabled
                    if (!WinQueryButtonCheckstate(hwnd, CBID_ACTIVEENABLED))
                    {
                        // check button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVETOP), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEBOTTOM), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVEHORIZ), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_ACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVETOP), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_ACTIVEBOTTOM), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_ACTIVEHORIZ), FALSE);
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
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVETOP), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEBOTTOM), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVEHORIZ), TRUE);
                    }
                    else
                    {
                        // uncheck button
                        WinCheckButton(hwnd, CBID_INACTIVEENABLED, FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVETOP), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INACTIVEBOTTOM), FALSE);
                        WinEnableWindow(WinWindowFromID(hwnd, CBID_INACTIVEHORIZ), FALSE);
                    }
                }
                break;

            }                   //end switch

            return ((MRESULT) 0);
        }                       //end WM_CONTROL

        break;

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
        return FALSE; //Not enabled for any state!

    if (pPluginData->bActiveEnabled)
    {
        AddLongAttribute(retStr, "ActiveTopColor", pPluginData->lActiveTopColor);
        AddLongAttribute(retStr, "ActiveBottomColor", pPluginData->lActiveBottomColor);

        if (pPluginData->bActiveHorizontal)
            AddStringAttribute(retStr, "ActiveHorizontal", "TRUE");
        else
            AddStringAttribute(retStr, "ActiveHorizontal", "FALSE");
    }

    if (pPluginData->bInactiveEnabled)
    {
        AddLongAttribute(retStr, "InactiveTopColor", pPluginData->lInactiveTopColor);
        AddLongAttribute(retStr, "InactiveBottomColor", pPluginData->lInactiveBottomColor);

        if (pPluginData->bInactiveHorizontal)
            AddStringAttribute(retStr, "InactiveHorizontal", "TRUE");
        else
            AddStringAttribute(retStr, "InactiveHorizontal", "FALSE");
    }

    return (TRUE);
};


HBITMAP SetupBitmap(HWND hwnd, long lColorTop, long lColorBottom, long lX, long lY)
{
    long lRedStart;
    long lGreenStart;
    long lBlueStart;
    long lRedEnd;
    long lGreenEnd;
    long lBlueEnd;
    RECTL rectl =
    {0};
    ULONG cbRow;
    ULONG cbData;
    void *pvImage;
    RGB2 rgb2;
    int i;
    int j;
    char *pvCurrent;
    HDC hdc;
    HAB hab;
    BITMAPINFOHEADER2 bif2;
    SIZEL slHPS =
    {0};
    HPS hps;
    HBITMAP hbm;

    if (hwnd)
    {
        hab = WinQueryAnchorBlock(hwnd);
    }
    else
    {
        hab = NULLHANDLE;
    }

    // break colors into components
    lRedStart = (lColorBottom & 0x00FF0000) >> 16;
    lGreenStart = (lColorBottom & 0x0000FF00) >> 8;
    lBlueStart = lColorBottom & 0x000000FF;

    lRedEnd = (lColorTop & 0x00FF0000) >> 16;
    lGreenEnd = (lColorTop & 0x0000FF00) >> 8;
    lBlueEnd = lColorTop & 0x000000FF;

    rectl.xRight = lX;
    rectl.yTop = lY;

    // allocate bitmap data
    cbRow = ((rectl.xRight * 24 + 31) / 32) * 4;
    cbData = cbRow * rectl.yTop;

    if (DosAllocMem(&pvImage, cbData, PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        return (NULLHANDLE);
    }

    if (lY > lX)                // vertical gradient.  it's based here on relative dimensions but doesn't have to be

    {
        // for each row, increment color...
        for (i = 0; i < rectl.yTop; i++)
        {
            rgb2.bRed = (lRedStart - (i * (lRedStart - lRedEnd) / (rectl.yTop - 1)) &
                         0x000000FF);
            rgb2.bGreen = (lGreenStart - (i * (lGreenStart - lGreenEnd) / (rectl.yTop - 1)) &
                           0x000000FF);
            rgb2.bBlue = lBlueStart - (i * (lBlueStart - lBlueEnd) / (rectl.yTop - 1)) &
                0x000000FF;

            pvCurrent = (char *) pvImage + i * cbRow;
            // and set entire row to that color
            for (j = 0; j < rectl.xRight; j++)
            {
                *pvCurrent++ = rgb2.bBlue;
                *pvCurrent++ = rgb2.bGreen;
                *pvCurrent++ = rgb2.bRed;
            }
        }
    }
    else                        // horizontal

    {
        for (i = 0; i < rectl.yTop; i++)  // in each row...

        {
            pvCurrent = (char *) pvImage + i * cbRow;

            // increment the color for each column
            for (j = 0; j < rectl.xRight; j++)
            {
                rgb2.bRed = (lRedStart - (j * (lRedStart - lRedEnd) / (rectl.xRight - 1)) &
                             0x000000FF);
                rgb2.bGreen = (lGreenStart - (j * (lGreenStart - lGreenEnd) / (rectl.xRight - 1)) &
                               0x000000FF);
                rgb2.bBlue = lBlueStart - (j * (lBlueStart - lBlueEnd) / (rectl.xRight - 1)) &
                    0x000000FF;

                *pvCurrent++ = rgb2.bBlue;
                *pvCurrent++ = rgb2.bGreen;
                *pvCurrent++ = rgb2.bRed;
            }
        }
    }

    // set up info for bitmap creations
    memset(&bif2, 0, sizeof(bif2));
    bif2.cbFix = sizeof(BITMAPINFOHEADER2);
    bif2.cx = rectl.xRight;
    bif2.cy = rectl.yTop;
    bif2.cBitCount = 24;
    bif2.cPlanes = 1;

    // PS dimensions
    slHPS.cx = rectl.xRight;
    slHPS.cy = rectl.yTop;

    // get device context
    hdc = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE);
    // create ps
    hps = GpiCreatePS(hab, hdc, &slHPS, PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC);
    // create bitmap
    hbm = GpiCreateBitmap(hps,
                          (PBITMAPINFOHEADER2) & (bif2),
                          (ULONG) CBM_INIT,
                          (PSZ) pvImage,
                          (PBITMAPINFO2) & (bif2));

    // clean up
    DosFreeMem(pvImage);
    GpiSetBitmap(hps, NULLHANDLE);
    GpiDestroyPS(hps);
    DevCloseDC(hdc);

    return (hbm);
}

// tiles an image across preview window
BOOL TileGradient(PSPAINT * pPaint, HBITMAP hbm)
{
    SIZEL slHps;
    BITMAPINFOHEADER2 bif2;
    BITMAPINFOHEADER2 bif;
    POINTL aptl[3];
    HDC hdc;
    HPS hps;
    HAB hab = WinQueryAnchorBlock(pPaint->hwnd);
    long starty;

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

    // initial blt location
    aptl[0].x = pPaint->rectlWindow.xLeft;
    aptl[0].y = pPaint->rectlWindow.yBottom;
    aptl[1].x = pPaint->rectlWindow.xRight;
    aptl[1].y = pPaint->rectlWindow.yTop;
    aptl[2].x = 0;
    aptl[2].y = 0;

    starty = aptl[0].y;
    while (aptl[0].x < aptl[1].x)
    {
        aptl[0].y = starty;

        while (aptl[0].y < aptl[1].y)
        {
            // blt
            GpiBitBlt(pPaint->hps, hps, 3L, aptl, ROP_SRCCOPY, BBO_IGNORE);

            // move
            aptl[0].y += bif2.cy;
        }
        aptl[0].x += bif2.cx;
    }

    // clean up
    GpiSetBitmap(hps, NULLHANDLE);
    GpiDestroyPS(hps);
    DevCloseDC(hdc);

    return (TRUE);
}
