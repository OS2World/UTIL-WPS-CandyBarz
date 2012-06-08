
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
extern LONG lcxIcon;
extern LONG lcyIcon;
extern LONG lcxPointer;
extern LONG lcyPointer;

/********************************************************************
* CandyBarZButtonProc: This is the Replacement Window Procedure     *
*   for the WC_BUTTON Control.                                      *
* Messages Processed:                                               *
*   WM_SETWINDOWPARAMS: The Background needs to be redrawn to       *
*       overwrite the previous button.(radio/checkbox text is       *
*       transparent, so it draws on top of the old text if we don't *
*       clear the previous button)                                  *
*   WM_ENABLE: Same as WM_SETWINDOWPARAMS                           *
*   WM_PAINT: Paint the Button control                              *
*   BM_SETHILITE: Draw button in hilited state                      *
*   BM_SETDEFAULT: Draw button in default state                     *
*   BM_SETCHECK: Draw button in checked state                       *
********************************************************************/
MRESULT EXPENTRY CandyBarZButtonProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CBZDATA     *pCBZData = NULL;   //current parameters for this window.
                                    // stored in the window words of toplevel
                                    // frame window.
    PFNWP       pfnOrigWndProc;     //address of original window procedure
    ULONG       ulDataOffset;       //offset in the window words of pCBZData

    RECTL       rectl;      //helper variables
    HPS         hps;
    ULONG       style;
    ULONG       controlType;
    ULONG       attrFound, lColor;
    short       checkState;
    int         i;
    HWND        hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);
    MRESULT     rc;
    BOOL       bReady;

    pfnOrigWndProc  = CBZShare.pButtonOrigWndProc; //get address of original wnd proc.
    ulDataOffset    = CBZShare.ulDataOffset;      //get offset to pCBZData


        if ((pCBZData = WinQueryWindowPtr(hwndFrame, ulDataOffset)) == NULL)
        {
            return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
        }
        else
        {
          style = WinQueryWindowULong(hwnd, QWL_STYLE);  // Query style bits
          controlType = style & (ULONG) 7;  // Mask control type style bits

            if ( (controlType == BS_AUTORADIOBUTTON) || (controlType == BS_RADIOBUTTON) )
            {
                //make sure CandyBarZ is enabled for RadioButtons.  If not pass all
                // messages on to the original window procedure.
                if ( !(pCBZData->bRadioButtonEnabled) ||
                    (pCBZData->sRadioButtonIndex == pCBZData->sCheckBoxIndex) )
                {
                    return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
                }

                bReady=FALSE;
                TRY_LOUD(excButton) // Exception handler from XFolder
                  {
                    //pass messages to plugins for optional processing
                    for (i = pCBZData->sRadioButtonIndex; i < pCBZData->sCheckBoxIndex; i++)
                      {
                        if ( pCBZData->Plugins[i].pPluginWndProc(hwnd, msg, mp1, mp2, pCBZData->Plugins[i].pData, 
                                                                 pfnOrigWndProc, &rc) == TRUE ) {
                          bReady=TRUE;
                          break;
                        }
                      }/* For */
                  }/* Try */
                  CATCH(excButton){
                  }
                  END_CATCH;
                  if(bReady)
                    return rc;
            }
            else if ( (controlType == BS_AUTOCHECKBOX) || (controlType == BS_CHECKBOX) ||
                      (controlType == BS_AUTO3STATE) )
              {
                //make sure CandyBarZ is enabled for CheckBoxes.  If not pass all
                // messages on to the original window procedure.
                if ( !(pCBZData->bCheckBoxEnabled) ||
                     (pCBZData->sCheckBoxIndex == pCBZData->sMinMaxIndex) )
                  {
                    return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
                  }

                bReady=FALSE;
                TRY_LOUD(excButton2) // Exception handler from XFolder
                  {
                    //pass messages to plugins for optional processing
                    for (i = pCBZData->sCheckBoxIndex; i < pCBZData->sMinMaxIndex; i++)
                      {
                        if ( pCBZData->Plugins[i].pPluginWndProc(hwnd, msg, mp1, mp2, pCBZData->Plugins[i].pData, 
                                                                 pfnOrigWndProc, &rc) == TRUE ) {
                          bReady=TRUE;
                          break;
                        }
                      } /* For */
                  }/* Try */
                  CATCH(excButton2){
                  }
                  END_CATCH;
                  if(bReady)
                    return rc;
              }
            else if (controlType == BS_PUSHBUTTON )
              {
                //make sure CandyBarZ is enabled for PushButtons.  If not pass all
                // messages on to the original window procedure.
              if ( !(pCBZData->bPushButtonEnabled) ||
                     (pCBZData->sPushButtonIndex == pCBZData->sRadioButtonIndex) || /*(style & (ULONG) (BS_BITMAP))*/
                     (style & (ULONG)(BS_BITMAP|BS_MINIICON|BS_ICON)) )
                {
                    return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
                }

              bReady=FALSE;
              TRY_LOUD(excButton3) // Exception handler from XFolder
                {
                  //pass messages to plugins for optional processing
                  for (i = pCBZData->sPushButtonIndex; i < pCBZData->sRadioButtonIndex; i++)
                    {
                      if ( pCBZData->Plugins[i].pPluginWndProc(hwnd, msg, mp1, mp2, pCBZData->Plugins[i].pData, 
                                                               pfnOrigWndProc, &rc) == TRUE ) {
                        bReady=TRUE;
                        break;
                      }
                    } /* For */
                }/* Try */
                CATCH(excButton3){
                }
                END_CATCH;
                if(bReady)
                  return rc;
              }
        }
        
        //process the message
        switch (msg)
          {
            //force the background under the button to be redrawn because
            // something has changed.
          case WM_SETWINDOWPARAMS:
          case WM_ENABLE:
            {
              RECTL rclFrame;
              HPS hpsFrame;
              HWND hwndFrame = WinQueryWindow(hwnd, QW_PARENT);
              
              
              // Draw background
              hpsFrame = WinGetPS(hwndFrame);

              TRY_LOUD(excWMEnable){ // Exception handler from XFolder
                WinQueryWindowRect(hwnd, &rclFrame);
                WinMapWindowPoints(hwnd, hwndFrame, (PPOINTL) & rclFrame, 2);
                CandyBarZFramePaintBackgroundProc(hwndFrame, pCBZData, &rclFrame, hpsFrame);
              }           
              CATCH(excWMEnable) {
              }
              END_CATCH;
              
              WinReleasePS(hpsFrame);
            }//let it fall through to the painting stuff
            
          case WM_PAINT:
          case BM_SETHILITE:
          case BM_SETDEFAULT:
          case BM_SETCHECK:
            {
              BOOL bError;
              
              if (!WinIsWindowVisible(hwnd))
                return pfnOrigWndProc(hwnd, msg, mp1, mp2);
              if (!WinIsWindowShowing(hwnd))
                return pfnOrigWndProc(hwnd, msg, mp1, mp2);
              
              WinEnableWindowUpdate(hwnd, FALSE);
              pfnOrigWndProc(hwnd, msg, mp1, mp2);
              WinEnableWindowUpdate(hwnd, TRUE);
              
#if 0     
              style = WinQueryWindowULong(hwnd, QWL_STYLE);  // Query style bits
              controlType = style & (ULONG) 7;  // Mask control type style bits
#endif
              if ((controlType == BS_AUTORADIOBUTTON) || (controlType == BS_RADIOBUTTON) ||
                  (controlType == BS_AUTOCHECKBOX) || (controlType == BS_CHECKBOX) ||
                  (controlType == BS_AUTO3STATE) )
                {
                  checkState = SHORT1FROMMR(WinSendMsg(hwnd,BM_QUERYCHECK,NULL,NULL));
                  if ( SHORT1FROMMR(WinSendMsg(hwnd, BM_QUERYHILITE, NULL, NULL)) == TRUE )
                    checkState += 2;
                  
                  bError=FALSE;
                  if ( (controlType == BS_AUTORADIOBUTTON) || (controlType == BS_RADIOBUTTON))
                    {
                      TRY_LOUD(excBtnPaint1) // Exception handler from XFolder
                        {
                          
                          if (!CandyBarZRadioButtonPaintProc(hwnd, pCBZData, checkState) )
                            {
                              //painting failed so fall back to original window procedure
                              bError=TRUE;
                            }
                        }
                        CATCH(excBtnPaint1){
                          bError=TRUE;
                        }
                        END_CATCH;
                        if(bError)
                          return pfnOrigWndProc(hwnd, msg, mp1, mp2);
                    }
                  else //it is a checkbox
                    {
                      TRY_LOUD(excBtnPaint2) // Exception handler from XFolder
                        { 
                          if (!CandyBarZCheckBoxPaintProc(hwnd, pCBZData, checkState) )
                            {
                              //painting failed so fall back to original window procedure
                              bError=TRUE;
                            }
                        }
                        CATCH(excBtnPaint2){
                          bError=TRUE;
                        }
                        END_CATCH;
                        if(bError)
                          return pfnOrigWndProc(hwnd, msg, mp1, mp2);
                    }
                  //check for disabled windows.
                  hps = WinGetPS(hwnd);
                  if(!WinIsWindowEnabled(hwnd))
                    {
                      POINTL pointl;
                      WinQueryWindowRect(hwnd, &rectl);
                      //GpiSetPattern(hps,PATSYM_HALFTONE);
                      GpiSetPattern(hps,PATSYM_DENSE7);
                      pointl.x = rectl.xLeft;
                      pointl.y = rectl.yBottom;
                      GpiMove(hps, &pointl);
                      pointl.x = rectl.xRight - 1;
                      pointl.y = rectl.yTop - 1;
                      GpiBox(hps,DRO_FILL,&pointl,1,1);// Draw as disabled if not enabled using PATSYM_XX
                    }
                  else
                    {
                      GpiSetPattern(hps,PATSYM_BLANK);
                    }
                  WinReleasePS(hps);
                  
                  return ((MRESULT) 0);
                }
              else if (controlType == BS_PUSHBUTTON )
                {
                  //if there is an explicit background color set, honor it.
              if ( (WinQueryPresParam(hwnd, PP_BACKGROUNDCOLOR, 0, &attrFound, sizeof(attrFound),
                                      &(lColor), QPF_PURERGBCOLOR)) > 0)
                return pfnOrigWndProc(hwnd, msg, mp1, mp2);//break;
              
              bError=FALSE;
              TRY_LOUD(excBtnPaint3) // Exception handler from XFolder
                { 
                  // paint
                  if (!CandyBarZPushButtonPaintProc(hwnd, pCBZData, !SHORT1FROMMR(WinSendMsg(hwnd,BM_QUERYHILITE,NULL,NULL))) )
                    {
                      bError=TRUE;
                    }
                }
                CATCH(excBtnPaint3){
                  bError=TRUE;
                }
                END_CATCH;
                
                if (bError )
                  {
                    return pfnOrigWndProc(hwnd, msg, mp1, mp2);//break;
                  }
                else
                {
                  hps = WinGetPS(hwnd);
#if 0
                  /* The PushButton was painted */
                  if(style & (ULONG)(BS_MINIICON|BS_ICON))
                    {
                      WNDPARAMS wndParams={0};
                      BTNCDATA  btnData= {0};
                      POINTL ptl= {0};
                      LONG x,y;
                      
                      WinQueryWindowRect(hwnd, &rectl);
                      
                      wndParams.fsStatus=WPM_CTLDATA;
                      wndParams.cbCtlData=sizeof(BTNCDATA);
                      wndParams.pCtlData=&btnData;
                      if(WinSendMsg(hwnd,WM_QUERYWINDOWPARAMS,&wndParams,(MPARAM)0)) {
                        /*We have a bitmap or alike, paint it */
                        if(style & (ULONG) BS_BITMAP) {
                          if(btnData.hImage) {
                            ptl.x=((rectl.xRight-rectl.xLeft)-lcxIcon)/2;
                            ptl.y=((rectl.yTop-rectl.yBottom)-lcyIcon)/2;
                            WinDrawBitmap(hps,btnData.hImage,NULL,&ptl,0,0,DBM_NORMAL);
                          }
                        }
                        else if(style & (ULONG) BS_MINIICON) {
                          if(btnData.hImage) {
                            x=((rectl.xRight-rectl.xLeft)-lcxPointer)/2;
                            y=((rectl.yTop-rectl.yBottom)-lcyPointer)/2;
                            WinDrawPointer(hps, x, y,btnData.hImage,DP_MINI);
                          }
                        }
                        else if(style & (ULONG) BS_ICON) {
                          if(btnData.hImage) {// Ok
                            x=((rectl.xRight-rectl.xLeft)-lcxIcon)/2;
                            y=((rectl.yTop-rectl.yBottom)-lcyIcon)/2;
                            WinDrawPointer(hps, x, y,btnData.hImage,DP_NORMAL);
                          }                                    
                        }
                      }
                    }
#endif
                  
                  if(!WinIsWindowEnabled(hwnd))
                    {
                      POINTL pointl;
                      WinQueryWindowRect(hwnd, &rectl);
                      //GpiSetPattern(hps,PATSYM_HALFTONE);
                      GpiSetPattern(hps,PATSYM_DENSE7);
                      pointl.x = rectl.xLeft;
                      pointl.y = rectl.yBottom;
                      GpiMove(hps, &pointl);
                      pointl.x = rectl.xRight - 1;
                      pointl.y = rectl.yTop - 1;
                      GpiBox(hps,DRO_FILL,&pointl,1,1);// Draw as disabled if not enabled using PATSYM_XX
                    }
                  else
                    {
                      GpiSetPattern(hps,PATSYM_BLANK);
                    }
                  
                  WinReleasePS(hps);
                  
                  return ((MRESULT) 0);
                }
            }
        }
        break;

    }//end switch

    //pass all unhandled messages on to the original window procedure!
    return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
}


/*************************************************************************************************************

    $Id: candybarzbuttonproc.c,v 1.7 2000/10/21 10:10:19 cwohlgemuth Exp $

    $Log: candybarzbuttonproc.c,v $
    Revision 1.7  2000/10/21 10:10:19  cwohlgemuth
    Added exception handler

    Revision 1.6  2000/09/24 20:55:36  cwohlgemuth
    Preparation for icons on custom drawn buttons.

    Revision 1.5  2000/09/09 23:11:52  cwohlgemuth
    Changed halftone pattern for disabled state (brighter)

    Revision 1.4  2000/09/02 13:09:07  cwohlgemuth
    Minor speed improvement

    Revision 1.3  2000/05/10 15:46:40  cwohlgemuth
    Application icons and BMPs for buttons were lost when using custom BG (CW)

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
