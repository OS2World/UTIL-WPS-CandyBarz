/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/


#ifndef _PSMM_H_
#define _PSMM_H_

#define INCL_OS2MM
#define INCL_MCIOS2
#define INCL_GPI
#include <os2.h>
#include <os2me.h>
#include <string.h>
#include <stdio.h>
#include <mmioos2.h>

#ifdef __cplusplus
    extern "C"{
#endif

ULONG PSMMInitMM( char *pszWavFile, USHORT *pusDevID );
ULONG PSMMPlayWave( USHORT usDevID );
BOOL PSMMRewindWave( USHORT usDevID );
void PSMMTermMM( USHORT usDevID );
void PSMMGetmciErrorString( ULONG ulRC, char *szErrorString, USHORT usStringSize );
BOOL PSMMFillImageFormat(  char *pszFileName, MMFORMATINFO *pFormat, MMIOINFO *pIOInfo );
HMMIO PSMMInitializeImageFile( char *pszFileName, MMIOINFO *pIOInfo, MMIMAGEHEADER *pImgHeader );
BOOL PSMMReadImageFile( HMMIO hmmio, void *pvData, ULONG cbData );
void PSMMCloseImageFile( HMMIO hmmio );

#ifdef __cplusplus
    }
#endif



/*************************************************************************************************************


    $Id: psMultiMedia.h,v 1.1 1999/06/15 20:48:19 ktk Exp $
    

    $Log: psMultiMedia.h,v $
    Revision 1.1  1999/06/15 20:48:19  ktk
    Import

    Revision 1.3  1998/09/11 03:02:36  pgarner
    Add the non-military use license

    Revision 1.2  1998/09/11 02:45:49  pgarner
    'Added

    Revision 1.1  1998/06/08 14:19:02  pgarner
    Initial Checkin



*************************************************************************************************************/

#endif
