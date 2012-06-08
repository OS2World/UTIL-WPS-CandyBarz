
#include "psrt.h"

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

    $Id: CandyBarZPSRT.c,v 1.1 1999/06/15 20:47:55 ktk Exp $

    $Log: CandyBarZPSRT.c,v $
    Revision 1.1  1999/06/15 20:47:55  ktk
    Import

    Revision 1.1  1998/10/17 19:30:29  enorman
    Initial revision

*************************************************************************************************************/
