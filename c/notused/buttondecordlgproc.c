/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

	C.W. 22.09.98

******************************************************************************/


#include "setup.h"

/* Button bitmaps for settings pages */
HBITMAP hbmButtonNormal;
HBITMAP hbmButtonNormalMsk;
HBITMAP hbmButtonHilite;
HBITMAP hbmButtonHiliteMsk;
HBITMAP hbmButtonNormal2;
HBITMAP hbmButtonNormalMsk2;
HBITMAP hbmButtonHilite2;
HBITMAP hbmButtonHiliteMsk2;
ULONG ulCornerCB2;


static PFNWP oldContainerProc;

/* For DnD of bitmap-files on container windows */
/* DnD not implemented yet */
MRESULT EXPENTRY ContainerProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  RECTL rcl;
  HPS hps;
  POINTL ptl;
  BITMAPINFOHEADER bihBitmap;
	
  switch( msg )
    {	
    case WM_PAINT:
      hps=WinBeginPaint(hwnd,NULLHANDLE,&rcl);
			
      WinQueryWindowRect(hwnd,&rcl);
      WinFillRect(hps,&rcl,SYSCLR_DIALOGBACKGROUND);
			
      switch(WinQueryWindowUShort(hwnd,QWS_ID))
        {
        case COID_BUTTONNORMAL:
          /* Draw bitmap centered */ 
          if(!hbmButtonNormal)break;/* Catch error */
          GpiQueryBitmapParameters(hbmButtonNormal,&bihBitmap);
          ptl.x=(rcl.xRight-rcl.xLeft-bihBitmap.cx)/2;
          ptl.y=(rcl.yTop-rcl.yBottom-bihBitmap.cy)/2;
          WinDrawBitmap(hps,hbmButtonNormal,NULL,&ptl,0,0,DBM_NORMAL);
          break;
        case COID_BUTTONNORMALMSK:
          if(!hbmButtonNormalMsk)break;/* Catch error */
          GpiQueryBitmapParameters(hbmButtonNormalMsk,&bihBitmap);
          ptl.x=(rcl.xRight-rcl.xLeft-bihBitmap.cx)/2;
          ptl.y=(rcl.yTop-rcl.yBottom-bihBitmap.cy)/2;
          WinDrawBitmap(hps,hbmButtonNormalMsk,NULL,&ptl,0,0,DBM_NORMAL);
          break;
        case COID_BUTTONHILITE:
          /* Draw bitmap centered */ 
          if(!hbmButtonHilite)break;/* Catch error */
          GpiQueryBitmapParameters(hbmButtonHilite,&bihBitmap);
          ptl.x=(rcl.xRight-rcl.xLeft-bihBitmap.cx)/2;
          ptl.y=(rcl.yTop-rcl.yBottom-bihBitmap.cy)/2;
          WinDrawBitmap(hps,hbmButtonHilite,NULL,&ptl,0,0,DBM_NORMAL);
          break;
        case COID_BUTTONHILITEMSK:
          if(!hbmButtonHiliteMsk)break;/* Catch error */
          GpiQueryBitmapParameters(hbmButtonHiliteMsk,&bihBitmap);
          ptl.x=(rcl.xRight-rcl.xLeft-bihBitmap.cx)/2;
          ptl.y=(rcl.yTop-rcl.yBottom-bihBitmap.cy)/2;
          WinDrawBitmap(hps,hbmButtonHiliteMsk,NULL,&ptl,0,0,DBM_NORMAL);
          break;
        }
      WinEndPaint(hps);
      return 0;
    default:
      break;
    }
  return( oldContainerProc( hwnd, msg, mp1, mp2 ) );	
}


/* Dialog procedure for button decoration page 1 and 2 */
MRESULT EXPENTRY ButtonDecorDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  ULONG           ulRc;
  char            szError[ 1024 ];
  char            szProfile[ CCHMAXPATH ];
  ULONG           cbData;
  static	HINI            hIni;
  static ULONG    ulCorner;
  short           sControlId;
  short           sHelpId;
  static HMODULE  hMod = NULLHANDLE;
  BOOL rc=TRUE;
  RECTL rcl;
	
  PFN         pfnWndProc;

  switch( msg )
    {
    case WM_INITDLG:
      {
        /* Query os2.ini for the location of the CandyBarZ ini file */
        if( !PrfQueryProfileString( HINI_USERPROFILE,
                                    "CandyBarZ",
                                    "Profile",
                                    NULL,
                                    szProfile,
                                    sizeof( szProfile ) ) )
          {
            /* We have no *.ini so quit. Something is wrong. */
            PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "Failed to query profile location", 0UL );
            WinPostMsg( NULLHANDLE, WM_QUIT, ( MPARAM )0, ( MPARAM )0 );
            return( ( MRESULT )FALSE );
          }
        /* open CandyBarZ profile */
        if( ( hIni = PrfOpenProfile( WinQueryAnchorBlock(hwnd), szProfile ) ) == NULLHANDLE )
          {
            PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "Failed to open profile", 0UL );
            WinPostMsg( NULLHANDLE, WM_QUIT, ( MPARAM )0, ( MPARAM )0 );
            return( ( MRESULT )FALSE );
          }

        /* Query bitmap position */
        cbData=sizeof(ULONG);
        if( !PrfQueryProfileData( hIni, "ButtonDecor", "btCorner", &ulCorner, &cbData ) )
          ulCorner=CORNER_BOTTOMLEFT;/* default corner */

        /* Query bitmap position 2 */
        cbData=sizeof(ULONG);
        if( !PrfQueryProfileData( hIni, "ButtonDecor", "btCorner2", &ulCornerCB2, &cbData ) )
          ulCornerCB2=CORNER_BOTTOMRIGHT;/* default corner */
			
        /* Check button */
        WinCheckButton( hwnd, ulCorner, TRUE );
        /*				WinCheckButton( hwnd, ulCorner2, TRUE );
         */
        /* Disable UNDO button */
        WinEnableWindow(WinWindowFromID( hwnd, PBID_PICDIRECTORYUNDO ),FALSE);

        /* Subclass the container windows. We need this later for DnD */
        oldContainerProc=WinSubclassWindow(WinWindowFromID(hwnd,COID_BUTTONNORMAL),ContainerProc);
        WinSubclassWindow(WinWindowFromID(hwnd,COID_BUTTONNORMALMSK),ContainerProc);
        WinSubclassWindow(WinWindowFromID(hwnd,COID_BUTTONHILITE),ContainerProc);
        WinSubclassWindow(WinWindowFromID(hwnd,COID_BUTTONHILITEMSK),ContainerProc);
      }
      break;
    case WM_COMMAND:
      {
        switch( COMMANDMSG( &msg )->cmd )
          {
          case PBID_PICDIRECTORYUNDO:
            /* User pressed UNDO button */
            /* Disable UNDO button */
            WinEnableWindow(WinWindowFromID( hwnd, PBID_PICDIRECTORYUNDO ),FALSE);
            /* Get path from profile */
            if( !PrfQueryProfileString( hIni, "ButtonDecor", "PicDir", NULL ,szError,	sizeof( szError) ) )
              {
                PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "can't find previous picture directory in profile!", 0UL );
                break;
              }
            /* Set path into entryfield */
            WinSetWindowText( WinWindowFromID( hwnd, EFID_PICDIRECTORY ), szError );

            /* Query bitmap position */
            cbData=sizeof(ULONG);
            if( !PrfQueryProfileData( hIni, "ButtonDecor", "btCorner", &ulCorner, &cbData ) ) {
              PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "can't find previous position in profile!", 0UL );
              break;
            }
            /* check corner button */
            WinCheckButton( hwnd, ulCorner, TRUE );

            /* Query bitmap position */
            cbData=sizeof(ulCornerCB2);
            if( !PrfQueryProfileData( hIni, "ButtonDecor", "btCorner2", &ulCornerCB2, &cbData ) ) {
              PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "can't find previous position in profile!", 0UL );
              break;
            }
            /* check corner button 2 */
            /* ?????????????????*/
            /*	WinCheckButton( hwnd, ulCornerCB2, TRUE );*/
            break;
          }
        return( ( MPARAM )0 );
      }
      break;
    case WM_CONTROL:
      {
        switch( SHORT1FROMMP( mp1 ) )
          {
          case ARID_BUTTONTOPRIGHT:
            {
              /* enable UNDO button */
              WinEnableWindow(WinWindowFromID( hwnd, PBID_PICDIRECTORYUNDO ),TRUE);
              ulCorner=CORNER_TOPRIGHT;
            }
            break;
          case ARID_BUTTONTOPLEFT:
            {
              /* enable UNDO button */
              WinEnableWindow(WinWindowFromID( hwnd, PBID_PICDIRECTORYUNDO ),TRUE);
              ulCorner=CORNER_TOPLEFT;
            }
            break;						
          case ARID_BUTTONBOTTOMRIGHT:
            {
              /* enable UNDO button */
              WinEnableWindow(WinWindowFromID( hwnd, PBID_PICDIRECTORYUNDO ),TRUE);
              ulCorner=CORNER_BOTTOMRIGHT;
            }
            break;
          case ARID_BUTTONBOTTOMLEFT:
            {
              /* enable UNDO button */
              WinEnableWindow(WinWindowFromID( hwnd, PBID_PICDIRECTORYUNDO ),TRUE);
              ulCorner=CORNER_BOTTOMLEFT;
            }
            break;									
          }
      }
      break;
    case WM_DESTROY:
      {
        if(hIni) {
          rc=PrfWriteProfileData(   hIni,
                                    "ButtonDecor",
                                    "btCorner",
                                    &ulCorner,
                                    sizeof(ulCorner ) );
          rc&=PrfWriteProfileData(   hIni,
                                     "ButtonDecor",
                                     "btCorner2",
                                     &ulCornerCB2,
                                     sizeof(ulCornerCB2 ) );
          if(!rc)
            {
              PSUTErrorFunc(  hwnd,
                              "Error",
                              "ButtonDecorDlgProc",
                              "Failed to write profile data.",
                              0UL );
            }
					
          PrfCloseProfile( hIni );
        }
        /* Reload new settings in WNDPROCS.DLL */
        /* Load WNDPROCS.DLL */
        /*        if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
                  {
                  if( DosQueryProcAddr( hMod, 0, "changePics", &pfnWndProc ) == NO_ERROR )
                  {
                  pfnWndProc((ULONG)bButtonDecor,(ULONG)BUTTONPROC);
                  }
                  else {
                  PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "Can't load changePics function from WNDPROCS.DLL!", 0UL );
                  }
                  DosFreeModule( hMod );
                  }
                  else {
                  PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
                  }*/
      }
      break;
    case WM_HELP:
      {
        return( ( MPARAM )0 );
      }
      break;
    }
  return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
}



/*************************************************************************************************************


    $Id: buttondecordlgproc.c,v 1.1 1999/06/15 20:47:58 ktk Exp $


    $Log: buttondecordlgproc.c,v $
    Revision 1.1  1999/06/15 20:47:58  ktk
    Import

    Revision 1.6  1998/09/11 03:02:26  pgarner
    Add the non-military use license

    Revision 1.5  1998/09/11 02:45:39  pgarner
    'Added

    Revision 1.4  1998/08/31 19:35:45  enorman
    Finished the implementation of 3D Text

    Revision 1.3  1998/08/10 02:58:14  enorman
    Added alot of code to support 3D text

    Revision 1.2  1998/06/14 11:37:29  mwagner
    Added a call to 'PrfCloseProfile( hIni )' during processing of the WM_INITDLG message

    Revision 1.1  1998/06/08 14:18:57  pgarner
    Initial Checkin



*************************************************************************************************************/

