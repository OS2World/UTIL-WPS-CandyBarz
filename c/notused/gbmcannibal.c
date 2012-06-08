/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/


#include "gbmcannibal.h"

static BOOL GBMReadWord( HFILE hFile, ULONG *w );       // read a ushort
static BOOL GBMReadDWord( HFILE hFile, ULONG *d );      // read a ulong
static AHEAD *GBMCreateAhead( HFILE hFile );            // create buffer for reading compressed bitmaps
static int GBMReadAhead( AHEAD *ahead );                // read into RLE buffer
static void GBMDestroyAhead( AHEAD *ahead );            // destroy RLE buffer

// read the bitmap header
BOOL GBMReadBMPHeader( HFILE hFile, GBM *gbm )
{
    ULONG       usType;                                 // bmp or bmp array
    ULONG       cbFix;                                  // size of header
    ULONG       cPlanes;                                // color planes of bmp
    BMP_PRIV    *bmp_priv = ( BMP_PRIV * )gbm->priv;    // info re bmp file
    ULONG       ulActual;

    DosSetFilePtr( hFile, 0L, FILE_BEGIN, &ulActual );  // rewind

    GBMReadWord( hFile, &usType );                      // read type

    if( usType == BFT_BITMAPARRAY )                     // in case of array, seek to first and read type
    {
        DosSetFilePtr( hFile, 2*( sizeof( ULONG ) + sizeof( USHORT ) ), FILE_CURRENT, &ulActual );
        GBMReadWord( hFile, &usType );
    }

    if( usType != BFT_BMAP )                            // not a bitmap!
    {
        return( FALSE );
    }

    bmp_priv->base = ulActual;                          // save pointer to beginning of desired bmp
                                                        // and set file to next relevant info
    DosSetFilePtr( hFile, sizeof( ULONG ) + 2*sizeof( USHORT ), FILE_CURRENT, &ulActual );
    GBMReadDWord( hFile, &bmp_priv->offBits );          // offset to beginning of bmp
    GBMReadDWord( hFile, &cbFix );                      // get size of header

    if( cbFix == 12 )                                   // OS/2 1.x uncompressed bitmap
    {
        GBMReadWord( hFile, &gbm->w );                  // width
        GBMReadWord( hFile, &gbm->h );                  // height
        GBMReadWord( hFile, &cPlanes );                 // number of planes
        GBMReadWord( hFile, &gbm->bpp );                // bits per pixel

        if( gbm->w == 0 ||
            gbm->h == 0 ||
            cPlanes != 1 ||
            ( gbm->bpp != 1 && gbm->bpp != 4 && gbm->bpp != 8 && gbm->bpp != 24 ) )
        {
            return( FALSE );
        }

        bmp_priv->windows = FALSE;
    }
                                                        // OS/2 2.0 and Windows 3.0
    else if( cbFix >= 16 && cbFix <= 64 && ( ( cbFix & 3 ) == 0 || cbFix == 42 || cbFix == 46 ) )
    {
        GBMReadDWord( hFile, &gbm->w );                 // width
        GBMReadDWord( hFile, &gbm->h );                 // height
        GBMReadWord( hFile, &cPlanes );                 // number of planes
        GBMReadWord( hFile, &gbm->bpp );                // bits per pel

        if( cbFix > 16 )
            GBMReadDWord( hFile, &bmp_priv->ulCompression );    // compressed ?
        else
            bmp_priv->ulCompression = BCA_UNCOMP;

        if( cbFix > 28 )                                        // skip some unnecessary stuff
        {
            DosSetFilePtr( hFile, 3*sizeof( ULONG ), FILE_CURRENT, &ulActual );
        }

        if( cbFix > 32 )                                        // number of colors
            GBMReadDWord( hFile, &bmp_priv->cclrUsed );
        else
            bmp_priv->cclrUsed = ( 1 << gbm->bpp );
        if( gbm->bpp != 24 && bmp_priv->cclrUsed == 0 )
            bmp_priv->cclrUsed = ( 1 << gbm->bpp );

        if( cbFix > 60 )                                        // more unnecessary stuff
        {
            DosSetFilePtr( hFile, 4*( sizeof( ULONG ) + sizeof( USHORT ) ), FILE_CURRENT, &ulActual );
        }

        if( gbm->w == 0L ||
            gbm->h == 0L ||
            cPlanes != 1 ||
            ( gbm->bpp != 1 && gbm->bpp != 4 && gbm->bpp != 8 && gbm->bpp != 24 ) )
        {
            return( FALSE );
        }

        bmp_priv->windows       = TRUE;
        bmp_priv->cbFix         = cbFix;
    }
    else
    {
        return( FALSE );
    }

    return( TRUE );
}

// read bitmap color palette
BOOL GBMReadPalette( HFILE hFile, GBM *gbm, GBMRGB *gbmrgb )
{
    BMP_PRIV *bmp_priv = ( BMP_PRIV * )gbm->priv;
    ULONG ulActual;

    if( gbm->bpp != 24 )
    {
        int i;
        BYTE b[4];

        if( bmp_priv->windows )             // OS/2 2.0 and Windows 3.0
        {
            DosSetFilePtr(  hFile,                                  // set to beginning of palette
                            bmp_priv->base + 14L + bmp_priv->cbFix,
                            FILE_BEGIN,
                            &ulActual );

            for( i = 0; i < ( int )bmp_priv->cclrUsed; i++ )        // read colors
            {
                DosRead( hFile, b, 4, &ulActual );                  // colors are in file as bgr
                ( gbmrgb + i )->Blue    = *b;
                ( gbmrgb + i )->Green   = *( b + 1 );
                ( gbmrgb + i )->Red     = *( b + 2 );
            }
        }
        else                                                        // OS/2 1.1 and 1.2
        {
            DosSetFilePtr( hFile, bmp_priv->base + 26L, FILE_BEGIN, &ulActual );  // set to palette start

            for( i = 0; i < ( 1 << gbm->bpp ); i++ )                            // read colors
            {
                DosRead( hFile, b, 3, &ulActual );                  // colors are in file as bgr
                ( gbmrgb + i )->Blue    = *b;
                ( gbmrgb + i )->Green   = *( b + 1 );
                ( gbmrgb + i )->Red     = *( b + 2 );
            }
        }
    }

    return( TRUE );
}

// read actual bitmap data
BOOL GBMReadData( HFILE hFile, GBM *gbm, BYTE *data )
{
    BMP_PRIV *bmp_priv = ( BMP_PRIV * ) gbm->priv;
    int cLinesWorth = ( ( gbm->bpp * gbm->w + 31 ) / 32 ) * 4;  // bytes per line
    ULONG ulActual;

    if( bmp_priv->windows )                     // OS/2 2.0 and Windows 3.0
    {
        DosSetFilePtr( hFile, bmp_priv->offBits, FILE_BEGIN, &ulActual );   // set to beginning of data

        switch( ( int )bmp_priv->ulCompression )
        {
            case BCA_UNCOMP:                                            // uncompressed
            {
                DosRead( hFile, data, gbm->h * cLinesWorth, &ulActual );  // read it straight in
            }
            break;

            case BCA_RLE8:                                              // rle compression
            {
                AHEAD *ahead;
                int stride = ( ( gbm->w + 3 ) & ~3 );
                int x = 0;
                int y = 0;
                BOOL eof8 = FALSE;

                if( ( ahead = GBMCreateAhead( hFile ) ) == NULL )
                {
                    return( FALSE );
                }

                while( !eof8 )
                {
                    BYTE cCount = ( BYTE )GBMReadAhead( ahead );        // count for this color
                    BYTE cChar = ( BYTE )GBMReadAhead( ahead );         // color

                    if( cCount )
                    {
                        memset( data, cChar, cCount );                  // set data to color for count
                        x += cCount;
                        data += cCount;
                    }
                    else
                    {
                        switch( cChar )
                        {
                            case MSWCC_EOL:             // line must end on 4-byte boundary, so pad
                            {
                                int to_eol = stride - x;

                                memset( data, 0, to_eol );
                                x = 0;
                                y++;
                                data += to_eol;
                            }
                            break;

                            case MSWCC_EOB:             // end of data
                            {
                                if( y < gbm->h )
                                {
                                    int to_eol = stride - x;

                                    memset( data, 0, to_eol );
                                    x = 0;
                                    y++;
                                    data += to_eol;

                                    while( y < gbm->h )
                                    {
                                        memset( data, 0, stride );
                                        data += stride;
                                        y++;
                                    }
                                }

                                eof8 = TRUE;
                            }
                            break;

                            case MSWCC_DELTA:           // block fill?
                            {
                                BYTE dx = ( BYTE )GBMReadAhead( ahead );
                                BYTE dy = ( BYTE )GBMReadAhead( ahead );
                                int fill = dx + dy * stride;

                                x += dx;
                                y += dy;

                                memset( data, 0, fill );
                                data += fill;
                            }
                            break;

                            default:                    // an uncompressed chunk?
                            {
                                int n = ( int )cChar;

                                while( n-- > 0 )
                                {
                                    *data++ = ( BYTE )GBMReadAhead( ahead );
                                }
                                x += cChar;
                                if( cChar & 1 )
                                {
                                    GBMReadAhead( ahead ); /* Align */
                                }
                            }
                            break;
                        }
                    }
                }

                GBMDestroyAhead( ahead );
            }
            break;

            case BCA_RLE4:                                          // another rle
            {
                AHEAD *ahead;
                int x = 0;
                int y = 0;
                BOOL eof4 = FALSE;
                int inx = 0;

                if( ( ahead = GBMCreateAhead( hFile ) ) == NULL )
                {
                    return( FALSE );
                }

                memset( data, 0, gbm->h * cLinesWorth );

                while( !eof4 )
                {
                    BYTE cCount = ( BYTE )GBMReadAhead( ahead );
                    BYTE cChar = ( BYTE )GBMReadAhead( ahead );

                    if( cCount )
                    {
                        BYTE h;
                        BYTE l;
                        int i;

                        if( x & 1 )
                        {
                            h = ( BYTE )( cChar >> 4 );
                            l = ( BYTE )( cChar << 4 );
                        }
                        else
                        {
                            h = ( BYTE )( cChar & 0xf0 );
                            l = ( BYTE )( cChar & 0x0f );
                        }
                        for( i = 0; i < ( int )cCount; i++, x++ )
                        {
                            if( x & 1 )
                            {
                                data[ inx++ ] |= l;
                            }
                            else
                            {
                                data[inx] |= h;
                            }
                        }
                    }
                    else
                    {
                        switch( cCount )
                        {
                            case MSWCC_EOL:         // MSWCC_EOL
                            {
                                for( ; x < gbm->w; x++ )
                                if( x & 1 )
                                {
                                    inx++;
                                }
                                x = 0;
                                y++;
                                inx = ( ( inx + 3 ) & ~3 ); /* Align output */
                            }
                            break;

                            case MSWCC_EOB:         //MSWCC_EOB
                            {
                                eof4 = TRUE;
                            }
                            break;

                            case MSWCC_DELTA:       //MSWCC_DELTA
                            {
                                BYTE dx = ( BYTE )GBMReadAhead( ahead );
                                BYTE dy = ( BYTE )GBMReadAhead( ahead );

                                y   += dy;
                                inx += dy * cLinesWorth;

                                if( dx > 0 )
                                {
                                    if( x & 1 )
                                    {
                                        inx++;
                                        x++;
                                        dx--;
                                    }

                                    inx += ( dx / 2 );
                                    x += dx;
                                }
                            }
                            break;

                            default:            //default
                            {
                                int i, nr = 0;

                                if( x & 1 )
                                {
                                    for( i = 0; i < ( int )cChar; i += 2 )
                                    {
                                        BYTE b = ( BYTE )GBMReadAhead( ahead );

                                        data[ inx++ ] |= ( b >> 4 );
                                        data[ inx ] |= ( b << 4 );
                                        nr++;
                                    }
                                    if( i < ( int )cChar )
                                    {
                                        data[ inx++ ] |= ( ( BYTE )GBMReadAhead( ahead ) >> 4 );
                                        nr++;
                                    }
                                }
                                else
                                {
                                    for( i = 0; i < ( int )cChar; i += 2 )
                                    {
                                        data[ inx++ ] = ( BYTE )GBMReadAhead( ahead );
                                        nr++;
                                    }
                                    if( i < ( int )cChar )
                                    {
                                        data[ inx ] = ( BYTE )GBMReadAhead( ahead );
                                        nr++;
                                    }
                                }
                                x += cChar;

                                if( nr & 1 )
                                {
                                    GBMReadAhead( ahead ); /* Align input stream to next word */
                                }
                            }
                            break;
                        }
                    }
                }

                GBMDestroyAhead(ahead);
            }
            break;

            default:                    //sdefault
            {
                return( FALSE );
            }
        }
    }
    else                                    // OS/2 1.1 and 1.2
    {                                       // uncompressed
        DosSetFilePtr( hFile, bmp_priv->offBits, FILE_BEGIN, &ulActual );
        DosRead( hFile, data, cLinesWorth * gbm->h, &ulActual );
    }
    return( TRUE );
}

static BOOL GBMReadWord( HFILE hFile, ULONG *w )
{
    BYTE low = 0;
    BYTE high = 0;
    ULONG ulActual;

    DosRead( hFile, &low, 1, &ulActual );
    if( ulActual != 1 )
    {
        return( FALSE );
    }
    DosRead( hFile, &high, 1, &ulActual );
    if( ulActual != 1 )
    {
        return( FALSE );
    }

    *w = ( ULONG )( low + ( ( USHORT )high << 8 ) );

    return( TRUE );
}

static BOOL GBMReadDWord( HFILE hFile, ULONG *d )
{
    ULONG low;
    ULONG high;

    GBMReadWord( hFile, &low );
    GBMReadWord( hFile, &high );

    *d = low + ( high << 16 );
    return( TRUE );
}

static AHEAD *GBMCreateAhead( HFILE hFile )
{
    AHEAD *ahead;

    if( DosAllocMem( ( PPVOID )&ahead, sizeof( AHEAD ), PAG_READ | PAG_WRITE | PAG_COMMIT ) != 0 )
    {
        return NULL;
    }

    ahead->inx = 0;
    ahead->cnt = 0;
    ahead->hFile  = hFile;

    return ahead;
}

static int GBMReadAhead( AHEAD *ahead )
{
    if( ahead->inx >= ahead->cnt )
    {
        DosRead( ahead->hFile, ahead->pchBuf, AHEAD_BUF, &ahead->cnt );
        if( ahead->cnt == 0 )
        {
            return( -1 );
        }
        ahead->inx = 0;
    }

    return( ( int )ahead->pchBuf[ahead->inx++] );
}

static void GBMDestroyAhead( AHEAD *ahead )
{
    DosFreeMem( ahead );
}



/*************************************************************************************************************


    $Id: gbmcannibal.c,v 1.1 1999/06/15 20:47:59 ktk Exp $
    

    $Log: gbmcannibal.c,v $
    Revision 1.1  1999/06/15 20:47:59  ktk
    Import

    Revision 1.3  1998/09/11 03:02:31  pgarner
    Add the non-military use license

    Revision 1.2  1998/09/11 02:45:44  pgarner
    'Added

    Revision 1.1  1998/06/08 14:18:59  pgarner
    Initial Checkin



*************************************************************************************************************/

