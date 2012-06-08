
/******************************************************************************
  Copyright Netlabs, 1998, this code may not be used for any military purpose
******************************************************************************/
#include "candybarz.h"

extern CBZSHARE    CBZShare;

MRESULT EXPENTRY CandyBarZMenuProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CBZDATA     *pCBZData = NULL;
    CBZSHARE    *pCBZShare = NULL;
    PFNWP       pfnOrigWndProc;
    HPS         hps;
    ULONG       ulDataOffset;
    RECTL       rcl;
    USHORT      mId;
    MRESULT     rc;
    int         i;

    HWND        hwndFrame = CandyBarZGetTopLevelFrameHWND(hwnd);

    pfnOrigWndProc  = CBZShare.pMenuOrigWndProc; //get address of original wnd proc.
    ulDataOffset    = CBZShare.ulDataOffset;      //get offset to pCBZData

#if 0
    //    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ) == NO_ERROR)
        {

        ulDataOffset = pCBZShare->ulDataOffset;
        pfnOrigWndProc = pCBZShare->pMenuOrigWndProc;
        DosFreeMem(pCBZShare);
#endif

        if ((pCBZData = WinQueryWindowPtr(hwndFrame, ulDataOffset)) == NULL)
        {
            return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
        }

        mId = WinQueryWindowUShort(hwnd, QWS_ID);

        if (mId == FID_MINMAX) /* Frame controls */
        {
            if ( !(pCBZData->bMinMaxEnabled) )
                return (pfnOrigWndProc(hwnd, msg, mp1, mp2)); /* Minmax painting not enabled */
            else
            {
                for (i = pCBZData->sMinMaxIndex; i < pCBZData->sMenuIndex; i++)
                {
                    if ( pCBZData->Plugins[i].pPluginWndProc(hwnd, msg, mp1, mp2, pCBZData->Plugins[i].pData,
                                                             pfnOrigWndProc, &rc) == TRUE)
                        return rc;
                }
            }
        } // end of if (mId == FID_MINMAX)
        else if  ( (mId != FID_MINMAX) && !(pCBZData->bMenuEnabled) )
            return (pfnOrigWndProc(hwnd, msg, mp1, mp2)); /* It's another menu */
#if 0 
    }
    else
        return WinDefWindowProc(hwnd, msg, mp1, mp2);
#endif

    switch (msg)
    {
/*
        case MM_INSERTITEM:
        {
            PMENUITEM pmi;

            pmi=(PMENUITEM) PVOIDFROMMP(mp1);
            if ( (pmi->id == SC_RESTORE) || (pmi->id == SC_MAXIMIZE) ||
                 (pmi->id == SC_HIDE) || (pmi->id == SC_MINIMIZE) )
                pmi->afStyle|=MIS_OWNERDRAW;
        }
        break;

        case WM_ACTIVATE:
        {
            USHORT state = SHORT1FROMMP(mp1);

            hps = WinGetPS(hwnd);

            if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_CLOSE_,FALSE),&rcl))
            {
                //draw CloseButton
                CandyBarZMinMaxPaintProc(hwnd, pCBZData,&rcl, hps, state ? 0 : 10);
            }
            if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_MINIMIZE,FALSE),&rcl))
            {
                //draw MinimizeButton
                CandyBarZMinMaxPaintProc(hwnd, pCBZData, &rcl, hps, state ? 2 : 12);
            }
            if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_MAXIMIZE,FALSE),&rcl))
            {
                //draw MaximizeButton
                CandyBarZMinMaxPaintProc(hwnd, pCBZData, &rcl, hps, state ? 4 : 14);
            }
            if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_RESTORE,FALSE),&rcl))
            {
                //draw RestoreButton
                CandyBarZMinMaxPaintProc(hwnd, pCBZData, &rcl, hps, state ? 6 : 16);
            }
            if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_HIDE,FALSE),&rcl))
            {
                //draw HideButton
                CandyBarZMinMaxPaintProc(hwnd, pCBZData, &rcl, hps, state ? 8 : 18);
            }

            WinReleasePS(hps);
        }
        break;

        case WM_PAINT:
        {
            BOOL    bActive = FALSE;

            mId = WinQueryWindowUShort(hwnd, QWS_ID);

            //paint the MinMax menu!
            if (mId == FID_MINMAX)
            {
                hwndFrame = WinQueryWindow(hwnd, QW_PARENT);

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
                    bActive = TRUE;

                hps = WinGetPS(hwnd);
                if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_CLOSE_,FALSE),&rcl))

                {
                    //draw CloseButton
                    CandyBarZMinMaxPaintProc(hwnd, pCBZData, &rcl, hps, bActive ? 0 : 10);
                }
                if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_MINIMIZE,FALSE),&rcl))
                {
                    //draw MinimizeButton
                    CandyBarZMinMaxPaintProc(hwnd, pCBZData, &rcl, hps, bActive ? 2 : 12);
                }
                if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_MAXIMIZE,FALSE),&rcl))
                {
                    //draw MaximizeButton
                    CandyBarZMinMaxPaintProc(hwnd, pCBZData, &rcl, hps, bActive ? 4 : 14);
                }
                if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_RESTORE,FALSE),&rcl))
                {
                    //draw RestoreButton
                    CandyBarZMinMaxPaintProc(hwnd, pCBZData, &rcl, hps, bActive ? 6 : 16);
                }
                if(WinSendMsg(hwnd,MM_QUERYITEMRECT,MPFROM2SHORT(SC_HIDE,FALSE),&rcl))
                {
                    //draw HideButton
                    CandyBarZMinMaxPaintProc(hwnd, pCBZData, &rcl, hps, bActive ? 8 : 18);
                }
                WinReleasePS(hps);
            }
        }
        break;
*/
    }
    return (pfnOrigWndProc(hwnd, msg, mp1, mp2));
}

/*************************************************************************************************************

    $Id: candybarzmenuproc.c,v 1.3 2000/09/02 13:13:45 cwohlgemuth Exp $

    $Log: candybarzmenuproc.c,v $
    Revision 1.3  2000/09/02 13:13:45  cwohlgemuth
    Minor speed improvement

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
