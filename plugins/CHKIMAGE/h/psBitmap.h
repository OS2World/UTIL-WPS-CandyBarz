/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#ifndef _PSBM_H_
#define _PSBM_H_

// for blttosize
#define INCL_GPI
#define INCL_WIN
#include <os2.h>
#include <string.h>
//#include "gbmcannibal.h"

#define SCALE_LARGEST		 0x00000001L
#define SCALE_SMALLEST		 0x00000002L
#define SCALE_HORIZONTAL	 0x00000004L
#define SCALE_VERTICAL		 0x00000008L

#ifdef __cplusplus
extern "C"
{
#endif

// blt a bmp to proper size
    HBITMAP PSBMBLTToSize(HAB hab, HPS hps, HBITMAP hbm, float fScale, ULONG ulOptions);
//char *PSBMLoadBitmapFile( char *pszFileName, GBM *pgbm, GBMRGB *pgbmrgb );

#ifdef __cplusplus
}
#endif

/*************************************************************************************************************

	 $Id: psBitmap.h,v 1.1 1999/06/15 20:48:12 ktk Exp $

	 $Log: psBitmap.h,v $
	 Revision 1.1  1999/06/15 20:48:12  ktk
	 Import
	
	 Revision 1.3	1998/09/11 03:02:35	pgarner
	 Add the non-military use license

	 Revision 1.2	1998/09/11 02:45:48	pgarner
	 'Added

	 Revision 1.1	1998/06/08 14:19:01	pgarner
	 Initial Checkin

*************************************************************************************************************/

#endif
