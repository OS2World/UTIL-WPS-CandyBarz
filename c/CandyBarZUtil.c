/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#include "candybarz.h"

/********************************************************************
* BltToWindow: Transfer the image in the offscreen presentation     *
*       space to the screen.                                        *
* Parameters:                                                       *
*   PSPAINT *pPaint - Pointer to the structure containing the paint *
*       information.                                                *
********************************************************************/
BOOL BltToWindow(PSPAINT * pPaint)
{
    POINTL aptl[TXTBOX_COUNT];

    aptl[0].x = aptl[2].x = pPaint->rectlUpdate.xLeft;
    aptl[0].y = aptl[2].y = pPaint->rectlUpdate.yBottom;
    aptl[1].x = aptl[3].x = pPaint->rectlUpdate.xRight;
    aptl[1].y = aptl[3].y = pPaint->rectlUpdate.yTop;

    if (GpiBitBlt(pPaint->hpsWin, pPaint->hps, 4, aptl, ROP_SRCCOPY, BBO_IGNORE) == GPI_ERROR)
    {
        return (FALSE);
    }
    return (TRUE);
}


/*************************************************************************************************************

    $Id: CandyBarZUtil.c,v 1.2 1999/12/29 19:38:08 enorman Exp $

    $Log: CandyBarZUtil.c,v $
    Revision 1.2  1999/12/29 19:38:08  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:47:56  ktk
    Import

    Revision 1.7  1998/09/27 23:38:41  enorman
    Fixed some problems with 16-bit color

    Revision 1.6  1998/09/12 10:05:31  pgarner
    Added copyright notice

    Revision 1.5  1998/09/12 07:35:45  mwagner
    Added support for bitmap images

    Revision 1.2  1998/08/31 19:14:57  enorman
    Uncommented the calls to GpiQueryColorIndex in order to eliminate the
    "Black Text" problem.

    Revision 1.1  1998/06/08 14:18:56  pgarner
    Initial Checkin

*************************************************************************************************************/
