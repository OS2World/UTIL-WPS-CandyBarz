
/******************************************************************************

  Copyright Netlabs, 1998-2000 

  http://www.netlabs.org

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

#include "candybarz.h"

extern ULONG       ulOffsetPrivateData;

/********************************************************************
* CandyBarZGeneralPaintProc: Paint the given window based on the    *
*   selected plugins.  The window image is first constructed in     *
*   a memory PS and then BitBlt'd to the screen. (avoid flickering) *
* Parameters:                                                       *
*   HWND hwnd - handle of the window to paint                       *
*   BOOL bEntire - Flag indicating whether to paint entire window   *
*                   or only the update rectangle.                   *
*   BOOL bActive - Flag indicating which state to paint.            *
*   int  startIndex - index of the first plugin to use.             *
*   int  endIndex - index of the last plugin to use.                *
*   ULONG ulWindowType - the type of window to paint                *
*********************************************************************/
BOOL CandyBarZGeneralPaintProc(HWND hwnd, CBZDATA *pCBZData, BOOL bEntire, int sState,
                                int startIndex, int endIndex, ULONG ulWindowType)
{
    //allocate the variables we need dynamically
    // to keep them off the stack
    // we operate in the stack space of the app
    // which may not have a large enough stack

    PSPAINT     *pPaint;        //structure containing info passed to the plugins
    TEMPPAINT   *pTBPaint;      //structure containing other temporary variables
    char        *pchnl;
    int         i;              //counter
    BOOL        rc;             //flag indicating success of the BitBlit function
    BOOL        bDraw;

    // alloc paint struct
    if (DosAllocMem((PPVOID) & pTBPaint,
                    sizeof(TEMPPAINT),
                    PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
    {
        return (FALSE);
    }
    memset(pTBPaint, 0, sizeof(TEMPPAINT));

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

if (ulWindowType != CBZ_FRAMEBRDR)
{
    //get the window text
    WinQueryWindowText(hwnd, sizeof(pPaint->szTitleText), pPaint->szTitleText);

    if(ulWindowType==CBZ_TITLEBAR) {
      //replace line breaks in szTitleText with spaces.
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
      //**NOTE**: This requires an extra WPS class that sets the szWPSViewText variable.
      //  If there is a way to query the view text of an open WPS object, please
      //  let me know!!!
      if (strlen(pCBZData->szWPSViewText) > 0)
        {
          strcat(pPaint->szTitleText, " - ");
          strcat(pPaint->szTitleText, pCBZData->szWPSViewText);
        }
    }

    // Open a device context that we can use to draw offscreen
    if ((pTBPaint->hdc = DevOpenDC(pTBPaint->hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE))
            == NULLHANDLE)
    {
        //error opening the DC, so cleanup and bail
        WinReleasePS(pPaint->hpsWin);
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }

    //create a presentation space for pTBPaint->hdc
    pTBPaint->slHPS.cx = pPaint->rectlWindow.xRight;
    pTBPaint->slHPS.cy = pPaint->rectlWindow.yTop;
    if ((pPaint->hps = GpiCreatePS(pTBPaint->hab,
                                   pTBPaint->hdc,
                                   &(pTBPaint->slHPS),
                                    PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC))
                                == NULLHANDLE)
    {
        //error creating the PS so cleanup and bail
        DevCloseDC(pTBPaint->hdc);
        WinReleasePS(pPaint->hpsWin);
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }

    //create a bitmap for pPaint->hps
    pTBPaint->bif2.cbFix = sizeof(BITMAPINFOHEADER2);
    pTBPaint->bif2.cx = pPaint->rectlWindow.xRight;
    pTBPaint->bif2.cy = pPaint->rectlWindow.yTop;
    pTBPaint->bif2.cBitCount = 24;      //**NOTE**: perhaps 8bits would be good enough?? (256 colors)
    pTBPaint->bif2.cPlanes = 1;
    if ((pTBPaint->hbmTemp = GpiCreateBitmap(pPaint->hps,  // create bitmap
                                    (PBITMAPINFOHEADER2) & (pTBPaint->bif2),
                                             (ULONG) 0,
                                             (PSZ) NULL,
                          (PBITMAPINFO2) & (pTBPaint->bif2))) == NULLHANDLE)
    {
        //error creating the bitmap so cleanup and bail
        GpiDestroyPS(pPaint->hps);
        DevCloseDC(pTBPaint->hdc);
        WinReleasePS(pPaint->hpsWin);
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }
    //set the bitmap into the presentation space
    if (GpiSetBitmap(pPaint->hps, pTBPaint->hbmTemp) == HBM_ERROR)
    {
        //error setting the bitmap so cleanup and bail
        GpiDeleteBitmap(pTBPaint->hbmTemp);
        GpiDestroyPS(pPaint->hps);
        DevCloseDC(pTBPaint->hdc);
        WinReleasePS(pPaint->hpsWin);
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }
}
    // set color table into rgb mode.  otherwise colors between 0 and
    // 15 are interpreted as indexes
    GpiCreateLogColorTable(pPaint->hps, 0, LCOLF_RGB, 0, 0, NULL);

    if ( ulWindowType == CBZ_TITLEBAR )
    {
        TBSetPSColors(pPaint->hwnd, pPaint->hps, sState);

        TBFillBG(pPaint, sState);
    }
    else if ( ( ulWindowType == CBZ_PUSHBUTTON ) 
              || ( ulWindowType == CBZ_RADIOBUTTON ) ||
              ( ulWindowType == CBZ_CHECKBOX ) )
    {
      ButtonSetPSColors(pPaint->hwnd, pPaint->hps, sState);
      ButtonFillBG(pPaint, sState, pCBZData);
#if 0
      if(ulWindowType == CBZ_PUSHBUTTON) {
        ULONG style=WinQueryWindowULong(hwnd, QWL_STYLE);
        if(style & (ULONG)(BS_MINIICON|BS_ICON))
         *(pPaint->szTitleText)=0;
      }
#endif
    }
    else if ( ulWindowType == CBZ_FRAMEBRDR )
    {
        //nothing yet
    }

    bDraw = FALSE;
    // call Plugins Paint Procedures here
    for (i = startIndex; i < endIndex; i++)
    {
        if ((pCBZData->Plugins[i].pfnPluginRender(pPaint, pCBZData->Plugins[i].pData, sState)))
        {
            bDraw = TRUE;
        }
    }

    if (!bDraw)
    {
        //none of the plugins did any drawing! so bail...
        GpiSetBitmap(pPaint->hps, NULLHANDLE);
        GpiDeleteBitmap(pTBPaint->hbmTemp);
        GpiDestroyPS(pPaint->hps);
        DevCloseDC(pTBPaint->hdc);
        WinReleasePS(pPaint->hpsWin);
        DosFreeMem(pPaint);
        DosFreeMem(pTBPaint);
        return (FALSE);
    }

    if ( ulWindowType == CBZ_FRAMEBRDR )
    {
        POINTL border;
        RECTL  rectlSide;
        WinSendMsg(pPaint->hwnd, WM_QUERYBORDERSIZE, MPFROMP(&border), 0);
        //left
        rectlSide.xLeft     = pPaint->rectlWindow.xLeft;
        rectlSide.xRight    = pPaint->rectlWindow.xLeft + border.x;
        rectlSide.yBottom   = pPaint->rectlWindow.yBottom;
        rectlSide.yTop      = pPaint->rectlWindow.yTop;
        WinValidateRect(pPaint->hwnd, &rectlSide, FALSE);

        //right
        rectlSide.xLeft     = pPaint->rectlWindow.xRight - border.x;
        rectlSide.xRight    = pPaint->rectlWindow.xRight;
        rectlSide.yBottom   = pPaint->rectlWindow.yBottom;
        rectlSide.yTop      = pPaint->rectlWindow.yTop;
        WinValidateRect(pPaint->hwnd, &rectlSide, FALSE);

        //bottom
        rectlSide.xLeft     = pPaint->rectlWindow.xLeft + border.x;
        rectlSide.xRight    = pPaint->rectlWindow.xRight - border.y;
        rectlSide.yBottom   = pPaint->rectlWindow.yBottom;
        rectlSide.yTop      = pPaint->rectlWindow.yBottom + border.y;
        WinValidateRect(pPaint->hwnd, &rectlSide, FALSE);

        //top
        rectlSide.xLeft     = pPaint->rectlWindow.xLeft + border.x;
        rectlSide.xRight    = pPaint->rectlWindow.xRight - border.y;
        rectlSide.yBottom   = pPaint->rectlWindow.yTop - border.y;
        rectlSide.yTop      = pPaint->rectlWindow.yTop;
        WinValidateRect(pPaint->hwnd, &rectlSide, FALSE);
    }
    //the offscreen bitmap is complete, so blt it to the screen.
    else if ((rc = BltToWindow(pPaint)) == TRUE)
    {
        //Blt was sucessful so mark the update window as valid!
        // otherwise the update window is still invalid and the
        // original window procedure will paint it.
        WinValidateRect(hwnd, &(pPaint->rectlUpdate), FALSE);
    }

    //clean up
    GpiSetBitmap(pPaint->hps, NULLHANDLE);
    if (pTBPaint->hbmTemp != NULLHANDLE)
        GpiDeleteBitmap(pTBPaint->hbmTemp);
    if (pPaint->hps != NULLHANDLE)
        GpiDestroyPS(pPaint->hps);
    if (pTBPaint->hdc != NULLHANDLE)
        DevCloseDC(pTBPaint->hdc);
    WinReleasePS(pPaint->hpsWin);
    DosFreeMem(pPaint);
    DosFreeMem(pTBPaint);
    return (rc);
}


void TBFillBG(PSPAINT *pPaint, int sState)
{
    LONG  lTbColor;
    ULONG attrFound;

    //set the titlebar color into the presentation space.
    if (sState)
    {
       if ( (WinQueryPresParam(pPaint->hwnd, PP_ACTIVECOLOR, 0, &attrFound, sizeof(attrFound),
                                                &lTbColor, QPF_PURERGBCOLOR)) == 0)
            lTbColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_ACTIVETITLE, 0);
    }
    else
    {
        if ( (WinQueryPresParam(pPaint->hwnd, PP_INACTIVECOLOR, 0, &attrFound, sizeof(attrFound),
                                                &lTbColor, QPF_PURERGBCOLOR)) == 0 )
            lTbColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_INACTIVETITLE, 0);
    }

    // fill entire window with default color
    WinFillRect(pPaint->hps, &(pPaint->rectlUpdate), lTbColor);
};

// sets active text fg and bg into hps.  for some reason, this must be done every paint
void TBSetPSColors(HWND hwnd, HPS hps, int sState)
{
    ULONG       ulAttrType;
    ULONG       aulColor[2];
    CHARBUNDLE  cBundle;
    long        lBitCount;

    DevQueryCaps(GpiQueryDevice(hps), CAPS_COLOR_BITCOUNT, 1L, &lBitCount);

    if (sState == TRUE)
    {
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
    }
    else
    {
        // query pres params
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
        else                        // if there are none, use sys color

        {
            cBundle.lColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_INACTIVETITLETEXT, 0);
        }
    }

    if (lBitCount < 16)
        cBundle.lColor = GpiQueryColorIndex(hps, 0, cBundle.lColor);
    else if (lBitCount < 24)
        cBundle.lColor = GpiQueryNearestColor(hps, 0, cBundle.lColor);

    if (sState)
    {
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
    }
    else
    {
        // query pres params
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
        else                        // if there are none, use sys color

        {
            cBundle.lColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_INACTIVETITLETEXT, 0);
        }
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

void ButtonFillBG(PSPAINT *pPaint, int sState, CBZDATA *pCBZData)
{
//**NOTE**: Ideally this should check if frame backgrounds are enabled.  If not enabled fill the
//              window with the background color.  If enabled, copy the contents of the frame ps
//              into the button ps.


  RECTL rectl;
  POINTL aptl[4];
  HWND hwndFrame;
  HPS hpsFrame;
  
  if(pCBZData->bFrameBackgroundEnabled) {
    //copy the contents of the frame ps to the new ps
    hwndFrame=WinQueryWindow(pPaint->hwnd,QW_PARENT);
    //  if(WinQueryWindowULong(hwndFrame ,ulOffsetPrivateData+offsetof(PRIVATEFRAMEDATA, bCustomPaint))==TRUE) {
    
    hpsFrame = WinGetPS(hwndFrame);
    rectl=pPaint->rectlUpdate;  
    WinMapWindowPoints(pPaint->hwnd, hwndFrame, (PPOINTL) & rectl, 2);
#if 0
    CandyBarZFramePaintBackgroundProc(hwndFrame, pCBZData, &rectl, hpsFrame);
    WinReleasePS(hpsFrame);
    return;
#endif
      aptl[0].x = pPaint->rectlUpdate.xLeft;
      aptl[0].y = pPaint->rectlUpdate.yBottom;
      aptl[1].x = pPaint->rectlUpdate.xRight;
      aptl[1].y = pPaint->rectlUpdate.yTop;
      aptl[2].x = rectl.xLeft;
      aptl[2].y = rectl.yBottom;
      aptl[3].x = rectl.xRight;
      aptl[3].y = rectl.yTop;
      GpiBitBlt(pPaint->hps, hpsFrame, 4, aptl, ROP_SRCCOPY, BBO_IGNORE);
      WinReleasePS(hpsFrame);
      return;
      // }
  }

  WinFillRect(pPaint->hps, &(pPaint->rectlUpdate), WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0));

};

void ButtonSetPSColors(HWND hwnd, HPS hps, int sState)
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
        cBundle.lColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_WINDOWTEXT, 0);
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
        cBundle.lBackColor = WinQuerySysColor(HWND_DESKTOP, SYSCLR_BACKGROUND, 0);
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


/*************************************************************************************************************

    $Id: candybarztbpaintproc.c,v 1.4 2000/09/24 21:01:48 cwohlgemuth Exp $

    $Log: candybarztbpaintproc.c,v $
    Revision 1.4  2000/09/24 21:01:48  cwohlgemuth
    Preparation for icons on custom painted buttons.

    Revision 1.3  2000/09/09 23:05:10  cwohlgemuth
    Resynced

    Revision 1.2  2000/05/10 15:45:27  cwohlgemuth
    Changed ButtonFillBG() to support frame BG with check/radio boxes (CW)

    Revision 1.1  2000/03/26 06:58:38  enorman
    resyncing with my local tree.

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
