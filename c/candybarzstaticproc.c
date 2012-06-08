/******************************************************************************
  Copyright Netlabs, 1998, this code may not be used for any military purpose
******************************************************************************/
#include "candybarz.h"

#define INCL_DOSEXCEPTIONS
#define INCL_DOSPROCESS

#include <setjmp.h>
#include <assert.h>
#include "except.h"

extern CBZSHARE    CBZShare;

#define LIGHTNINGFAST

#if 1
MRESULT EXPENTRY CandyBarZStaticProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

    CBZDATA     *pCBZData = NULL;   //current parameters for this window.
                                    // stored in the window words of toplevel
                                    // frame window.

    CBZSHARE    *pCBZShare = NULL;  //structure in shared memory that holds the
                                    // default settings and procedure addresses.

    PFNWP       pfnOrigWndProc;     //address of original window procedure

    ULONG       ulDataOffset;

    HWND hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);/* Falls entfernt in WM_SHOW einsetztn */
    HWND hwndFrame2;

    HPS hps, hpsFrame;
    RECTL rclFrame, rclWindow, rclUpdate;
    POINTL pointl, points[4];
    char textBuffer[CCHMAXPATH];
    char *textPtr;
    ULONG textLength;
    ULONG hTotalDrawn;
    ULONG hDrawn;
    ULONG style;
    ULONG controlType;
    ULONG drawStyle;
    SIZEL charSize;
    ULONG xDelta;
    ULONG yDelta;
    MRESULT result;
    char className[10];

#if 0        
    pfnOrigWndProc  = CBZShare.pStaticOrigWndProc; //get address of original wnd proc.
    ulDataOffset    = CBZShare.ulDataOffset;      //get offset to pCBZData
#endif    


    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ) == NO_ERROR)
      {
        ulDataOffset = pCBZShare->ulDataOffset;
        pfnOrigWndProc = pCBZShare->pStaticOrigWndProc;
        DosFreeMem(pCBZShare);

        if ((pCBZData = WinQueryWindowPtr(hwndFrame, ulDataOffset)) == NULL)
          {
            return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
          }              
      }
    else    //Unable to get Shared Memory, so try the default Window Procedure.
      return WinDefWindowProc(hwnd, msg, mp1, mp2);


    /* if the parent frame does not have a candybarZ background we need no spezial handling */
    if(!pCBZData->bFrameBackgroundEnabled)
      return pfnOrigWndProc(hwnd, msg, mp1, mp2);
    
        
	/* If the parent isn't a frame window, we have no custom background thus the system can paint the
		 static windows */
    hwndFrame2=WinQueryWindow(hwnd,QW_PARENT);
    if(!hwndFrame2)
      return pfnOrigWndProc(hwnd, msg, mp1, mp2);

    WinQueryClassName(hwndFrame2, sizeof(className), className);
    if (strcmp(className, "#1") != 0)  /* Alle Windows ausser frame windows */
        return pfnOrigWndProc(hwnd, msg, mp1, mp2);  /* Parent isn't a frame */

    
    switch (msg)
    {


    case WM_PRESPARAMCHANGED:
        {
            /* Drawing problem with fly over solved with this */
            if (LONGFROMMP(mp1) == PP_BACKGROUNDCOLORINDEX || LONGFROMMP(mp1) == PP_BACKGROUNDCOLOR)
            {
                /* Background has to change */
                result = pfnOrigWndProc(hwnd, msg, mp1, mp2);
                WinSetWindowULong(hwnd, QWL_STYLE, WinQueryWindowULong(hwnd, QWL_STYLE) | DT_ERASERECT);
                WinQueryWindowRect(hwnd, &rclWindow);
                WinInvalidateRect(hwnd, &rclWindow, FALSE);  /* Force repainting */
                return result;
            }
#if 0
            if (LONGFROMMP(mp1) == PP_FONTNAMESIZE)
            {
                WinShowWindow(hwnd, FALSE);
                result = pfnOrigWndProc(hwnd, msg, mp1, mp2);
                WinShowWindow(hwnd, TRUE);
                /* Caution: WinShowWindow(TRUE) forces repainting of the background. A message WM_SETWINDOWPARAMS is sent,too. */
                /* This causes another background painting. Might become a speed problem under some circumstances. Don't know how */
                /* to circumvent that */
                return result;
            }
#endif
        }
        break;
    case WM_SETWINDOWPARAMS:
        {         
            if (((PWNDPARAMS) PVOIDFROMMP(mp1))->fsStatus == WPM_TEXT)
            {
                result = pfnOrigWndProc(hwnd, msg, mp1, mp2);

                // s.o.   hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);         
                /* Draw background */
                hpsFrame = WinGetPS(hwndFrame);

                TRY_LOUD(excWndParams){ // Exception handler from XFolder
         
                  WinQueryWindowRect(hwnd, &rclFrame);
                  WinMapWindowPoints(hwnd, hwndFrame, (PPOINTL) & rclFrame, 2);
                  CandyBarZFramePaintBackgroundProc(hwndFrame, pCBZData, &rclFrame, hpsFrame);
           
                }            
                CATCH(excWndParams) {
                }
                END_CATCH;
                
                
                WinReleasePS(hpsFrame);
                WinQueryWindowRect(hwnd, &rclWindow);
                WinInvalidateRect(hwnd, &rclWindow, FALSE);  /* Force repainting */
                
                return result;
            }
        }
        break;


#ifndef LIGHTNINGFAST         // Is this correct??
    case WM_SHOW:
      {
        
        if (!SHORT1FROMMP(mp1))
          return pfnOrigWndProc(hwnd, msg, mp1, mp2);  /* Hide the window */

        /* Draw the background to clear it. During WM_PAINT we draw the txt */
        hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);
        /* Draw background */
        hpsFrame = WinGetPS(hwndFrame);

        TRY_LOUD(excWM_SHOW){ // Exception handler from XFolder
          
          WinQueryWindowRect(hwnd, &rclFrame);
          WinMapWindowPoints(hwnd, hwndFrame, (PPOINTL) & rclFrame, 2);
          //CandyBarZFramePaintBackgroundProc(hwndFrame, &rclFrame, hpsFrame);
          CandyBarZFramePaintBackgroundProc(hwndFrame, pCBZData, &rclFrame, hpsFrame);
          
        }            
        CATCH(excWM_SHOW) {
        }
        END_CATCH;
        
        WinReleasePS(hpsFrame);
        return (MRESULT) 0;
      }
#endif
      //force the background to be redrawn because
      // something has changed.
    case WM_ENABLE:
      {

#ifndef LIGHTNINGFAST         // Is this correct??
        HWND hwndFrame = WinQueryWindow(hwnd, QW_PARENT);
        // Draw background
        hpsFrame = WinGetPS(hwndFrame);

        TRY_LOUD(excWM_ENABLE){ // Exception handler from XFolder

        WinQueryWindowRect(hwnd, &rclFrame);
        rclWindow=rclFrame;
        WinMapWindowPoints(hwnd, hwndFrame, (PPOINTL) & rclFrame, 2);
        CandyBarZFramePaintBackgroundProc(hwndFrame, pCBZData, &rclFrame, hpsFrame);
        }            
        CATCH(excWM_ENABLE) {
        }
        END_CATCH;

        WinReleasePS(hpsFrame);
#else
        style = WinQueryWindowULong(hwnd, QWL_STYLE);
        controlType = style & (ULONG) 0x7F;  // Mask control type style bits
        
        WinQueryWindowRect(hwnd, &rclWindow);
        if(controlType==SS_GROUPBOX)
          {
            //Groupboxes currently only draw lines. No drawing problems known yet. If later discovered invalidate the whole groupbox
            //and draw the background during WM_PAINT. This bg drawing may be thrown away then.
            HWND hwndFrame = WinQueryWindow(hwnd, QW_PARENT);
            // Draw background
            hpsFrame = WinGetPS(hwndFrame);

            TRY_LOUD(excWM_ENABLE2){ // Exception handler from XFolder
              
              WinQueryWindowRect(hwnd, &rclFrame);
              rclWindow=rclFrame;
              WinMapWindowPoints(hwnd, hwndFrame, (PPOINTL) & rclFrame, 2);
              CandyBarZFramePaintBackgroundProc(hwndFrame, pCBZData, &rclFrame, hpsFrame);
            }            
            CATCH(excWM_ENABLE2) {
            }
            END_CATCH;

            WinReleasePS(hpsFrame);
          }
#endif

        WinInvalidateRect(hwnd,&rclWindow,FALSE);
      }
      break;


    case WM_PAINT:
      {
        style = WinQueryWindowULong(hwnd, QWL_STYLE);
        
        controlType = style & (ULONG) 0x7F;  // Mask control type style bits
        
        drawStyle = style;
        drawStyle &= (ULONG) 0xFF00;
        
            switch (controlType)
            {
                // It's a static text control
            case SS_TEXT:
                {


                  WinQueryUpdateRect(hwnd, &rclUpdate);
                  /*
                  if(!WinQueryUpdateRect(hwnd, &rclUpdate))
                    return ((MRESULT) FALSE);
                    */


                  //                    hps = WinGetPS(hwnd);
                  hps = WinBeginPaint( hwnd, NULLHANDLE, NULLHANDLE );

                  GpiCreateLogColorTable(hps, 0, LCOLF_RGB, 0, 0, NULL);

                  
                  
                  //WinQueryUpdateRect(hwnd, &rclUpdate);
                  // redraw window text
                  WinQueryWindowRect(hwnd, &rclWindow);
                  textPtr = textBuffer;
                  WinQueryWindowText(hwnd, sizeof(textBuffer), textBuffer);
                  
                  textLength = strlen(textBuffer);
                  GpiQueryDefCharBox(hps, &charSize);  // Get charactersize
                  
                  /* Look for color index or pure RGB foreground color */
                  /* Use ULONG controlType as var to hold presparam so we need no new one */
                  if (WinQueryPresParam(hwnd, PP_FOREGROUNDCOLOR, PP_FOREGROUNDCOLORINDEX, &yDelta,
                                        sizeof(controlType), &controlType, QPF_PURERGBCOLOR | QPF_ID2COLORINDEX))
                    {
                      
                      /* Presparam found */
                      xDelta = GpiQueryColorIndex(hps, 0, controlType);  /* find index from RGB */
                      if (xDelta == GPI_ALTERROR)
                        {
                          //!!!!!!!!!!!!!
                          xDelta = WinQuerySysColor(HWND_DESKTOP, SYSCLR_WINDOWSTATICTEXT, 0);
                          xDelta = GpiQueryColorIndex(hps, 0, xDelta);  /* find index from RGB */
                          //xDelta=SYSCLR_WINDOWSTATICTEXT;
                        }
                    }
                  else
                    /* No presparam */
                    {
                      //!!!!!!!!!!!!
                      xDelta = WinQuerySysColor(HWND_DESKTOP, SYSCLR_WINDOWSTATICTEXT, 0);
                      xDelta = GpiQueryColorIndex(hps, 0, xDelta);  /* find index from RGB */
                      //  xDelta=SYSCLR_WINDOWSTATICTEXT;
                    }

                  //SYSCLR_WINDOWSTATICTEXT;/* Use default color */
                  GpiSetColor(hps, xDelta);  /* Set the color */
                  
                  /* Look for color index or pure RGB background color */
                  /* Use controlType as var to hold presparam so we need no new one */
                  if (WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, PP_BACKGROUNDCOLORINDEX, &yDelta,
                                        sizeof(controlType), &controlType, QPF_PURERGBCOLOR | QPF_ID2COLORINDEX))
                    { 
                      /* Presparam found */
                      xDelta = GpiQueryColorIndex(hps, 0, controlType);  /* find index from RGB */

#if 0                      
This removed to get PMView to work. 
                      if (xDelta == GPI_ALTERROR)
                        {
                          /* Catch error */
                          xDelta = WinQuerySysColor(HWND_DESKTOP, SYSCLR_BACKGROUND, 0);
                          xDelta = GpiQueryColorIndex(hps, 0, xDelta);  /* find index from RGB */
                          //xDelta=SYSCLR_BACKGROUND;
     
                        }
                      WinFillRect(hps, &rclUpdate, xDelta);  /* Clear background */

#endif
                      if (xDelta != GPI_ALTERROR)//PMView-workaround
                        WinFillRect(hps, &rclUpdate, xDelta);  /* Clear background */


                    }
                  else
                    {
                      
                      /* No presparam */
                      
                      xDelta = WinQuerySysColor(HWND_DESKTOP, SYSCLR_BACKGROUND, 0);
                      xDelta = GpiQueryColorIndex(hps, 0, xDelta);  /* find index from RGB */
                      
                      //SYSCLR_BACKGROUND;/* Use default color */
                      //  xDelta=SYSCLR_BACKGROUND;/* Use default color */
                      drawStyle &= ~DT_ERASERECT;
                      /* Set background bitmap here */
                      
                      
                      /* This is better for correct drawing but slows down the building of windows with a lot of text controls enormous */
                      /* I think we should activate this only on fast machines. */
                      /* [ Insert backround drawing code here ] */

#ifdef LIGHTNINGFAST 
                      /* Draw the background to clear it. */
                      // s.o. hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);
                      /* Draw background */
                             

                       /************/

                      //**NOTE**: Ideally this should check if frame backgrounds are enabled.  If not enabled fill the
                      //              window with the background color.  If enabled, copy the contents of the frame ps
                      //              into the button ps.
                      
                      //!!!!!!!!!
                      //hwndFrame=WinQueryWindow(hwnd,QW_PARENT);                     
                   
                      hpsFrame = WinGetPS(hwndFrame);
                      rclFrame=rclUpdate;
                      WinMapWindowPoints(hwnd, hwndFrame, (PPOINTL) & rclFrame, 2);
                        
#if 0                    
                      if(pCBZData->bFrameBackgroundEnabled) {
                        //copy the contents of the frame ps to the new ps
                        
                                                                     
                        points[0].x = rclUpdate.xLeft;
                        points[0].y = rclUpdate.yBottom;
                        points[1].x = rclUpdate.xRight;
                        points[1].y = rclUpdate.yTop;
                        points[2].x = rclFrame.xLeft;
                        points[2].y = rclFrame.yBottom;
                        points[3].x = rclFrame.xRight;
                        points[3].y = rclFrame.yTop;
                        //      GpiBitBlt(hpsFrame, hpsFrame, 4, points, ROP_SRCCOPY, BBO_IGNORE);
                        GpiBitBlt(hps, hpsFrame, 4, points, ROP_SRCCOPY, BBO_IGNORE);
                     
                      }
                      else
                        WinFillRect(hpsFrame, &rclUpdate, WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0));                     
#endif

                      /*************/

                        TRY_LOUD(excWM_PAINT){ // Exception handler from XFolder
                          CandyBarZFramePaintBackgroundProc(hwndFrame, pCBZData, &rclFrame, hpsFrame);
                        }            
                        CATCH(excWM_PAINT) {
                        }
                        END_CATCH;
                        
                        WinReleasePS(hpsFrame);
#endif
                        
                    }
                  GpiSetBackColor(hps, xDelta);  /* Set the color */
               
                  if(drawStyle & DT_WORDBREAK) {
                    if (textLength)
                      {
                        for (hTotalDrawn = 0; hTotalDrawn != textLength; rclWindow.yTop -= (charSize.cy + 1))
                          {
                            /* if (rclWindow.yTop < rclWindow.yBottom + 1)
                               rclWindow.yTop += (charSize.cy + 1);*/
                            hDrawn = WinDrawText(hps, textLength - hTotalDrawn, textPtr + hTotalDrawn, &rclWindow, 0,
                                                 0, drawStyle | DT_TEXTATTRS | DT_WORDBREAK);
                            if(hDrawn)
                              hTotalDrawn += hDrawn;
                            else 
                              break;
                          }       // for(hTotalDrawn=0;hTotalDrawn!=...
                      }
                  }
                  else {
                    WinDrawText(hps, textLength, textPtr , &rclWindow, 0,
                                0, drawStyle | DT_TEXTATTRS );
                  }
 
                  //check for disabled windows.
                    
                  if(!WinIsWindowEnabled(hwnd))
                    {
                      POINTL pointl;
                      WinQueryWindowRect(hwnd, &rclWindow);
                      //GpiSetPattern(hps,PATSYM_HALFTONE);
                      GpiSetPattern(hps,PATSYM_DENSE7);
                      pointl.x = rclWindow.xLeft;
                      pointl.y = rclWindow.yBottom;
                      GpiMove(hps, &pointl);
                      pointl.x = rclWindow.xRight - 1;
                      pointl.y = rclWindow.yTop - 1;
                      GpiBox(hps,DRO_FILL,&pointl,1,1);// Draw as disabled if not enabled using PATSYM_XX
                    }
                  else
                    {
                      GpiSetPattern(hps,PATSYM_BLANK);
                    }
                  /* !!!!!!!!!!!!!!!!!!!!!!!!!!!  NEW  !!!!!*/
                  WinEndPaint(hps);
                  
                  return ((MRESULT) 0);
                }
                
            case SS_GROUPBOX:
                {
                    hps = WinBeginPaint(hwnd, NULLHANDLE, &rclWindow);
                    WinQueryWindowRect(hwnd, &rclWindow);
                    // Get Textboxdimensions
                    WinQueryWindowText(hwnd, sizeof(textBuffer), textBuffer);
                    textLength = strlen(textBuffer);  // ACHTUNG!!!!! SYSTEMFUNKTION VERWENDEN??!!

                    GpiQueryTextBox(hps, textLength, textBuffer, TXTBOX_COUNT, points);
                    GpiQueryDefCharBox(hps, &charSize);

                    pointl.y = rclWindow.yTop - charSize.cy / 2 - 3;
                    pointl.x = 3;
                    GpiMove(hps, &pointl);
                    GpiSetColor(hps, SYSCLR_BUTTONLIGHT);  // Change to SYSCLR use

                    pointl.x = 1;
                    GpiLine(hps, &pointl);
                    // left
                    pointl.y = 0;
                    GpiLine(hps, &pointl);
                    // bottom
                    pointl.x = rclWindow.xRight - 1;
                    GpiLine(hps, &pointl);
                    // right
                    pointl.y = rclWindow.yTop - charSize.cy / 2 - 3;
                    GpiLine(hps, &pointl);
                    xDelta = points[TXTBOX_TOPRIGHT].x + 1;
                    if (xDelta == 1)
                        xDelta = -1;
                    pointl.x = xDelta + 5;
                    GpiLine(hps, &pointl);
                    pointl.y += 1;
                    GpiMove(hps, &pointl);
                    GpiSetColor(hps, SYSCLR_WINDOWFRAME);
                    pointl.x = rclWindow.xRight - 2;
                    GpiLine(hps, &pointl);
                    pointl.y = 1;
                    GpiLine(hps, &pointl);
                    pointl.x = 0;
                    GpiLine(hps, &pointl);
                    pointl.y = rclWindow.yTop - charSize.cy / 2 - 2;
                    GpiLine(hps, &pointl);
                    pointl.x = 3;
                    GpiLine(hps, &pointl);
                    //pointl.y=rcl.yTop-charSize.cy/2-2;
                    //rcl.yTop-=(points[TXTBOX_TOPLEFT].y-points[TXTBOX_BOTTOMLEFT].y)/2;
                    // Define Textbox
                    // rcl.yTop+=(points[TXTBOX_TOPLEFT].y-points[TXTBOX_BOTTOMLEFT].y)/2;
                    rclWindow.xRight -= 7;
                    rclWindow.xLeft = 5;
                    rclWindow.yBottom = 0;
                    /* Looks like a senseless second call but is necessary */
                    WinQueryWindowText(hwnd, sizeof(textBuffer), textBuffer);
                    WinDrawText(hps, textLength, textBuffer, &rclWindow, SYSCLR_WINDOWSTATICTEXT,
                                0, drawStyle);
                    /* !!!!!!!!!!!!!!!!!!!!!!!!!!!  NEW  !!!!!*/
                    //check for disabled windows.
                    if(!WinIsWindowEnabled(hwnd))
                      {
                        POINTL pointl;
                        //GpiSetPattern(hps,PATSYM_HALFTONE);
                        GpiSetPattern(hps,PATSYM_DENSE7);
                        pointl.x = rclWindow.xLeft;
                        pointl.y = rclWindow.yBottom;
                        GpiMove(hps, &pointl);
                        pointl.x = rclWindow.xRight - 1;
                        pointl.y = rclWindow.yTop - 1;
                        GpiBox(hps,DRO_FILL,&pointl,1,1);// Draw as disabled if not enabled using PATSYM_XX
                    }
                    else
                    {
                        GpiSetPattern(hps,PATSYM_BLANK);
                    }

                    WinEndPaint(hps);
                    return ((MRESULT) FALSE);
                }
            }                   //end of switch (controlType)

            break;
        }                       //end of WM_PAINT

        break;
    }
    return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
}

#endif
/*************************************************************************************************************

    $Id: candybarzstaticproc.c,v 1.7 2000/10/21 10:12:17 cwohlgemuth Exp $

    $Log: candybarzstaticproc.c,v $
    Revision 1.7  2000/10/21 10:12:17  cwohlgemuth
    Added exception handlers

    Revision 1.6  2000/09/24 21:00:37  cwohlgemuth
    Bugfix. Text controls sometimes had no custom bg.

    Revision 1.5  2000/09/09 22:59:18  cwohlgemuth
    Bugfix for painting failures. Support for enabling/disabling added.

    Revision 1.4  2000/09/02 13:10:20  cwohlgemuth
    Minor speed improvement

    Revision 1.3  2000/05/10 15:43:52  cwohlgemuth
    activated and modified CB static control procedure to support custom frame BG (CW)

    Revision 1.2  1999/12/29 19:38:08  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:47:56  ktk
    Import

    Revision 1.12  1998/10/17 19:31:21  enorman
    Made some changes to eliminate PSRT.DLL
    Revision 1.11  1998/09/30 05:09:05  mwagner
    Made the PMView fix optional
    Revision 1.10  1998/09/22 12:09:15  mwagner
    Added better dialog support
    Revision 1.7  1998/09/12 10:04:54  pgarner
    Added copyright notice
    Revision 1.6  1998/09/12 07:35:45  mwagner
    Added support for bitmap images
    Revision 1.3  1998/08/31 19:35:27  enorman
    Finished the implementation of 3D Text
    Revision 1.2  1998/06/29 15:30:52  mwagner
    Matts S3 and PMView fixes
    Revision 1.1  1998/06/08 14:18:56  pgarner
    Initial Checkin

*************************************************************************************************************/
