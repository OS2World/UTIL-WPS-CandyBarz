
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


extern ULONG       ulOffsetPrivateData;
extern CBZSHARE    CBZShare;
static BOOL bAcceptBroadcast=TRUE;

/* This is the handle of the WM object window.
   We send a msg to this window if we want to build a frame decoration. */
HWND hwndWindowManagerObject;

int APIENTRY setWindowManagerHwnd(HWND hwnd)
{
  HATOMTBL    hAtomTbl;
  //char msg[200];
  CBZSHARE    *pCBZShare = NULL;
  ATOM WMgrAtom;
  int rc;

  hAtomTbl = WinQuerySystemAtomTable();

  if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ|PAG_WRITE) != NO_ERROR)
    {

      DosBeep(100,600);
      /* If we have no atom, we can't communicate, so don't accept the handle. */
      return 1;
    }


  pCBZShare->hwndWindowManagerObject=hwnd;

  hwndWindowManagerObject=hwnd;

  rc=0;
  if(hwnd ) {
    if ((WMgrAtom = WinAddAtom(hAtomTbl, WMGR_MESSAGE_ATOM)) == 0)
      {
        DosBeep(100,400);
        rc= 1;
      }
    if(WMgrAtom) {
      /*   sprintf(msg,"setWindowManagerHwnd(HWND hwnd): HWND is %x:",hwndWindowManagerObject);*/
      //WinMessageBox(HWND_DESKTOP,HWND_DESKTOP,msg,"",123,MB_OK);
      pCBZShare->WMgrAtom=WMgrAtom;
    }
  }
  else {
    WinDeleteAtom(hAtomTbl, pCBZShare->WMgrAtom);
    pCBZShare->WMgrAtom=0;
    rc=0;
  }
  DosFreeMem(pCBZShare);

  /* If we have no atom, we can't communicate, so don't accept the handle. */
  return rc;
}


/********************************************************************
* CandyBarZFrameProc: This is the Replacement Window Procedure for  *
*   the WC_FRAME Control.                                           *
* Messages Processed:                                               *
*   WM_PAINT: Paint the Frame Border based on current settings      *
*   WM_ACTIVATE: Switch Frame Border between active/inactive states *
*   WM_ERASEBACKGROUND: Paint the frame background based on the     *
*           current settings.                                       *
*   WM_DRAWITEM: OwnerDraw Messages for the Min/Max/Close buttons.  *
*   WM_CREATE: Allocate Memory and setup resources.                 *
*   WM_DESTROY: Free Memory and resources.                          *
********************************************************************/
MRESULT EXPENTRY CandyBarZFrameProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CBZDATA     *pCBZData = NULL;   //current parameters for this window.
                                    // stored in the window words of the
                                    // toplevel frame window.

    //CBZSHARE    *pCBZShare = NULL;  //structure in shared memory that holds the
                                    // default settings and procedure addresses.

    PFNWP       pfnOrigWndProc;     //address of original window procedure
    ULONG       ulDataOffset;       //offset in the window words of pCBZData

    int         i;                  //helper variables
    HATOMTBL    hAtomTbl;
    ATOM        Atom     ;//= WinFindAtom(hAtomTbl, PSTB_COLORCHANGE_ATOM);

    //    CBZSHARE    *ptrCBZShare;
#if 0
    ATOM        WMgrAtom;
    PCREATESTRUCT      pCreate;
#endif
    HPS         hps;
    RECTL       *prectl, rectl;
    USHORT      flags, state;
    HWND        hwndMenu;
    MENUITEM    mi;
    MRESULT     rc;

    HWND        hwndFocus; // = WinQueryFocus(HWND_DESKTOP);
    HWND        hwndFrame;// = CandyBarZGetTopLevelFrameHWND(hwnd);
    //SWP * pswp;
    BOOL       bReady;

    pfnOrigWndProc  = CBZShare.pFrameOrigWndProc; //get address of original wnd proc.
    ulDataOffset    = CBZShare.ulDataOffset;      //get offset to pCBZData


    /*********** Creating the window **************/
    if(msg==WM_CREATE) {
      hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);
      //only create settings if this window is the topmost frame window in the process
      if (hwnd == hwndFrame)
        {
          //init settings.
          CandyBarZWindowCreate(hwnd);
          /* Mark the frame as candybarz painted                              */
          /* This is always done on creation                                  */
          /* During the first WM_PAINT we check if the whole frame is covered */
          /* by a client and may disable custom painting                      */
          /* We may use this for disabled programs later, too                 */
          WinSetWindowULong(hwnd, ulOffsetPrivateData+offsetof(PRIVATEFRAMEDATA, bCustomPaint),TRUE);

          hAtomTbl = WinQuerySystemAtomTable();
          // add atom for this app
          if ((Atom = WinAddAtom(hAtomTbl, PSTB_COLORCHANGE_ATOM)) == 0)
            {
              return (FALSE);
            }
          WinSetWindowULong(hwnd,ulOffsetPrivateData+offsetof(PRIVATEFRAMEDATA, Atom),Atom);
        }

      //make sure pCBZData structure was created sucessfully
      if ( (pCBZData = WinQueryWindowPtr(hwndFrame, ulDataOffset)) == NULL )
        return (pfnOrigWndProc(hwnd, msg, mp1, mp2));

      /* Let the original wnd proc build the window */
      rc = (pfnOrigWndProc(hwnd, msg, mp1, mp2));

      //set the min/max buttons as ownerdrawn if bMinMaxEnabled is set
      if ( (pCBZData->bMinMaxEnabled) && (pCBZData->sMinMaxIndex != pCBZData->sMenuIndex) )
        {
          hwndMenu=WinWindowFromID(hwnd,FID_MINMAX);
          if(hwndMenu)
            {
              // Mark buttons as ownerdrawn
              if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_CLOSE_,FALSE),&mi))
                {
                  mi.afStyle|=MIS_OWNERDRAW;
                  WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                }
              if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_MINIMIZE,FALSE),&mi))
                {
                  mi.afStyle|=MIS_OWNERDRAW;
                  WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                }
              if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_MAXIMIZE,FALSE),&mi))
                {
                  mi.afStyle|=MIS_OWNERDRAW;
                  WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                }
              if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_HIDE,FALSE),&mi))
                {
                  mi.afStyle|=MIS_OWNERDRAW;
                  WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                }
              if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_RESTORE,FALSE),&mi))
                {
                  mi.afStyle|=MIS_OWNERDRAW;
                  WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                }
            }//end if (hwndMenu)
        }//end bMinMaxEnabled if


      /* Call the plugins */
      bReady=FALSE;
      TRY_LOUD(excCreate) // Exception handler from XFolder
        {
          for (i = pCBZData->sFrameBackgroundIndex; i < pCBZData->sPushButtonIndex; i++)
            {
              if ( pCBZData->Plugins[i].pPluginWndProc(hwnd, msg, mp1, mp2, pCBZData->Plugins[i].pData,
                                                       pfnOrigWndProc, &rc) == TRUE) {
                bReady=TRUE;
                break;
              }
            }/* for */
        }
        CATCH(excCreate){
        }
        END_CATCH;
        if(bReady)
          return rc;

#if 0
      /* Tell the window manager that there's a new toplevel frame */
      hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);
      if((hwnd == hwndFrame) && hwndWindowManagerObject) {
        pCreate=(PCREATESTRUCT)PVOIDFROMMP(mp2);
        WinPostMsg(hwndWindowManagerObject,WMgrAtom,MPFROMSHORT(11),MPFROMLONG(WinQueryWindowULong(hwnd,QWL_STYLE)));
        WinPostMsg(hwndWindowManagerObject,WMgrAtom,MPFROMSHORT(10),MPFROMLONG(pCreate->flStyle));
        //if((WinQueryWindowULong(hwnd,QWL_STYLE) & FCF_TASKLIST) && (pCreate->id!=5555))
        if(WinWindowFromID(hwnd,FID_TITLEBAR))
                WinPostMsg(hwndWindowManagerObject,WMgrAtom,MPFROMSHORT(WMGR_FRAMECREATED),MPFROMHWND(hwnd));

      }
#endif
      return rc;
    }
    /*********** End WM_CREATE **********/


    Atom=WinQueryWindowULong(hwnd,ulOffsetPrivateData+offsetof(PRIVATEFRAMEDATA, Atom));

    //if msg is WM_CREATE or Atom, then continue in case we are enabled on the fly.
    if (msg != Atom)
      {
        hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);
        //Query the pCBZData structure, if it doesn't exist then fallback to the
        //  original window procedure.
        if ((pCBZData = WinQueryWindowPtr(hwndFrame, ulDataOffset)) == NULL )
          {
            return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
          }
        else
          {
            //Need to pass to frame background plugins, and frame border plugins

                 if(!CBZShare.plugins.bFrameBackgroundEnabled && !CBZShare.plugins.bFrameBorderEnabled)
                 return (pfnOrigWndProc(hwnd, msg, mp1, mp2));

            // pass messages to the Plugins Window procedures
            bReady=FALSE;
            TRY_LOUD(excFrame) // Exception handler from XFolder
              {
                for (i = pCBZData->sFrameBackgroundIndex; i < pCBZData->sPushButtonIndex; i++)
                  {
                    if ( pCBZData->Plugins[i].pPluginWndProc(hwnd, msg, mp1, mp2, pCBZData->Plugins[i].pData,
                                                             pfnOrigWndProc, &rc) == TRUE) {
                      bReady=TRUE;
                      break;
                    }
                  }/* for */
              }
              CATCH(excFrame){

              }
              END_CATCH;
              if(bReady)
                return rc;

          } //end else
      }//end msg check if


#if 0
    if(msg==WMgrAtom) {
      switch(SHORT1FROMMP(mp1))
        {
        case WMGR_YOURWMFRAME:
          if(pCBZData)
            pCBZData->hwndWMgrFrame=HWNDFROMMP(mp2);
          return (MRESULT)0;
        default:
          return (MRESULT)0;
          break;
        }

    }
#endif

    switch (msg)
    {


      //Message to catch the OwnerDraw Messages of the Min/Max buttons
    case WM_DRAWITEM:
      {
        OWNERITEM *pOwnerItem;
        BOOL bSuccess;

        //make sure Min/Max enhancements are enabled
        if ( !(pCBZData->bMinMaxEnabled) || (pCBZData->sMinMaxIndex == pCBZData->sMenuIndex) )
          break;

        pOwnerItem=(POWNERITEM)PVOIDFROMMP(mp2);
        bSuccess=FALSE;
        TRY_LOUD(excDrawItem) // Exception handler from XFolder
          {

            if( pOwnerItem->idItem == SC_CLOSE_ )// Close button
              {
                //draw CloseButton
                if ( pOwnerItem->fsAttribute & MIA_HILITED )
                  {
                    if ( CandyBarZMinMaxPaintProc( pOwnerItem->hwnd, pCBZData, &(pOwnerItem->rclItem), pOwnerItem->hps, 1 ) )
                      {
                        pOwnerItem->fsAttributeOld = (pOwnerItem->fsAttribute &= ~MIA_HILITED);
                        bSuccess=TRUE;
                      }
                  }
                else
                  {
                    if ( CandyBarZMinMaxPaintProc( pOwnerItem->hwnd, pCBZData, &(pOwnerItem->rclItem), pOwnerItem->hps, 0 ) )
                      {
                        pOwnerItem->fsAttributeOld = (pOwnerItem->fsAttribute &= ~MIA_HILITED);
                        bSuccess=TRUE;
                      }
              }
              }
            else if( pOwnerItem->idItem == SC_MINIMIZE )// Minimize button
              {
                //draw Minimize Button
                if ( pOwnerItem->fsAttribute & MIA_HILITED )
                  {
                    if ( CandyBarZMinMaxPaintProc( pOwnerItem->hwnd, pCBZData, &(pOwnerItem->rclItem), pOwnerItem->hps, 3 ) )
                      {
                        pOwnerItem->fsAttributeOld = (pOwnerItem->fsAttribute &= ~MIA_HILITED);
                        bSuccess=TRUE;
                      }
                  }
                else
                  {
                    if ( CandyBarZMinMaxPaintProc( pOwnerItem->hwnd, pCBZData, &(pOwnerItem->rclItem), pOwnerItem->hps, 2 ) )
                      {
                        pOwnerItem->fsAttributeOld = (pOwnerItem->fsAttribute &= ~MIA_HILITED);
                        bSuccess=TRUE;
                      }
                  }
              }
            else if( pOwnerItem->idItem == SC_MAXIMIZE )// Maximize button
              {
                //draw Maximize Button
                if ( pOwnerItem->fsAttribute & MIA_HILITED )
                  {
                    if ( CandyBarZMinMaxPaintProc( pOwnerItem->hwnd, pCBZData, &(pOwnerItem->rclItem), pOwnerItem->hps, 5 ) )
                      {
                        pOwnerItem->fsAttributeOld = (pOwnerItem->fsAttribute &= ~MIA_HILITED);
                        bSuccess=TRUE;
                      }
                  }
                else
                  {
                    if ( CandyBarZMinMaxPaintProc( pOwnerItem->hwnd, pCBZData, &(pOwnerItem->rclItem), pOwnerItem->hps, 4 ) )
                      {
                        pOwnerItem->fsAttributeOld = (pOwnerItem->fsAttribute &= ~MIA_HILITED);
                        bSuccess=TRUE;
                      }
                  }
              }
            else if( pOwnerItem->idItem == SC_RESTORE )// Restore button
              {
                //draw Restore Button
                if ( pOwnerItem->fsAttribute & MIA_HILITED )
                  {
                    if ( CandyBarZMinMaxPaintProc( pOwnerItem->hwnd, pCBZData, &(pOwnerItem->rclItem), pOwnerItem->hps, 7 ) )
                      {
                        pOwnerItem->fsAttributeOld = (pOwnerItem->fsAttribute &= ~MIA_HILITED);
                        bSuccess=TRUE;
                      }
                  }
                else
                  {
                    if ( CandyBarZMinMaxPaintProc( pOwnerItem->hwnd, pCBZData, &(pOwnerItem->rclItem), pOwnerItem->hps, 6 ) )
                      {
                        pOwnerItem->fsAttributeOld = (pOwnerItem->fsAttribute &= ~MIA_HILITED);
                        bSuccess=TRUE;
                      }
                  }
              }
            else if( pOwnerItem->idItem == SC_HIDE )// Hide button
              {
                //draw Hide Button
                if ( pOwnerItem->fsAttribute & MIA_HILITED )
                  {
                    if ( CandyBarZMinMaxPaintProc( pOwnerItem->hwnd, pCBZData, &(pOwnerItem->rclItem), pOwnerItem->hps, 9 ) )
                      {
                        pOwnerItem->fsAttributeOld = (pOwnerItem->fsAttribute &= ~MIA_HILITED);
                        bSuccess=TRUE;
                      }
                  }
                else
                  {
                    if ( CandyBarZMinMaxPaintProc( pOwnerItem->hwnd, pCBZData, &(pOwnerItem->rclItem), pOwnerItem->hps, 8 ) )
                      {
                        pOwnerItem->fsAttributeOld = (pOwnerItem->fsAttribute &= ~MIA_HILITED);
                        bSuccess=TRUE;
                      }
                  }
              }
          }
          CATCH(excDrawItem){
          }
          END_CATCH;
          
          return (MRESULT) bSuccess;
          
      }//end WM_DRAWITEM
      break;
      
      //Message to catch the background paint message
      //Occurs for example for the CD-Creator classes
      //Normally the background wouldn't be painted, because on creation
      //the clent covers the whole window
    case WM_ERASEBACKGROUND:
      {
        hps = (HPS) HWNDFROMMP(mp1);
        prectl = (RECTL *) PVOIDFROMMP(mp2);

        //fill the background
        if (!CandyBarZFramePaintBackgroundProc(hwnd, pCBZData, prectl, hps))
          {
            return ((MRESULT) TRUE);
          }
        else
          {
            return ((MRESULT) FALSE);
          }
      }//end WM_ERASEBACKGROUND
      break;
      //Message to switch the frame border between the active and inactive states
    case WM_ACTIVATE:
      {

        if ( WinQueryWindowUShort(hwnd, QWS_ID) == FID_CLIENT )
          return pfnOrigWndProc(hwnd, msg, mp1, mp2);

        TRY_LOUD(excAct) // Exception handler from XFolder
          {

            state = SHORT1FROMMP(mp1);
            flags = WinQueryWindowUShort(hwnd, QWS_FLAGS);

            //Set The Active Flag!
            if (state == TRUE)
              WinSetWindowUShort(hwnd, QWS_FLAGS, flags | FF_ACTIVE);
            else
              WinSetWindowUShort(hwnd, QWS_FLAGS, flags & ~FF_ACTIVE);

            //set the state of the titlebar
            WinSendMsg(WinWindowFromID(hwnd, FID_TITLEBAR), TBM_SETHILITE, mp1, 0);
            
            //set the state of the min/max menu
            WinSendMsg(WinWindowFromID(hwnd, FID_MINMAX), WM_ACTIVATE, mp1, mp2);
            
            //paint the border
            CandyBarZGeneralPaintProc(hwnd, pCBZData, FALSE, !state,
                                      pCBZData->sFrameBorderIndex, pCBZData->sPushButtonIndex, CBZ_FRAMEBRDR);
        
        //set the state of the FID_CLIENT window
        WinSendMsg(WinWindowFromID(hwnd, FID_CLIENT), WM_ACTIVATE, mp1, mp2);

#if 0
        // set the state of the window manager decoration
        WinSendMsg(pCBZData->hwndWMgrFrame, WM_ACTIVATE, mp1,mp2);
#endif
          }
          CATCH(excAct){
          }
          END_CATCH;            
          
        return 0;
      }//end WM_ACTIVATE
#if 0
    case WM_ADJUSTFRAMEPOS:

      pswp=(PSWP)mp1;
      /* Do not use Titelbar move. It's done by the mgr */
      if(pswp->fl & SWP_MAXIMIZE)
        pswp->cy-=10;
      WinSetWindowPos(pCBZData->hwndWMgrFrame,pswp->hwndInsertBehind,pswp->x,pswp->y,pswp->cx,pswp->cy,
                      pswp->fl/*&~SWP_ACTIVATE*/);
      break;
#endif

      //Message to paint the frame and the border.
    case WM_PAINT:
      {
        HWND hwndClient;
        RECTL rclClient,rcl ;
        BOOL bBorderDone=FALSE;

        if(CBZShare.plugins.bFrameBackgroundEnabled) {
          WinQueryWindowRect(hwnd,&rcl);
          if(!WinQueryWindowULong(hwnd, ulOffsetPrivateData+offsetof(PRIVATEFRAMEDATA, bCustomPaint))) {
            /*Save update rect */
            // WinQueryUpdateRect(hwnd, &rectl);
            /* This also paints the border. */
            (pfnOrigWndProc(hwnd, msg, mp1, mp2));/* No border painting at the moment!! */
            bBorderDone=TRUE;
          }
          else{
            hwndClient=WinWindowFromID(hwnd,FID_CLIENT);
            if(hwndClient) { /* We have got a Client */
              /* get client dimensions */
              WinQueryWindowRect(hwndClient,&rclClient);
              
              /* Check if the client covers almost the whole frame. If so do not paint the background. */
              /* That gives us a lot of performance */
              //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
              //Achtung fÅr den Vergleich wird die Framegrî·e gebraucht, nicht update!!
              //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
              if(((rcl.xRight-rcl.xLeft)-(rclClient.xRight-rclClient.xLeft))<50) {/* 50 seems to be a good value */
                /* Set a marker into the framewindow data area to show we don't have a custom Background */
                /* Do not paint the BG for this frame in the future */
                WinSetWindowULong(hwnd, ulOffsetPrivateData+offsetof(PRIVATEFRAMEDATA, bCustomPaint),FALSE);
                /*Save update rect */
                //WinQueryUpdateRect(hwnd, &rectl);
                (pfnOrigWndProc(hwnd, msg, mp1, mp2));/* No border painting at the moment */
                bBorderDone=TRUE;
              }
              else {
                hps = WinBeginPaint(hwnd,NULLHANDLE,&rectl);
                //fill the background
                TRY_LOUD(excPaint1) // Exception handler from XFolder
                  {
                    CandyBarZFramePaintBackgroundProc(hwnd, pCBZData, &rectl, hps);       
                  }
                  CATCH(excPaint1){
                  }
                  END_CATCH;            
                WinEndPaint(hps);
              }
            }
            else {
              hps = WinBeginPaint(hwnd,NULLHANDLE,&rectl);
              //fill the background
              TRY_LOUD(excPaint2) // Exception handler from XFolder
                {
                  CandyBarZFramePaintBackgroundProc(hwnd, pCBZData, &rectl, hps);       
                }
                CATCH(excPaint2){
                }
                END_CATCH;            
                WinEndPaint(hps);
            }
          }
        }
        else {
          (pfnOrigWndProc(hwnd, msg, mp1, mp2));/* No border painting at the moment */
          bBorderDone=TRUE;
        }
        /* Background was painted. Do the border now. */
        
        if (!(pCBZData->sFrameBorderIndex == pCBZData->sPushButtonIndex) ) {
          //*** Should only draw border if the update rectangle includes the border!!
          // try to determine active or inactive.  the current criteria are
          // 1. Is the focus window a child of our frame
          // 2. Is the frame owner of the focus window a child of our frame
          // 3. Is the owner of the frame owner of the focus window a child of our frame
          /* 4. Is the display active flag set in the window styles */
          hwndFocus = WinQueryFocus(HWND_DESKTOP);
          TRY_LOUD(excPaintBrd) // Exception handler from XFolder
            {
              if (WinIsChild(hwndFocus, hwnd) ||
                  WinIsChild(WinQueryWindow(hwndFocus, QW_FRAMEOWNER), hwnd) ||
                  WinIsChild(WinQueryWindow(WinQueryWindow(hwndFocus, QW_FRAMEOWNER), QW_OWNER),
                             hwnd) ||
                  (WinQueryWindowUShort(hwnd, QWS_FLAGS) & FF_ACTIVE)) {
                /* Draw border in the Active State */
                CandyBarZGeneralPaintProc(hwnd, pCBZData, FALSE, FALSE,
                                          pCBZData->sFrameBorderIndex, pCBZData->sPushButtonIndex, CBZ_FRAMEBRDR);
              }
              else{
                /* Draw border in the Inactive State. */
                CandyBarZGeneralPaintProc(hwnd, pCBZData, FALSE, TRUE,
                                          pCBZData->sFrameBorderIndex, pCBZData->sPushButtonIndex, CBZ_FRAMEBRDR);
              }
            }
            CATCH(excPaintBrd){
            }
            END_CATCH;            
              
          return ((MRESULT)0);
        }

        if(!bBorderDone){
          hps = WinGetPS(hwnd);
          if(WinQueryWindowULong(hwnd,QWL_STYLE) & FS_SIZEBORDER)/* Draw frame border */
            WinDrawBorder(hps,&rcl,1,1,
                          SYSCLR_ACTIVEBORDER,SYSCLR_INACTIVEBORDER,0x400);
          else if(WinQueryWindowULong(hwnd,QWL_STYLE) & FS_DLGBORDER)
            /* Draw dlg border */
            WinDrawBorder(hps,&rcl,2,2,
                          SYSCLR_ACTIVEBORDER,SYSCLR_INACTIVEBORDER,0x400);
          WinReleasePS(hps);
        }
        return ((MRESULT)0);
      }//end WM_PAINT
      return ((MRESULT) FALSE);
     
      //Message to deallocate memory and resources
    case WM_DESTROY:
      {

        /* Delete Atom */
        hAtomTbl = WinQuerySystemAtomTable();
        if(hAtomTbl) {
          Atom=WinQueryWindowULong(hwnd,ulOffsetPrivateData+offsetof(PRIVATEFRAMEDATA, Atom));
          if(Atom)
            WinDeleteAtom(hAtomTbl,Atom);
        }

        hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);
            if (hwnd == hwndFrame)
            {
                CandyBarZDestroyWindowData(hwnd);
            }

         }//end WM_DESTROY
        break;

        default:  // this mirrors the WM_CREATE handling with a few exceptions
        {
            if (!Atom)
            {
                break;
            }
            if (msg == Atom)
            {
              hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);


                if (hwnd == hwndFrame)
                {
                    //destroy old settings and re-init the settings based on the
                    //  shared memory
                    CandyBarZSettingsChange(hwnd);
                    if(!mp1)
                      {
                        if(bAcceptBroadcast) {
                          CBZSHARE *pMyCBZShare = NULL;
                          // copy shared mem to local DLL data area
                          // get shared mem
                          if (DosGetNamedSharedMem((PPVOID) & pMyCBZShare, CBZ_SHARE, PAG_READ) == NO_ERROR)
                            {
                              memcpy(&CBZShare, pMyCBZShare,  sizeof(CBZSHARE));
                            }
                          DosFreeMem(pMyCBZShare);
                          bAcceptBroadcast=FALSE; // copied, disable copy
                        }
                      }
                    else {
                      bAcceptBroadcast=TRUE;//enable copy
                    }

                }

                //make sure pCBZData exists
                if ( (pCBZData = WinQueryWindowPtr(hwndFrame, ulDataOffset)) == NULL )
                    break;

                if ( (pCBZData->bMinMaxEnabled) && (pCBZData->sMinMaxIndex != pCBZData->sMenuIndex) )
                  {
                    hwndMenu=WinWindowFromID(hwnd,FID_MINMAX);
                    if(hwndMenu)
                      {
                        // Mark buttons as ownerdrawn
                        if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_CLOSE_,FALSE),&mi))
                          {
                            mi.afStyle|=MIS_OWNERDRAW;
                            WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                          }
                        if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_MINIMIZE,FALSE),&mi))
                        {
                            mi.afStyle|=MIS_OWNERDRAW;
                            WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                        }
                        if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_MAXIMIZE,FALSE),&mi))
                        {
                            mi.afStyle|=MIS_OWNERDRAW;
                            WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                        }
                        if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_HIDE,FALSE),&mi))
                        {
                            mi.afStyle|=MIS_OWNERDRAW;
                            WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                        }
                        if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_RESTORE,FALSE),&mi))
                        {
                            mi.afStyle|=MIS_OWNERDRAW;
                            WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                        }
                    }
                }
                else
                {
                    hwndMenu=WinWindowFromID(hwnd,FID_MINMAX);
                    if(hwndMenu)
                    {
                        // Mark buttons as NOT ownerdrawn
                        if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_CLOSE_,FALSE),&mi))
                        {
                            mi.afStyle &= ~MIS_OWNERDRAW;
                            WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                        }
                        if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_MINIMIZE,FALSE),&mi))
                        {
                            mi.afStyle &= ~MIS_OWNERDRAW;
                            WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                        }
                        if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_MAXIMIZE,FALSE),&mi))
                        {
                            mi.afStyle &= ~MIS_OWNERDRAW;
                            WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                        }
                        if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_HIDE,FALSE),&mi))
                        {
                            mi.afStyle &= ~MIS_OWNERDRAW;
                            WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                        }
                        if(WinSendMsg(hwndMenu,MM_QUERYITEM,MPFROM2SHORT(SC_RESTORE,FALSE),&mi))
                        {
                            mi.afStyle &= ~MIS_OWNERDRAW;
                            WinSendMsg(hwndMenu,MM_SETITEM,MPFROM2SHORT(0,FALSE),&mi);
                        }
                    }
                }

                // force repaint
                WinQueryWindowRect(hwnd, &rectl);
                WinInvalidateRect(hwnd, &rectl, TRUE);
            }
        }//end default
        break;
    }
    return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
}

/*************************************************************************************************************

    $Id: candybarzframeproc.c,v 1.6 2000/10/21 10:11:12 cwohlgemuth Exp $

    $Log: candybarzframeproc.c,v $
    Revision 1.6  2000/10/21 10:11:12  cwohlgemuth
    Added exception handlers

    Revision 1.5  2000/09/24 20:34:31  cwohlgemuth
    Moved background painting from WM_ERASEBACKGROUND to WM_PAINT. Gives bg on some more frames.

    Revision 1.4  2000/09/09 23:07:18  cwohlgemuth
    Local DLL data will be updated when broadcasting change atom

    Revision 1.3  2000/08/31 17:13:37  cwohlgemuth
    Reenabled frame control painting. WM_CREATE for plugins.

    Revision 1.2  1999/12/29 19:38:09  enorman
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
