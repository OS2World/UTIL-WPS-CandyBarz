/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#include "psbitmap.h"

HBITMAP PSBMBLTToSize(HAB hab, HPS hps, HBITMAP hbm, float fScale, ULONG ulOptions)
{
    BITMAPINFOHEADER2 bif2Before;
    BITMAPINFOHEADER2 bif2After;
    SIZEL slAfter;
    ULONG ulScreenWidth;
    ULONG ulScreenHeight;
    HDC hdcAfter;
    HPS hpsAfter;
    HBITMAP hbmAfter;
    POINTL aptl[4];
    float fAspect;
    HBITMAP hbmOld;

    if ((hbmOld = GpiSetBitmap(hps, hbm)) == HBM_ERROR)
    {
        return (NULLHANDLE);
    }

    memset(&bif2Before, 0, sizeof(bif2Before));  // get current info

    bif2Before.cbFix = sizeof(bif2Before);
    if (GpiQueryBitmapInfoHeader(hbm, &bif2Before) != TRUE)
    {
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    ulScreenWidth = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);  // screen width

    ulScreenHeight = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);  // screen height

    fAspect = (bif2Before.cx * 1.0) / (bif2Before.cy * 1.0);  // calculate aspect ratio

    if (ulOptions & SCALE_LARGEST)
    {
        if (bif2Before.cx > bif2Before.cy)
        {
            slAfter.cx = (LONG) (ulScreenWidth * fScale);
            slAfter.cy = (LONG) (slAfter.cx / fAspect);
        }
        else
        {
            slAfter.cy = (LONG) (ulScreenHeight * fScale);
            slAfter.cx = (LONG) (slAfter.cy * fAspect);
        }
    }
    else if (ulOptions & SCALE_SMALLEST)
    {
        if (bif2Before.cx < bif2Before.cy)
        {
            slAfter.cx = (LONG) (ulScreenWidth * fScale);
            slAfter.cy = (LONG) (slAfter.cx / fAspect);
        }
        else
        {
            slAfter.cy = (LONG) (ulScreenHeight * fScale);
            slAfter.cx = (LONG) (slAfter.cy * fAspect);
        }
    }
    else if (ulOptions & SCALE_HORIZONTAL)
    {
        slAfter.cx = (LONG) (ulScreenWidth * fScale);
        if (ulOptions & SCALE_VERTICAL)  // basically, screw the aspect

        {
            slAfter.cy = (LONG) (ulScreenHeight * fScale);
        }
        else
        {
            slAfter.cy = (LONG) (slAfter.cx / fAspect);
        }
    }
    else if (ulOptions & SCALE_VERTICAL)
    {
        slAfter.cy = (LONG) (ulScreenHeight * fScale);
        slAfter.cx = (LONG) (slAfter.cy * fAspect);
    }
    else
    {
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    if ((hdcAfter = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE))
            == NULLHANDLE)
    {
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }
    if ((hpsAfter = GpiCreatePS(hab,
                                hdcAfter,
                                &slAfter,
           PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC)) == NULLHANDLE)
    {
        DevCloseDC(hdcAfter);
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    memset(&bif2After, 0, sizeof(bif2After));
    bif2After.cbFix = sizeof(bif2After);
    bif2After.cx = slAfter.cx;
    bif2After.cy = slAfter.cy;
    bif2After.cBitCount = bif2Before.cBitCount;
    bif2After.cPlanes = bif2Before.cPlanes;

    if ((hbmAfter = GpiCreateBitmap(hpsAfter,  // create bitmap
                                     (PBITMAPINFOHEADER2) & (bif2After),
                                    (ULONG) FALSE,
                                    NULL,
                                    NULL)) == NULLHANDLE)
    {
        GpiDestroyPS(hpsAfter);
        DevCloseDC(hdcAfter);
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    if (GpiSetBitmap(hpsAfter, hbmAfter) == HBM_ERROR)
    {
        GpiDeleteBitmap(hbmAfter);
        GpiDestroyPS(hpsAfter);
        DevCloseDC(hdcAfter);
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slAfter.cx;
    aptl[1].y = slAfter.cy;
    aptl[2].x = 0;
    aptl[2].y = 0;
    aptl[3].x = bif2Before.cx;
    aptl[3].y = bif2Before.cy;

    if (GpiBitBlt(hpsAfter,
                  hps,
                  4L,
                  aptl,
                  ROP_SRCCOPY,
                  BBO_IGNORE) == GPI_ERROR)
    {
        GpiSetBitmap(hpsAfter, NULLHANDLE);
        GpiDeleteBitmap(hbmAfter);
        GpiDestroyPS(hpsAfter);
        DevCloseDC(hdcAfter);
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    GpiSetBitmapDimension(hbmAfter, &slAfter);

    GpiSetBitmap(hpsAfter, NULLHANDLE);
    GpiDestroyPS(hpsAfter);
    DevCloseDC(hdcAfter);
    GpiSetBitmap(hps, hbmOld);

    return (hbmAfter);
}
/*
   char *PSBMLoadBitmapFile( char *pszFileName, GBM *pgbm, GBMRGB *pgbmrgb )
   {
   HFILE hBMPFile;
   ULONG ulNewFile;
   ULONG ulStride;
   ULONG cbData;
   char    *pchData;

   if( DosOpen(   pszFileName,
   &hBMPFile,
   &ulNewFile,
   0L,
   FILE_NORMAL,
   OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
   OPEN_SHARE_DENYWRITE | OPEN_ACCESS_READONLY,
   NULL ) != 0L )
   {
   return( NULL );
   }

   if( GBMReadBMPHeader( hBMPFile, pgbm ) != TRUE )   // read bmp header
   {
   DosClose( hBMPFile );
   return( NULL );
   }

   if( GBMReadPalette( hBMPFile, pgbm, pgbmrgb ) != TRUE )  // read bmp palette
   {
   DosClose( hBMPFile );
   return( NULL );
   }

   ulStride = ( ( pgbm->w * pgbm->bpp + 31 ) / 32 ) * 4;   // calculate size of data and alloc it
   cbData = pgbm->h * ulStride;
   if( DosAllocMem( ( PPVOID )&pchData, cbData, PAG_READ | PAG_WRITE | PAG_COMMIT ) != 0 )
   {
   DosClose( hBMPFile );
   return( NULL );
   }

   if( GBMReadData( hBMPFile, pgbm, pchData ) != TRUE )        // read it in
   {
   DosFreeMem( pchData );
   DosClose( hBMPFile );
   return( NULL );
   }

   DosClose( hBMPFile );

   return( pchData );
   }
 */

/*************************************************************************************************************

	 $Id: psBitmap.c,v 1.1 1999/06/15 20:48:18 ktk Exp $

	 $Log: psBitmap.c,v $
	 Revision 1.1  1999/06/15 20:48:18  ktk
	 Import
	
	 Revision 1.3	1998/09/11 03:02:32	pgarner
	 Add the non-military use license

	 Revision 1.2	1998/09/11 02:45:45	pgarner
	 'Added

	 Revision 1.1	1998/06/08 14:18:59	pgarner
	 Initial Checkin

*************************************************************************************************************/
