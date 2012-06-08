/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#include "candybarz.h"

extern ULONG       ulOffsetPrivateData;

BOOL CandyBarZFramePaintBackgroundProc(HWND hwnd, CBZDATA *pCBZData, RECTL * rect, HPS hpsFrame)
{
    PSPAINT     *pPaint;
    TEMPPAINT   *pFramePaint;
    BOOL        rc;
    int         i;

    if (pCBZData->sFrameBackgroundIndex >= pCBZData->sFrameBorderIndex)
    {
        return (FALSE);
    }

    // alloc paint struct
    if (DosAllocMem((PPVOID) & pFramePaint,
                    sizeof(TEMPPAINT),
                    PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        return (FALSE);
    }
    memset(pFramePaint, 0, sizeof(TEMPPAINT));

    pFramePaint->hab = WinQueryAnchorBlock(hwnd);

    // alloc paint struct
    if (DosAllocMem((PPVOID) & pPaint,
                    sizeof(PSPAINT),
                    PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        DosFreeMem(pFramePaint);
        return (FALSE);
    }
    memset(pPaint, 0, sizeof(PSPAINT));

    //Text plugin needs this for quering presentation parameters.
    pPaint->hwnd = hwnd;

    // entire window
    WinQueryWindowRect(pPaint->hwnd, &(pPaint->rectlWindow));

    //setup update rectangle
    pPaint->rectlUpdate.xLeft = rect->xLeft;
    pPaint->rectlUpdate.xRight = rect->xRight;
    pPaint->rectlUpdate.yTop = rect->yTop;
    pPaint->rectlUpdate.yBottom = rect->yBottom;

    // get PS
    pPaint->hpsWin = hpsFrame;


    // we will blt and draw gradient text into the hps associated with this dc
    if ((pFramePaint->hdc = DevOpenDC(pFramePaint->hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE))
            == NULLHANDLE)
    {
        DosFreeMem(pPaint);
        DosFreeMem(pFramePaint);
        return (FALSE);
    }
    // create hps2 for hdc2.  hps for hdc depends on size of bitmap, so it's not created until
    // we know if it's active or not.  create for size of window, then we blt only portion
    // needed
    pFramePaint->slHPS.cx = pPaint->rectlWindow.xRight;
    pFramePaint->slHPS.cy = pPaint->rectlWindow.yTop;

    if ((pPaint->hps = GpiCreatePS(pFramePaint->hab,
                                   pFramePaint->hdc,
                                   &(pFramePaint->slHPS),
                          PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC))
            == NULLHANDLE)
    {
        DevCloseDC(pFramePaint->hdc);
        DosFreeMem(pPaint);
        DosFreeMem(pFramePaint);
        return (FALSE);
    }

    pFramePaint->bif2.cbFix = sizeof(BITMAPINFOHEADER2);
    pFramePaint->bif2.cx = pPaint->rectlWindow.xRight;
    pFramePaint->bif2.cy = pPaint->rectlWindow.yTop;
    pFramePaint->bif2.cBitCount = 24;
    pFramePaint->bif2.cPlanes = 1;
    if ((pFramePaint->hbmTemp = GpiCreateBitmap(pPaint->hps,  // create bitmap
                                 (PBITMAPINFOHEADER2) & (pFramePaint->bif2),
                                                (ULONG) 0,
                                                (PSZ) NULL,
                       (PBITMAPINFO2) & (pFramePaint->bif2))) == NULLHANDLE)
    {
        GpiDestroyPS(pPaint->hps);
        DevCloseDC(pFramePaint->hdc);
        DosFreeMem(pPaint);
        DosFreeMem(pFramePaint);
        return (FALSE);
    }
    if (GpiSetBitmap(pPaint->hps, pFramePaint->hbmTemp) == HBM_ERROR)
    {
        GpiDeleteBitmap(pFramePaint->hbmTemp);
        GpiDestroyPS(pPaint->hps);
        DevCloseDC(pFramePaint->hdc);
        DosFreeMem(pPaint);
        DosFreeMem(pFramePaint);
        return (FALSE);
    }

    // set color table into rgb mode.  otherwise colors between 0 and
    // 15 are interpreted as indexes
    GpiCreateLogColorTable(pPaint->hps, 0, LCOLF_RGB, 0, 0, NULL);

#if 0
    {
        //fill ps with default bg color...
        ULONG attrFound;
        long lColor;
        if ( (WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, 0, &attrFound, sizeof(attrFound),
                                                &lColor, QPF_PURERGBCOLOR)) == 0 )
           lColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0);
        // fill with default color
        WinFillRect(pPaint->hps, &(pPaint->rectlUpdate), lColor);
    }
#endif

    // call Plugins Paint Procedures here
    for (i = pCBZData->sFrameBackgroundIndex; i < pCBZData->sFrameBorderIndex; i++)
    {
        if ( !(pCBZData->Plugins[i].pfnPluginRender(pPaint, pCBZData->Plugins[i].pData, TRUE)) )
        {

            GpiSetBitmap(pPaint->hps, NULLHANDLE);
            GpiDeleteBitmap(pFramePaint->hbmTemp);
            GpiDestroyPS(pPaint->hps);
            DevCloseDC(pFramePaint->hdc);

            DosFreeMem(pPaint);
            DosFreeMem(pFramePaint);
            return (FALSE);
        }
    }


    if ((rc = BltToWindow(pPaint)) == TRUE)
    {
        //paint sucessful, validate the window
      //        WinQueryWindowRect(hwnd, &(pPaint->rectlUpdate));
        WinValidateRect(hwnd, &(pPaint->rectlUpdate), FALSE);
    }
    GpiSetBitmap(pPaint->hps, NULLHANDLE);
    GpiDeleteBitmap(pFramePaint->hbmTemp);
    GpiDestroyPS(pPaint->hps);
    DevCloseDC(pFramePaint->hdc);

    DosFreeMem(pPaint);
    DosFreeMem(pFramePaint);
    
    /* Mark the frame as candybarz painted */
    //WinSetWindowULong(hwnd,ulOffsetPrivateData+offsetof(PRIVATEFRAMEDATA, bCustomPaint),TRUE);

    return (rc);
}


BOOL CandyBarZFrameBorderPaintProc(HWND hwnd, RECTL * rect, HPS hpsFrame, short sState)
{
    CBZDATA     *pCBZData;
    CBZSHARE    *pCBZShare;
    PSPAINT     *pPaint;
    TEMPPAINT   *pFramePaint;
    BOOL        rc;
    int         i;
    ULONG       ulDataOffset;

    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE,  PAG_READ) != NO_ERROR)
    {
        return (FALSE);
    }
    ulDataOffset = pCBZShare->ulDataOffset;
    DosFreeMem(pCBZShare);

    // get window data
    if ((pCBZData = WinQueryWindowPtr(CandyBarZGetTopLevelFrameHWND(hwnd), ulDataOffset)) == NULL
            || !pCBZData->bFrameBorderEnabled  )
    {
        return (FALSE);
    }

    if (pCBZData->sFrameBorderIndex >= pCBZData->sPushButtonIndex)
    {
        return (FALSE);
    }

    // alloc paint struct
    if (DosAllocMem((PPVOID) & pFramePaint,
                    sizeof(TEMPPAINT),
                    PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        return (FALSE);
    }
    memset(pFramePaint, 0, sizeof(TEMPPAINT));

    pFramePaint->hab = WinQueryAnchorBlock(hwnd);

    // alloc paint struct
    if (DosAllocMem((PPVOID) & pPaint,
                    sizeof(PSPAINT),
                    PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        DosFreeMem(pFramePaint);
        return (FALSE);
    }
    memset(pPaint, 0, sizeof(PSPAINT));

    //Text plugin needs this for quering presentation parameters.
    pPaint->hwnd = hwnd;

    // entire window
    pPaint->rectlWindow = pPaint->rectlUpdate = *rect;

    // get PS
    pPaint->hpsWin = hpsFrame;

    // we will blt and draw gradient text into the hps associated with this dc
    if ((pFramePaint->hdc = DevOpenDC(pFramePaint->hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE))
            == NULLHANDLE)
    {
        DosFreeMem(pPaint);
        DosFreeMem(pFramePaint);
        return (FALSE);
    }
    // create hps2 for hdc2.  hps for hdc depends on size of bitmap, so it's not created until
    // we know if it's active or not.  create for size of window, then we blt only portion
    // needed
    pFramePaint->slHPS.cx = pPaint->rectlWindow.xRight;
    pFramePaint->slHPS.cy = pPaint->rectlWindow.yTop;

    if ((pPaint->hps = GpiCreatePS(pFramePaint->hab,
                                   pFramePaint->hdc,
                                   &(pFramePaint->slHPS),
                          PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC))
            == NULLHANDLE)
    {
        DevCloseDC(pFramePaint->hdc);
        DosFreeMem(pPaint);
        DosFreeMem(pFramePaint);
        return (FALSE);
    }

    pFramePaint->bif2.cbFix = sizeof(BITMAPINFOHEADER2);
    pFramePaint->bif2.cx = pPaint->rectlWindow.xRight;
    pFramePaint->bif2.cy = pPaint->rectlWindow.yTop;
    pFramePaint->bif2.cBitCount = 24;
    pFramePaint->bif2.cPlanes = 1;
    if ((pFramePaint->hbmTemp = GpiCreateBitmap(pPaint->hps,  // create bitmap
                                 (PBITMAPINFOHEADER2) & (pFramePaint->bif2),
                                                (ULONG) 0,
                                                (PSZ) NULL,
                       (PBITMAPINFO2) & (pFramePaint->bif2))) == NULLHANDLE)
    {
        GpiDestroyPS(pPaint->hps);
        DevCloseDC(pFramePaint->hdc);
        DosFreeMem(pPaint);
        DosFreeMem(pFramePaint);
        return (FALSE);
    }
    if (GpiSetBitmap(pPaint->hps, pFramePaint->hbmTemp) == HBM_ERROR)
    {
        GpiDeleteBitmap(pFramePaint->hbmTemp);
        GpiDestroyPS(pPaint->hps);
        DevCloseDC(pFramePaint->hdc);
        DosFreeMem(pPaint);
        DosFreeMem(pFramePaint);
        return (FALSE);
    }

    // set color table into rgb mode.  otherwise colors between 0 and
    // 15 are interpreted as indexes
    GpiCreateLogColorTable(pPaint->hps, 0, LCOLF_RGB, 0, 0, NULL);

    {
        //fill ps with default bg color...
        ULONG attrFound;
        long lColor;
        if ( (WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, 0, &attrFound, sizeof(attrFound),
                                                &lColor, QPF_PURERGBCOLOR)) == 0 )
           lColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0);
        // fill with default color
        WinFillRect(pPaint->hps, &(pPaint->rectlUpdate), lColor);
    }

    // call Plugins Paint Procedures here
    for (i = pCBZData->sFrameBorderIndex; i < pCBZData->sPushButtonIndex; i++)
    {
        if ( !(pCBZData->Plugins[i].pfnPluginRender(pPaint, pCBZData->Plugins[i].pData, sState)) )
        {
            GpiSetBitmap(pPaint->hps, NULLHANDLE);
            GpiDeleteBitmap(pFramePaint->hbmTemp);
            GpiDestroyPS(pPaint->hps);
            DevCloseDC(pFramePaint->hdc);
            DosFreeMem(pPaint);
            DosFreeMem(pFramePaint);
            return (FALSE);
        }
    }


    if ((rc = BltToWindow(pPaint)) == TRUE)
    {
        //paint sucessful, validate the window
//        WinQueryWindowRect(hwnd, &(pPaint->rectlUpdate));
        WinValidateRect(hwnd, &(pPaint->rectlUpdate), FALSE);
    }
    GpiSetBitmap(pPaint->hps, NULLHANDLE);
    GpiDeleteBitmap(pFramePaint->hbmTemp);
    GpiDestroyPS(pPaint->hps);
    DevCloseDC(pFramePaint->hdc);
    DosFreeMem(pPaint);
    DosFreeMem(pFramePaint);

    return (rc);
}


/*************************************************************************************************************

    $Id: candybarzframepaintproc.c,v 1.4 2000/09/24 20:59:13 cwohlgemuth Exp $

    $Log: candybarzframepaintproc.c,v $
    Revision 1.4  2000/09/24 20:59:13  cwohlgemuth
    Resync.

    Revision 1.3  2000/09/09 23:10:18  cwohlgemuth
    Resynced

    Revision 1.2  1999/12/29 19:38:08  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:47:55  ktk
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
