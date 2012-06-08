/******************************************************************************
  Copyright Netlabs, 1998, this code may not be used for any military purpose
******************************************************************************/

#ifndef _PSRT_H_
#define _PSRT_H_

#define NO_MMPM2

#define INCL_WIN
#define INCL_OS2MM
#define INCL_MCIOS2
#define INCL_GPI
#include <os2.h>
#include <os2me.h>
#include <string.h>
#include <stdio.h>
#include <mmioos2.h>
#define PSRT_VERSION    0x0000000D
#define SCALE_LARGEST       0x00000001L
#define SCALE_SMALLEST      0x00000002L
#define SCALE_HORIZONTAL    0x00000004L
#define SCALE_VERTICAL      0x00000008L
#ifdef __cplusplus
extern "C"
{
#endif

//    BOOL PSMMFillImageFormat(char *pszFileName, MMFORMATINFO * pFormat, MMIOINFO * pIOInfo);
//    HMMIO PSMMInitializeImageFile(char *pszFileName, MMIOINFO * pIOInfo, MMIMAGEHEADER * pImgHeader);
//    BOOL PSMMReadImageFile(HMMIO hmmio, void *pvData, ULONG cbData);
//    void PSMMCloseImageFile(HMMIO hmmio);
    void PSUTErrorFunc(HWND hwnd, char *Type, char *Location, char *Msg, ULONG ulStyles);
    int PSUTVersion(void);
//    HBITMAP PSBMBLTToSize(HAB hab, HPS hps, HBITMAP hbm, float fScale, ULONG ulOptions);
#ifdef __cplusplus
}
#endif

/*************************************************************************************************************

    $Id: psrt.h,v 1.1 1999/06/15 20:48:01 ktk Exp $

    $Log: psrt.h,v $
    Revision 1.1  1999/06/15 20:48:01  ktk
    Import

    Revision 1.1  1998/10/17 19:33:37  enorman
    Initial revision

*************************************************************************************************************/

#endif                          //PSRT_H
