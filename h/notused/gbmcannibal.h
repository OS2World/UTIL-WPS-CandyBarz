/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/


#ifndef _GBM_H_
#define _GBM_H_

#include <stdio.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#define INCL_WIN
#include <os2.h>
#include <pmbitmap.h>

#define PRIV_SIZE 2000
#define AHEAD_BUF 0x4000
#define MSWCC_EOL       0
#define MSWCC_EOB       1
#define MSWCC_DELTA     2

// the following two defines are from the original gbm
//#define U_SAT_ADD(a,b)  usat[(a)+(b)+256]
//#define S_SAT_ADD(a,b)  ssat[(a)+(b)+512]
#define USAT(x)         ( (BYTE *)(x) )
#define SSAT(x)         ( (SHORT *)( ((BYTE *)(x)) + 768 ) )
#define INDEX4(x)       ( (BYTE *)( ((SHORT *)SSAT(x)) + 1024 ) )
#define INDEX6(x)       ( (BYTE *)((INDEX4(x))+256) )
#define INDEX7(x)       ( (BYTE *)((INDEX6(x))+256) )
#define INDEX8(x)       ( (BYTE *)((INDEX7(x))+256) )
#define INDEX16(x)      ( (BYTE *)((INDEX8(x))+256) )
#define RANDTAB(x)      ( (USHORT *)( ((BYTE *)INDEX16(x)) + 256 ) )
#define USAT_ADD(x,y,z) ((BYTE)((USAT(x))[((y)+(z)+256)]))
#define SSAT_ADD(x,y,z) ((SHORT)((SSAT(x))[((y)+(z)+512)]))

#ifdef __cplusplus
    extern "C"{
#endif

typedef struct __GBMSTRUCT
{
    ULONG w;
    ULONG h;
    ULONG bpp;                  /* Bitmap dimensions                 */
    BYTE priv [PRIV_SIZE];          /* Private internal buffer           */
} GBM;

typedef struct __GBMRGB
{
    BYTE Red;
    BYTE Green;
    BYTE Blue;
} GBMRGB;

typedef struct _BMP_PRIV
{
    ULONG   base;
    BOOL    windows;
    ULONG   cbFix;
    ULONG   ulCompression;
    ULONG   cclrUsed;
    ULONG   offBits;
} BMP_PRIV;

typedef struct
{
    char pchBuf[ AHEAD_BUF ];
    ULONG inx;
    ULONG cnt;
    HFILE hFile;
} AHEAD;

BOOL GBMReadBMPHeader( HFILE hFile, GBM *gbm );
BOOL GBMReadPalette( HFILE hFile, GBM *gbm, GBMRGB *gbmrgb );
BOOL GBMReadData( HFILE hFile, GBM *gbm, BYTE *data );
BYTE *GBMErrInitialize( void );
void GBMErrDiffPalVGA( BYTE *pbErrDiff, GBMRGB *gbmrgb );
BOOL GBMErrDiffVGA( BYTE *pbErrDiff, GBM *gbm, BYTE *data24, BYTE *data4);
void GBMErrDiffLineVGA( BYTE *pbErrDiff, BYTE *src, BYTE *dest, short *errs, int cx );
void GBMErrDiffPal7R8G4B( BYTE *pbErrDiff, GBMRGB *gbmrgb );
BOOL GBMErrDiff7R8G4B( BYTE *pbErrDiff, GBM *gbm, BYTE *data24, BYTE *data8 );
void GBMErrDiffLine7R8G4B( BYTE *pbErrDiff, BYTE *src, BYTE *dest, short *errs, int cx);
BOOL GBMErrDiff24( BYTE *pbErrDiff, GBM *gbm, BYTE *data24, BYTE *data24a, BYTE rm, BYTE gm, BYTE bm);
void GBMErrDiffLine24(  BYTE *pbErrDiff,
                        BYTE *src,
                        BYTE *dest,
                        short *errs,
                        int cx,
                        BYTE rm,
                        BYTE gm,
                        BYTE bm );
BOOL GBMTo24Bit( GBM *gbm, GBMRGB *gbmrgb, char **ppchData );

#ifdef __cplusplus
    }
#endif



/*************************************************************************************************************


    $Id: gbmcannibal.h,v 1.1 1999/06/15 20:48:02 ktk Exp $
    

    $Log: gbmcannibal.h,v $
    Revision 1.1  1999/06/15 20:48:02  ktk
    Import

    Revision 1.3  1998/09/11 03:02:34  pgarner
    Add the non-military use license

    Revision 1.2  1998/09/11 02:45:48  pgarner
    'Added

    Revision 1.1  1998/06/08 14:19:01  pgarner
    Initial Checkin



*************************************************************************************************************/

#endif
