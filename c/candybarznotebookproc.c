
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

extern CBZSHARE    CBZShare;

/********************************************************************
* CandyBarZNotebookProc: This is the Replacement Window Procedure   *
*   for the WC_NOTEBOOK Control.                                    *
* Messages Processed:                                              *
*   WM_ENABLE: Same as WM_SETWINDOWPARAMS                          *
*   WM_PAINT: Paint the control                                    *
********************************************************************/
MRESULT EXPENTRY CandyBarZNotebookProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CBZDATA     *pCBZData = NULL;   //current parameters for this window.
                                    // stored in the window words of toplevel
                                    // frame window.
    PFNWP       pfnOrigWndProc;     //address of original window procedure
    ULONG       ulDataOffset;       //offset in the window words of pCBZData

    int         i;

    HWND        hwndFrame;

    MRESULT     rc;


    pfnOrigWndProc  = CBZShare.pNotebookOrigWndProc; //get address of original wnd proc.
    ulDataOffset    = CBZShare.ulDataOffset;      //get offset to pCBZData


    hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);
    if ((pCBZData = WinQueryWindowPtr(hwndFrame, ulDataOffset)) == NULL)
      {
        return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
      }
    else
      {
        if(!pCBZData->bNoteBookBackgroundEnabled)
          return (pfnOrigWndProc(hwnd, msg, mp1, mp2));

        //pass messages to plugins for optional processing
        for (i = pCBZData->sNoteBookBackgroundIndex; i < pCBZData->sScrollBarIndex; i++)
          {

            if ( pCBZData->Plugins[i].pPluginWndProc(hwnd, msg, mp1, mp2, pCBZData->Plugins[i].pData, 
                                                     pfnOrigWndProc, &rc) == TRUE )
              return rc;
          }
      }
    
    
    //process the message
    switch (msg)
      {
      case WM_PAINT:

        if (!CandyBarZNotebookPaintProc(hwnd, pCBZData, 1) )
          {
            //painting failed so fall back to original window procedure
            return pfnOrigWndProc(hwnd, msg, mp1, mp2);
          }       
        
        return ((MRESULT) 0);
      default:
        break;
      }//end switch
    
    //pass all unhandled messages on to the original window procedure!
    return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
}






