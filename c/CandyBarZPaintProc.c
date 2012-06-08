/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#include "pcandybarz.h"

BOOL CandyBarZTBPaintProc(HWND hwnd, BOOL bEntire, BOOL bActive)
{
    CBZDATA     *pCBZData;
    CBZSHARE    *pCBZShare;
    PSPAINT     *pPaint;
    PSTBPAINT   *pTBPaint;
    char        *pchnl;
    BOOL        rc;
    int         i;
    ULONG       ulDataOffset;

    // alloc paint struct
    if (DosAllocMem((PPVOID) & pTBPaint,
                    sizeof(PSTBPAINT),
                    PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        return (FALSE);
    }
    memset(pTBPaint, 0, sizeof(PSTBPAINT));

    pTBPaint->hab = WinQueryAnchorBlock(hwnd);

    // alloc paint struct
    if (DosAllocMem((PPVOID) & pPaint,
                    sizeof(PSPAINT),
                    PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        DosFreeMem(pTBPaint);
        return (FALSE);
    }
    memset(pPaint, 0, sizeof(PSPAINT));

    //Text plugin needs this for quering presentation parameters.
    pPaint->hwnd = hwnd;

    // entire window
    WinQueryWindowRect(hwnd, &(pPaint->rectlWindow));

    // portion to update
    if (bEntire)
        WinQueryWindowRect(hwnd, &(pPaint->rectlUpdate));
    else
        WinQueryUpdateRect(hwnd, &(pPaint->rectlUpdate));

    // get PS
    pPaint->hpsWin = WinGetPS(hwnd);

    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE,  PAG_READ) != NO_ERROR)
    {
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }
    ulDataOffset = pCBZShare->ulDataOffset;
    DosFreeMem(pCBZShare);

    // get window data
    if ((pCBZData = WinQueryWindowPtr(CandyBarZGetTopLevelFrameHWND(hwnd), ulDataOffset)) == NULL
            || !pCBZData->bTitlebarEnabled  )
    {
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }


    //if there are no plugins defined, fallback to the default painting
    if (pCBZData->sTitlebarIndex == pCBZData->sFrameBackgroundIndex)
    {
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }

    WinQueryWindowText(hwnd, sizeof(pPaint->szTitleText), pPaint->szTitleText);

    pchnl = pPaint->szTitleText;
    while (*pchnl)
    {
        if (*pchnl == '\r' && *(pchnl + 1) == '\n')
        {
            *pchnl = ' ';
            memmove(pchnl + 1, pchnl + 2, strlen(pchnl + 2) + 1);
        }
        else if (*pchnl == '\n' || *pchnl == '\r')
        {
            *pchnl = ' ';
        }
        pchnl++;
    }

    //if WPSViewText has been defined, concatenate it to the TitleText
    if (strlen(/*pTBData*/pCBZData->szWPSViewText) > 0)
    {
        strcat(pPaint->szTitleText, " - ");
        strcat(pPaint->szTitleText, /*pTBData*/pCBZData->szWPSViewText);
    }

    //moved text rectangle define to the text plugin...might move it back here?!?

    // we will blt and draw gradient text into the hps associated with this dc
    if ((pTBPaint->hdc = DevOpenDC(pTBPaint->hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE))
            == NULLHANDLE)
    {
        DevCloseDC(pTBPaint->hdc);
        WinReleasePS(pPaint->hpsWin);
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }
    // create hps2 for hdc2.  hps for hdc depends on size of bitmap, so it's not created until
    // we know if it's active or not.  create for size of window, then we blt only portion
    // needed
    pTBPaint->slHPS.cx = pPaint->rectlWindow.xRight;
    pTBPaint->slHPS.cy = pPaint->rectlWindow.yTop;
    if ((pPaint->hps = GpiCreatePS(pTBPaint->hab,
                                   pTBPaint->hdc,
                                   &(pTBPaint->slHPS),
                          PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC))
            == NULLHANDLE)
    {
        DevCloseDC(pTBPaint->hdc);
        WinReleasePS(pPaint->hpsWin);
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }

    pTBPaint->bif2.cbFix = sizeof(BITMAPINFOHEADER2);
    pTBPaint->bif2.cx = pPaint->rectlWindow.xRight;
    pTBPaint->bif2.cy = pPaint->rectlWindow.yTop;
    pTBPaint->bif2.cBitCount = 24;
    pTBPaint->bif2.cPlanes = 1;
    if ((pTBPaint->hbmTemp = GpiCreateBitmap(pPaint->hps,  // create bitmap
                                    (PBITMAPINFOHEADER2) & (pTBPaint->bif2),
                                             (ULONG) 0,
                                             (PSZ) NULL,
                          (PBITMAPINFO2) & (pTBPaint->bif2))) == NULLHANDLE)
    {
        GpiDestroyPS(pPaint->hps);
        DevCloseDC(pTBPaint->hdc);
        WinReleasePS(pPaint->hpsWin);
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }
    if (GpiSetBitmap(pPaint->hps, pTBPaint->hbmTemp) == HBM_ERROR)
    {
        GpiDeleteBitmap(pTBPaint->hbmTemp);
        GpiDestroyPS(pPaint->hps);
        DevCloseDC(pTBPaint->hdc);
        WinReleasePS(pPaint->hpsWin);
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }

    // set color table into rgb mode.  otherwise colors between 0 and
    // 15 are interpreted as indexes
    GpiCreateLogColorTable(pPaint->hps, 0, LCOLF_RGB, 0, 0, NULL);


    if (bActive)
    {
        ULONG attrFound;
       if ( (WinQueryPresParam(hwnd, PP_ACTIVECOLOR, 0, &attrFound, sizeof(attrFound),
                                                &(pTBPaint->lTbColor), QPF_PURERGBCOLOR)) == 0)
            pTBPaint->lTbColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_ACTIVETITLE, 0);

        // set text colors into presentation space
        PSTBSetActiveColors(pPaint->hwnd, pPaint->hps);
    }
    else
    {
        ULONG attrFound;
        if ( (WinQueryPresParam(hwnd, PP_INACTIVECOLOR, 0, &attrFound, sizeof(attrFound),
                                                &(pTBPaint->lTbColor), QPF_PURERGBCOLOR)) == 0 )
            pTBPaint->lTbColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_INACTIVETITLE, 0);

        // set text colors into presentation space
        PSTBSetInactiveColors(pPaint->hwnd, pPaint->hps);
    }
    // fill with default color
    WinFillRect(pPaint->hps, &(pPaint->rectlUpdate), pTBPaint->lTbColor);
    //End of temporary section...

    // call Plugins Paint Procedures here
    for (i = pCBZData->sTitlebarIndex; i < pCBZData->sFrameBackgroundIndex; i++)
    {
        if (!(/*pTBData*/pCBZData->Plugins[i].pfnPluginRender(pPaint, /*pTBData*/pCBZData->Plugins[i].pData, bActive)))
        {
            GpiSetBitmap(pPaint->hps, NULLHANDLE);
            GpiDeleteBitmap(pTBPaint->hbmTemp);
            GpiDestroyPS(pPaint->hps);
            DevCloseDC(pTBPaint->hdc);
            WinReleasePS(pPaint->hpsWin);
            DosFreeMem(pPaint);
            DosFreeMem(pTBPaint);
            return (FALSE);
        }
    }

    if ((rc = BltToWindow(pPaint)) == TRUE)
    {
        WinQueryUpdateRect(hwnd, &(pPaint->rectlUpdate));
        WinValidateRect(hwnd, &(pPaint->rectlUpdate), FALSE);
    }
    GpiSetBitmap(pPaint->hps, NULLHANDLE);
    GpiDeleteBitmap(pTBPaint->hbmTemp);
    GpiDestroyPS(pPaint->hps);
    DevCloseDC(pTBPaint->hdc);
    WinReleasePS(pPaint->hpsWin);
    DosFreeMem(pPaint);
    DosFreeMem(pTBPaint);
    return (rc);
}


// sets active text fg and bg into hps.  for some reason, this must be done every paint
void PSTBSetActiveColors(HWND hwnd, HPS hps)
{
    ULONG       ulAttrType;
    ULONG       aulColor[2];
    CHARBUNDLE  cBundle;
    long        lBitCount;

    DevQueryCaps(GpiQueryDevice(hps), CAPS_COLOR_BITCOUNT, 1L, &lBitCount);

    // query pres params
    if (WinQueryPresParam(hwnd,
                          PP_ACTIVETEXTFGNDCOLOR,
                          PP_ACTIVETEXTFGNDCOLORINDEX,
                          &ulAttrType,
                          sizeof(aulColor),
                          &aulColor,
                          QPF_PURERGBCOLOR))
    {
        cBundle.lColor = aulColor[0];
    }
    else                        // if there are none, use sys color

    {
        cBundle.lColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_ACTIVETITLETEXT, 0);
    }

    if (lBitCount < 16)
        cBundle.lColor = GpiQueryColorIndex(hps, 0, cBundle.lColor);
    else if (lBitCount < 24)
        cBundle.lColor = GpiQueryNearestColor(hps, 0, cBundle.lColor);

    // ditto
    if (WinQueryPresParam(hwnd,
                          PP_ACTIVETEXTBGNDCOLOR,
                          PP_ACTIVETEXTBGNDCOLORINDEX,
                          &ulAttrType,
                          sizeof(aulColor),
                          &aulColor,
                          QPF_PURERGBCOLOR))
    {
        cBundle.lBackColor = aulColor[0];
    }
    else
    {
        cBundle.lBackColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_ACTIVETITLETEXTBGND, 0);
    }
    if (lBitCount < 16)
        cBundle.lBackColor = GpiQueryColorIndex(hps, 0, cBundle.lBackColor);
    else if (lBitCount < 24)
        cBundle.lBackColor = GpiQueryNearestColor(hps, 0, cBundle.lBackColor);

    GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);

    // set these into hps
    GpiSetAttrs(hps, PRIM_CHAR, CBB_COLOR | CBB_BACK_COLOR, 0, (PBUNDLE) & cBundle);

    return;
}

// set inactive text fg and bg into hps.  see comments for setactivecolors
void PSTBSetInactiveColors(HWND hwnd, HPS hps)
{
    ULONG       ulAttrType;
    ULONG       aulColor[2];
    CHARBUNDLE  cBundle;
    long        lBitCount;

    DevQueryCaps(GpiQueryDevice(hps), CAPS_COLOR_BITCOUNT, 1L, &lBitCount);

    if (WinQueryPresParam(hwnd,
                          PP_INACTIVETEXTFGNDCOLOR,
                          PP_INACTIVETEXTFGNDCOLORINDEX,
                          &ulAttrType,
                          sizeof(aulColor),
                          &aulColor,
                          QPF_PURERGBCOLOR))
    {
        cBundle.lColor = aulColor[0];
    }
    else
    {
        cBundle.lColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_INACTIVETITLETEXT, 0);
    }

    if (lBitCount < 16)
        cBundle.lColor = GpiQueryColorIndex(hps, 0, cBundle.lColor);
    else if (lBitCount < 24)
        cBundle.lColor = GpiQueryNearestColor(hps, 0, cBundle.lColor);

    if (WinQueryPresParam(hwnd,
                          PP_INACTIVETEXTBGNDCOLOR,
                          PP_INACTIVETEXTBGNDCOLORINDEX,
                          &ulAttrType,
                          sizeof(aulColor),
                          &aulColor,
                          QPF_PURERGBCOLOR))
    {
        cBundle.lBackColor = aulColor[0];
    }
    else
    {
        cBundle.lBackColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_INACTIVETITLETEXTBGND, 0);
    }

    if (lBitCount < 16)
        cBundle.lBackColor = GpiQueryColorIndex(hps, 0, cBundle.lBackColor);
    else if (lBitCount < 24)
        cBundle.lBackColor = GpiQueryNearestColor(hps, 0, cBundle.lBackColor);

    GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);

    GpiSetAttrs(hps, PRIM_CHAR, CBB_COLOR | CBB_BACK_COLOR, 0, (PBUNDLE) & cBundle);

    return;
}


/*************************************************************************************************************

    $Id: CandyBarZPaintProc.c,v 1.1 1999/06/15 20:47:56 ktk Exp $

    $Log: CandyBarZPaintProc.c,v $
    Revision 1.1  1999/06/15 20:47:56  ktk
    Import

    Revision 1.13  1998/10/19 18:14:19  enorman
    Partial fix for the WPS open view text bug

    Revision 1.12  1998/09/26 17:55:50  mwagner
    Fixed bug that was causing crash when compile with O+ flag

    Revision 1.11  1998/09/19 17:26:57  mwagner
    Fixed font problem

    Revision 1.10  1998/09/12 10:04:16  pgarner
    Added copyright notice

    Revision 1.9  1998/09/12 07:35:45  mwagner
    Added support for bitmap images

    Revision 1.6  1998/08/31 19:34:33  enorman
    Finished the implementation of 3D Text

    Revision 1.5  1998/08/10 03:29:34  mwagner
    Moved the call to DosFreeMem back to it's proper scope

    Revision 1.4  1998/08/10 03:01:32  enorman
    Added Code to support 3D text

    Revision 1.3  1998/06/29 15:32:43  mwagner
    Matts S3 and PMView fixes
    .\

    Revision 1.2  1998/06/12 16:19:43  ENORMAN
    Modified paint procedure to draw the text with a 3D-ish shadow
    Right now the shadow is hard coded to CLR_WHITE

    Revision 1.1  1998/06/08 14:18:56  pgarner
    Initial Checkin

*************************************************************************************************************/
