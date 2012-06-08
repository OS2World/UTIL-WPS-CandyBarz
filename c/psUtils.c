
/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#include "psutils.h"

void PSUTErrorFunc(HWND hwnd, char *Type, char *Location, char *Msg, ULONG ulStyles)
{
    char szCaptionBuffer[1024];

    sprintf(szCaptionBuffer, "%s in %s", Type, Location);

    if (!ulStyles)
    {
        WinMessageBox(HWND_DESKTOP,
                      hwnd,
                      Msg,
                      szCaptionBuffer,
                      0,
                      MB_OK | MB_ICONEXCLAMATION);
    }
    else
    {
        WinMessageBox(HWND_DESKTOP,
                      hwnd,
                      Msg,
                      szCaptionBuffer,
                      0,
                      ulStyles);
    }

    return;
}

int PSUTVersion(void)
{
    return ((int) PSRT_VERSION);
}

/*************************************************************************************************************

    $Id: psUtils.c,v 1.1 1999/06/15 20:47:58 ktk Exp $
    

    $Log: psUtils.c,v $
    Revision 1.1  1999/06/15 20:47:58  ktk
    Import

    Revision 1.3  1998/09/11 03:02:34  pgarner
    Add the non-military use license

    Revision 1.2  1998/09/11 02:45:47  pgarner
    'Added

    Revision 1.1  1998/06/08 14:19:00  pgarner
    Initial Checkin

*************************************************************************************************************/
