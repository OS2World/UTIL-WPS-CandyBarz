/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#ifndef _PSUT_H_
#define _PSUT_H_

#define INCL_WIN
#include <os2.h>
#include <stdio.h>

#define PSRT_VERSION    0x0000000D

#ifdef __cplusplus
extern "C"
{
#endif

    void PSUTErrorFunc(HWND hwnd, char *Type, char *Location, char *Msg, ULONG ulStyles);
    int PSUTVersion(void);

#ifdef __cplusplus
}
#endif

/*************************************************************************************************************

    $Id: psUtils.h,v 1.1 1999/06/15 20:48:01 ktk Exp $
    

    $Log: psUtils.h,v $
    Revision 1.1  1999/06/15 20:48:01  ktk
    Import

    Revision 1.3  1998/09/11 03:02:36  pgarner
    Add the non-military use license

    Revision 1.2  1998/09/11 02:45:49  pgarner
    'Added

    Revision 1.1  1998/06/08 14:19:02  pgarner
    Initial Checkin

*************************************************************************************************************/

#endif
