/******************************************************************************
  Copyright Netlabs, 1998, this code may not be used for any military purpose
******************************************************************************/

#include "candybarz.h"

#define INCL_DOSEXCEPTIONS
#define INCL_DOSPROCESS
#include <os2.h>
#include <stdio.h>
#include <setjmp.h>
#include <assert.h>
#include "except.h"

/********************************************************************
* CandyBarZTBProc: This is the Replacement Window Procedure for the *
*   WC_TITLEBAR Control.                                            *
* Messages Processed:                                               *
*   WM_PAINT: Paint the Titlebar based on current settings          *
*   TBM_SETHILITE: Switch between Active and Inactive State, redraw *
********************************************************************/
MRESULT EXPENTRY CandyBarZTBProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CBZDATA     *pCBZData = NULL;   //current parameters for this window.
                                    // stored in the window words of the
                                    // toplevel frame window.

    CBZSHARE    *pCBZShare = NULL;  //structure in shared memory that holds the
                                    // default settings and procedure addresses.

    PFNWP       pfnOrigWndProc;     //address of original window procedure
    ULONG       ulDataOffset;       //offset in the window words of pCBZData

    int         i;                  //helper variables
    RECTL       rectl = {0};

    MRESULT     rc;
    BOOL       bReady;

    HWND        hwndFocus = WinQueryFocus(HWND_DESKTOP);
    HWND        hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);

    //The shared memory area MUST exist, or we won't be able to query the settings
    //  for this window, or the address of the original window procedure.
    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ) == NO_ERROR)
    {
        ulDataOffset    = pCBZShare->ulDataOffset;   //get offset to pCBZData
        pfnOrigWndProc  = pCBZShare->pTBOrigWndProc; //get address of original wnd proc
        DosFreeMem(pCBZShare);  //release the shared memory.

        //Query the pCBZData structure and check to see if CandyBarZ is enabled for
        //  this titlebar.
        if ((pCBZData = WinQueryWindowPtr(hwndFrame, ulDataOffset)) == NULL ||
                !(pCBZData->bTitlebarEnabled) || (pCBZData->sTitlebarIndex == pCBZData->sFrameBackgroundIndex) )
        {
            //CandyBarZ not enabled, so pass the message on to the original wnd procedure.
            return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
        }
        else    //CandyBarZ is Enabled!
        {

            //pass the message to each of the titlebar plugins.
            // titlebar plugins are the plugins from sTitleBarIndex up to sFrameBackgroundIndex
          bReady=FALSE;
          TRY_LOUD(excTB) // Exception handler from XFolder
            {              
              for (i = pCBZData->sTitlebarIndex; i < pCBZData->sFrameBackgroundIndex; i++)
                {
                  if ( pCBZData->Plugins[i].pPluginWndProc(hwnd, msg, mp1, mp2, pCBZData->Plugins[i].pData, pfnOrigWndProc, &rc) == TRUE ) {
                    bReady=TRUE;
                    break;
                  }
                }/* for */
            }
            CATCH(excTB){
            }
            END_CATCH;
            if(bReady)
              return rc;
        }
    }
    else    //Unable to get Shared Memory, so try the default Window Procedure.
      return WinDefWindowProc(hwnd, msg, mp1, mp2);
    

    switch (msg)
    {

        case WM_CREATE:
        {
            WinSetWindowULong(hwnd, QWL_STYLE, WinQueryWindowULong(hwnd, QWL_STYLE) | WS_CLIPCHILDREN);
        }
        break;

        //Message indicating that the titlebar should be redrawn in either the Active
        //  or Inactive state based on the value of mp1.
        case TBM_SETHILITE:
        {
          bReady=TRUE;
          // paint entire rectangle
          TRY_LOUD(excTBM_SETHILITE) // Exception handler from XFolder
            {              
              if (!CandyBarZTBPaintProc(hwnd, pCBZData, TRUE, SHORT1FROMMP(mp1) ? TRUE : FALSE))
                {
                  //Error in CandyBarZTBPaintProc so fall back to original window procedure.
                  bReady=FALSE;
                }
            }
            CATCH(excTBM_SETHILITE){
              bReady=FALSE;
            }
            END_CATCH;

            if(!bReady) {
              break;
            }
            else
            {
              //Painting was sucessful, so return.
              return ((MRESULT) TRUE);
            }
        }//end TBM_SETHILITE
        break;

        //Paint message for the titlebar control.
        case WM_PAINT:
        {
            hwndFrame = WinQueryWindow(hwnd, QW_PARENT);
            // don't paint if update rectangle is empty or crappy.  i don't know how they got negative
            // numbers in there, but....
            WinQueryUpdateRect(hwnd, &rectl);
            if (rectl.xRight <= 0 || rectl.yTop <= 0)
            {
                WinQueryWindowRect(hwnd, &rectl);
                WinValidateRect(hwnd, &rectl, FALSE);
                return ((MRESULT) 0);
            }
            // try to determine active or inactive.  the current criteria are
            // 1. Is the focus window a child of our frame
            // 2. Is the frame owner of the focus window a child of our frame
            // 3. Is the owner of the frame owner of the focus window a child of our frame
            // 4. Is the display active flag set in the window styles
            if (WinIsChild(hwndFocus, hwndFrame) ||
                    WinIsChild(WinQueryWindow(hwndFocus, QW_FRAMEOWNER), hwndFrame) ||
                    WinIsChild(WinQueryWindow(WinQueryWindow(hwndFocus, QW_FRAMEOWNER), QW_OWNER),
                               hwndFrame) ||
                    (WinQueryWindowUShort(hwndFrame, QWS_FLAGS) & FF_ACTIVE))
            {

              bReady=TRUE;
              // paint entire rectangle
              TRY_LOUD(excWM_PAINT1) // Exception handler from XFolder
                {              
                  //Draw titlebar in the Active State
                  if (!CandyBarZTBPaintProc(hwnd, pCBZData, FALSE, TRUE))
                    {
                      //Error during painting, so fall back to original window procedure.
                      bReady=FALSE;
                    }
                }
                CATCH(excWM_PAINT1){
                  bReady=FALSE;
                }
                END_CATCH;

                if(!bReady) {
                  break;
                }
                else
                  {
                    //Painting sucessful so return.
                    return ((MRESULT) 0);
                  }

            }
            else
            {
              bReady=TRUE;
              // paint entire rectangle
              TRY_LOUD(excWM_PAINT2) // Exception handler from XFolder
                {
                  //Draw titlebar in the Inactive State.
                  if (!CandyBarZTBPaintProc(hwnd, pCBZData, FALSE, FALSE))
                    {
                      //Error during painting, so fall back to original window procedure.
                      bReady=FALSE;
                    }
                }
                CATCH(excWM_PAINT2){
                  bReady=FALSE;
                }
                END_CATCH;
                
                if(!bReady) {
                  break;
                }
                else
                  {
                    //Painting sucessful so return.
                    return ((MRESULT) 0);
                  }
            }
        }//end WM_PAINT
        break;

    }//end switch

    //pass all unhandled messages on to the original window procedure!
    return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
}

/*************************************************************************************************************

    $Id: candybarzTBproc.c,v 1.4 2000/10/21 10:13:01 cwohlgemuth Exp $

    $Log: candybarzTBproc.c,v $
    Revision 1.4  2000/10/21 10:13:01  cwohlgemuth
    Added exception handlers

    Revision 1.3  2000/03/26 06:51:21  enorman
    resyncing with my local tree.

    Revision 1.2  1999/12/29 19:38:09  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:47:57  ktk
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
