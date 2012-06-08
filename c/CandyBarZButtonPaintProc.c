
/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "candybarz.h"

//all functionality has been moved to CandybarzGeneralPaintProc

/*************************************************************************************************************

    $Id: CandyBarZButtonPaintProc.c,v 1.3 2000/03/26 06:51:20 enorman Exp $

    $Log: CandyBarZButtonPaintProc.c,v $
    Revision 1.3  2000/03/26 06:51:20  enorman
    resyncing with my local tree.

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
