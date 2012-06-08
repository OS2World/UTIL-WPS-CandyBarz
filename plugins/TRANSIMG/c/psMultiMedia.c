/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/


#include "psmultimedia.h"

ULONG PSMMInitMM( char *pszWavFile, USHORT *pusDevID )
{
    MCI_OPEN_PARMS  mop;
    ULONG           ulRC;

    memset( &mop, 0, sizeof( mop ) );

    if( !pszWavFile || !*pszWavFile )
        mop.pszElementName = "roadkill.wav";
    else
        mop.pszElementName = pszWavFile;

    ulRC = mciSendCommand(  0,
                            MCI_OPEN,
                            MCI_WAIT | MCI_OPEN_ELEMENT | MCI_READONLY | MCI_OPEN_SHAREABLE,
                            &mop,
                            0 );
    *pusDevID = mop.usDeviceID;
    return( ulRC );
}

void PSMMTermMM( USHORT usDevID )
{
    MCI_GENERIC_PARMS   mgenp;
    ULONG               ulRC;

    memset( &mgenp, 0, sizeof( mgenp ) );
    ulRC = mciSendCommand(  usDevID,
                            MCI_CLOSE,
                            0L,
                            &mgenp,
                            0 );
    return;
}

ULONG PSMMPlayWave( USHORT usDevID )
{
    MCI_PLAY_PARMS      mpp;
    MCI_GENERIC_PARMS   mgp;
    MCI_SEEK_PARMS      mseekp;
    ULONG               ulRC;
    char                pszErrorString[ 128 ];

    memset( &mgp, 0, sizeof( mgp ) );
    ulRC = mciSendCommand(  usDevID,
                            MCI_ACQUIREDEVICE,
                            MCI_WAIT,
                            &mgp,
                            0 );
    if( ulRC != MCIERR_SUCCESS )
    {
        return( ulRC );
    }

    memset( &mseekp, 0, sizeof( mseekp ) );
    ulRC = mciSendCommand(  usDevID,
                            MCI_SEEK,
                            MCI_WAIT | MCI_TO_START,
                            &mseekp,
                            0 );
    if( ulRC != MCIERR_SUCCESS )
    {
        return( ulRC );
    }

    memset( &mpp, 0, sizeof( mpp ) );
    ulRC = mciSendCommand(  usDevID,
                            MCI_PLAY,
                            0L,
                            &mpp,
                            0 );
    return( ulRC );
}

BOOL PSMMRewindWave( USHORT usDevID )
{
    MCI_SEEK_PARMS      mseekp;
    MCI_GENERIC_PARMS   mgenp;
    ULONG               ulRC;
    char                pszErrBuf[ 128 ];

    if( !usDevID )
    {
        return( FALSE );
    }

    memset( &mseekp, 0, sizeof( mseekp ) );
    memset( &mgenp, 0, sizeof( mgenp ) );

    ulRC = mciSendCommand(  usDevID,
                            MCI_ACQUIREDEVICE,
                            MCI_WAIT,
                            &mgenp,
                            0 );

    ulRC = mciSendCommand(  usDevID,
                            MCI_SEEK,
                            MCI_WAIT | MCI_TO_START,
                            &mseekp,
                            0 );

    if( LOUSHORT( ulRC ) != MCIERR_SUCCESS )
    {
        mciGetErrorString( ulRC, pszErrBuf, sizeof( pszErrBuf ) );
        return( FALSE );
    }

    return( TRUE );
}

void PSMMGetmciErrorString( ULONG ulRC, char *szErrorString, USHORT usStringSize )
{
    mciGetErrorString( ulRC, szErrorString, usStringSize );

    return;
}

BOOL PSMMFillImageFormat(  char *pszFileName, MMFORMATINFO *pFormat, MMIOINFO *pIOInfo )
{
    FOURCC fcc;

    if( !pszFileName || !pFormat || !pIOInfo )
    {
        return( FALSE );
    }
    if( mmioIdentifyFile( pszFileName, NULL, pFormat, &fcc, 0UL, 0UL ) != MMIO_SUCCESS )
    {
        return( FALSE );
    }
    if( pFormat->fccIOProc == FOURCC_DOS )
    {
        return( FALSE );
    }
    if( ( pFormat->ulMediaType != MMIO_MEDIATYPE_IMAGE ) ||
        ( ( pFormat->ulFlags & MMIO_CANREADTRANSLATED ) == 0 ) )
    {
        return( FALSE );
    }

    pIOInfo->fccIOProc = pFormat->fccIOProc;
    pIOInfo->ulTranslate = MMIO_TRANSLATEHEADER | MMIO_TRANSLATEDATA;

    return( TRUE );
}

HMMIO PSMMInitializeImageFile( char *pszFileName, MMIOINFO *pIOInfo, MMIMAGEHEADER *pImgHeader )
{
    HMMIO   hMMIO;
    long    cbHeader;

    if( !pszFileName || !pIOInfo || !pImgHeader )
    {
        return( NULLHANDLE );
    }
    if( ( hMMIO = mmioOpen( pszFileName,
                            pIOInfo,
                            MMIO_READ | MMIO_DENYNONE | MMIO_NOIDENTIFY ) ) == NULLHANDLE )
    {
        return( NULLHANDLE );
    }
    if( mmioQueryHeaderLength(  hMMIO, &cbHeader, 0UL, 0UL ) != MMIO_SUCCESS ||
        cbHeader != sizeof( MMIMAGEHEADER ) )
    {
        mmioClose( hMMIO, 0L );
        return( NULLHANDLE );
    }
    if( mmioGetHeader( hMMIO, ( PVOID )pImgHeader, ( long )sizeof( MMIMAGEHEADER ), &cbHeader, 0L, 0L )
        != MMIO_SUCCESS )
    {
        mmioClose( hMMIO, 0L );
        return( NULLHANDLE );
    }

    return( hMMIO );
}

BOOL PSMMReadImageFile( HMMIO hmmio, void *pvData, ULONG cbData )
{
    long cbRead;

    if( !hmmio || !pvData || !cbData )
    {
        return( FALSE );
    }
    if( ( cbRead =  mmioRead( hmmio, pvData, cbData ) ) == MMIO_ERROR || !cbRead )
    {
        return( FALSE );
    }

    return( TRUE );
}

void PSMMCloseImageFile( HMMIO hmmio )
{
    mmioClose( hmmio, 0L );

    return;
}



/*************************************************************************************************************


    $Id: psMultiMedia.c,v 1.1.1.1 2000/09/08 22:20:32 cwohlgemuth Exp $
    

    $Log: psMultiMedia.c,v $
    Revision 1.1.1.1  2000/09/08 22:20:32  cwohlgemuth
    Transparent images initial checkin

    Revision 1.1  1999/06/15 20:48:07  ktk
    Import

    Revision 1.3  1998/09/11 03:02:33  pgarner
    Add the non-military use license

    Revision 1.2  1998/09/11 02:45:47  pgarner
    'Added

    Revision 1.1  1998/06/08 14:19:00  pgarner
    Initial Checkin



*************************************************************************************************************/

