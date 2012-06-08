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


/* Dialog procedure for button decoration page 1 and 2 */
MRESULT EXPENTRY ThemesMainDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  HWND            hwndFocus;
  ULONG           ulRc;
  char            szError[ 1024 ];
  static BOOL     bButtonDecor;
  static BOOL     bFrameCntrl;
  static BOOL     bFrameBackground;
  static BOOL     bStatic;
  char            szProfile[ CCHMAXPATH ];
  ULONG           cbData;
  static	HINI            hIni;
  short           sControlId;
  short           sHelpId;
  static HMODULE  hMod = NULLHANDLE;
  static HPS hps;
  FILEDLG fd = { 0 };
  ULONG ulDriveNum;
  ULONG ulDriveMap;
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
            PSUTErrorFunc( hwnd, "Error", "ThemesMainDecorDlgProc", "Failed to query profile location", 0UL );
            WinPostMsg( NULLHANDLE, WM_QUIT, ( MPARAM )0, ( MPARAM )0 );
            return( ( MRESULT )FALSE );
          }
        /* open CandyBarZ profile */
        if( ( hIni = PrfOpenProfile( WinQueryAnchorBlock(hwnd), szProfile ) ) == NULLHANDLE )
          {
            PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Failed to open profile", 0UL );
            WinPostMsg( NULLHANDLE, WM_QUIT, ( MPARAM )0, ( MPARAM )0 );
            return( ( MRESULT )FALSE );
          }
        /* check to see if button decoration is enabled */
        cbData = sizeof( bButtonDecor );
        if( !PrfQueryProfileData( hIni, "ButtonDecor", "enable", &bButtonDecor, &cbData ) )
          {
            bButtonDecor = TRUE;
            /* Activate procedure as default */
            if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
              {
                if( DosQueryProcAddr( hMod, 0, "enable", &pfnWndProc ) == NO_ERROR )
                  {
                    pfnWndProc((ULONG)bButtonDecor,(ULONG)BUTTONPROC);/* Enable buttons */
                  }
                else {
                  PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc",
                                 "Can't load enable function from WNDPROCS.DLL!", 0UL );
                }
                DosFreeModule( hMod );
              }
            else {
              PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", 
                             "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
            }
          }
        /* check to see if backgrounds are enabled */
        cbData = sizeof( bFrameBackground );
        if( !PrfQueryProfileData( hIni, "FrameBackground", "enable", &bFrameBackground, &cbData ) )
          {
            bFrameBackground = TRUE;
            /* Activate procedure as default */
            if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
              {
                if( DosQueryProcAddr( hMod, 0, "enable", &pfnWndProc ) == NO_ERROR )
                  {
                    pfnWndProc((ULONG)bFrameBackground,(ULONG)FRAMEPROC);/* Enable frame backgrounds */
                  }
                else {
                  PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc",
                                 "Can't load enable function from WNDPROCS.DLL!", 0UL );
                }
                DosFreeModule( hMod );
              }
            else {
              PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", 
                             "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
            }
          }
        /* check to see if frame control replacement is enabled */
        cbData = sizeof( bFrameCntrl );
        if( !PrfQueryProfileData( hIni, "FrameCntrl", "enable", &bFrameCntrl, &cbData ) )
          {
            bFrameCntrl = TRUE;
            /* Activate procedure as default */
            if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
              {
                if( DosQueryProcAddr( hMod, 0, "enable", &pfnWndProc ) == NO_ERROR )
                  {
                    pfnWndProc((ULONG)bFrameCntrl,(ULONG)FRAMECNTRLREPL);/* Enable frame backgrounds */
                  }
                else {
                  PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc",
                                 "Can't load enable function from WNDPROCS.DLL!", 0UL );
                }
                DosFreeModule( hMod );
              }
            else {
              PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", 
                             "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
            }
          }
        /* check to see if static text controls are enabled */
        cbData = sizeof( bStatic );
        if( !PrfQueryProfileData( hIni, "StaticText", "enable", &bStatic, &cbData ) )
          {
            bStatic = TRUE;
            /* Activate procedure as default */
            if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
              {
                if( DosQueryProcAddr( hMod, 0, "enable", &pfnWndProc ) == NO_ERROR )
                  {
                    pfnWndProc((ULONG)bStatic,(ULONG)STATICPROC);/* Enable frame backgrounds */
                  }
                else {
                  PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc",
                                 "Can't load enable function from WNDPROCS.DLL!", 0UL );
                }
                DosFreeModule( hMod );
              }
            else {
              PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", 
                             "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
            }
          }
        /* Query default picture directory */
        if( !PrfQueryProfileString( hIni, "Themes", "ThemesDir", NULL ,szError,	sizeof( szError) ) )
          {
            /* Can't find entry in candybarz.ini so build default path */
            strcpy(szProfile,"");
            cbData = sizeof( szProfile );
            if(DosQueryCurrentDisk(&ulDriveNum,&ulDriveMap)!=NO_ERROR) {/* Query current disk */
              PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "DosQueryCurrentDisk() failed!", 0UL );
            }
            else {
              if(DosQueryCurrentDir(0,szProfile,&cbData)!=NO_ERROR)/* Query current dir */
                PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "DosQueryCurrentDir() failed!", 0UL );
              sprintf(szError,"%c:\\%s\\themes\\",'A'+ulDriveNum-1,szProfile);
            }
          }
        /* Set max textlength for entry field */
        WinSendMsg( WinWindowFromID( hwnd, IDEF_PICTUREDIR ), EM_SETTEXTLIMIT, MPFROMSHORT(CCHMAXPATH),0 );
        /* Fill entry field with path */
        WinSetWindowText( WinWindowFromID( hwnd, IDEF_PICTUREDIR ), szError );

			
        /* Check button */
        WinCheckButton( hwnd, IDCB_ENABLEBUTTONS, bButtonDecor );
        WinCheckButton( hwnd, IDCB_FRAMEBACKGROUND, bFrameBackground );
        WinCheckButton( hwnd, IDCB_ENABLETEXTCONTROLS, bStatic );
        WinCheckButton( hwnd, IDCB_REPLACEFRAMECTRL, bFrameCntrl );

        /* Disable UNDO button */
        WinEnableWindow(WinWindowFromID( hwnd, IDPB_UNDO ),FALSE);

        /* Load the bitmaps */
        hps=WinGetPS(hwnd);
        sprintf(szProfile,"%secke1.bmp",szError);
        hbmButtonNormal=ReadBitmap(szProfile,hps);
        sprintf(szProfile,"%secke1msk.bmp",szError);
        hbmButtonNormalMsk=ReadBitmap(szProfile,hps);
        sprintf(szProfile,"%seckeein1.bmp",szError);
        hbmButtonHilite=ReadBitmap(szProfile,hps);
        sprintf(szProfile,"%seein1msk.bmp",szError);
        hbmButtonHiliteMsk=ReadBitmap(szProfile,hps);
        /* Load the bitmaps for 2nd page */
        sprintf(szProfile,"%secke2.bmp",szError);
        hbmButtonNormal2=ReadBitmap(szProfile,hps);
        sprintf(szProfile,"%secke2msk.bmp",szError);
        hbmButtonNormalMsk2=ReadBitmap(szProfile,hps);
        sprintf(szProfile,"%seckeein2.bmp",szError);
        hbmButtonHilite2=ReadBitmap(szProfile,hps);
        sprintf(szProfile,"%seein2msk.bmp",szError);
        hbmButtonHiliteMsk2=ReadBitmap(szProfile,hps);
        WinReleasePS(hps);
      }
      break;
    case WM_COMMAND:
      {
        switch( COMMANDMSG( &msg )->cmd )
          {
          case IDPB_BROWSE:
            {
              /* Fill file dialog struct */
              fd.cbSize = sizeof( fd );
              fd.fl = FDS_OPEN_DIALOG;
              fd.pszTitle = "Choose the picture directory";
              fd.pfnDlgProc = INIDestFileDlgProc;
              /* Show file dialog */
              if( WinFileDlg( HWND_DESKTOP, hwnd, &fd ) == NULLHANDLE )
                {
                  PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "WinFileDlg for picture directory failed!", 0UL );
                  break;
                }
              if( fd.lReturn == DID_OK )
                {
                  /* Enable UNDO button */
                  WinEnableWindow(WinWindowFromID( hwnd, IDPB_UNDO ),TRUE);
                  /* Set path into entryfield */
                  WinSetWindowText( WinWindowFromID( hwnd, IDEF_PICTUREDIR ), fd.szFullFile );
                  /* Free old bitmaps */
                  if(hbmButtonNormal)
                    GpiDeleteBitmap(hbmButtonNormal);
                  if(hbmButtonNormalMsk)
                    GpiDeleteBitmap(hbmButtonNormalMsk);
                  if(hbmButtonHilite)
                    GpiDeleteBitmap(hbmButtonHilite);
                  if(hbmButtonHiliteMsk)
                    GpiDeleteBitmap(hbmButtonHiliteMsk);
                  /* Load new bitmaps */
                  hps=WinGetPS(hwnd);
                  sprintf(szProfile,"%secke1.bmp",fd.szFullFile);
                  hbmButtonNormal=ReadBitmap(szProfile,hps);
                  sprintf(szProfile,"%secke1msk.bmp",fd.szFullFile);
                  hbmButtonNormalMsk=ReadBitmap(szProfile,hps);
                  sprintf(szProfile,"%seckeein1.bmp",fd.szFullFile);
                  hbmButtonHilite=ReadBitmap(szProfile,hps);
                  sprintf(szProfile,"%seein1msk.bmp",fd.szFullFile);
                  hbmButtonHiliteMsk=ReadBitmap(szProfile,hps);
                  sprintf(szProfile,"%secke2.bmp",fd.szFullFile);
                  hbmButtonNormal2=ReadBitmap(szProfile,hps);
                  sprintf(szProfile,"%secke2msk.bmp",fd.szFullFile);
                  hbmButtonNormalMsk2=ReadBitmap(szProfile,hps);
                  sprintf(szProfile,"%seckeein2.bmp",fd.szFullFile);
                  hbmButtonHilite2=ReadBitmap(szProfile,hps);
                  sprintf(szProfile,"%seein2msk.bmp",fd.szFullFile);
                  hbmButtonHiliteMsk2=ReadBitmap(szProfile,hps);
                  WinReleasePS(hps);
                  /* refresh the page */
                  WinQueryWindowRect(hwnd,&rcl);
                  WinInvalidateRect(hwnd,&rcl,TRUE);
                }
            }
            break;
          case IDPB_UNDO:
            /* User pressed UNDO button */
            /* Disable UNDO button */
            WinEnableWindow(WinWindowFromID( hwnd, IDPB_UNDO ),FALSE);
            /* Get path from profile */
            if( !PrfQueryProfileString( hIni, "Themes", "ThemesDir", NULL ,szError,	sizeof( szError) ) )
              {
                PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "can't find previous picture directory in profile!", 0UL );
                break;
              }
            /* Set path into entryfield */
            WinSetWindowText( WinWindowFromID( hwnd, IDEF_PICTUREDIR ), szError );

            cbData=sizeof(bButtonDecor);
			if( !PrfQueryProfileData( hIni, "ButtonDecor", "enable", &bButtonDecor, &cbData ) ) {
              PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "can't find previous value for buttons in profile!", 0UL );
              break;
            }
            WinCheckButton( hwnd, IDCB_ENABLEBUTTONS, bButtonDecor );

            cbData=sizeof(bFrameBackground);
			if( !PrfQueryProfileData( hIni, "FrameBackground", "enable", &bFrameBackground, &cbData ) ) {
              PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "can't find previous value for backgrounds in profile!", 0UL );
              break;
            }
            WinCheckButton( hwnd, IDCB_FRAMEBACKGROUND, bFrameBackground );
          
            cbData=sizeof(bStatic);
			if( !PrfQueryProfileData( hIni, "StaticText", "enable", &bStatic, &cbData ) ) {
              PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "can't find previous value for text controls in profile!", 0UL );
              break;
            }
            WinCheckButton( hwnd, IDCB_ENABLETEXTCONTROLS, bStatic );
 
            cbData=sizeof(bFrameCntrl);
			if( !PrfQueryProfileData( hIni, "FrameCntrl", "enable", &bFrameCntrl, &cbData ) ) {
              PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "can't find previous value for frame controls in profile!", 0UL );
              break;
            }
            WinCheckButton( hwnd, IDCB_REPLACEFRAMECTRL, bFrameCntrl );
            /* Free old bitmaps */
            if(hbmButtonNormal)
              GpiDeleteBitmap(hbmButtonNormal);
            if(hbmButtonNormalMsk)
              GpiDeleteBitmap(hbmButtonNormalMsk);
            if(hbmButtonHilite)
              GpiDeleteBitmap(hbmButtonHilite);
            if(hbmButtonHiliteMsk)
              GpiDeleteBitmap(hbmButtonHiliteMsk);
            /* Load new bitmaps */
            WinGetPS(hwnd);
            sprintf(szProfile,"%secke1.bmp",szError);
            hbmButtonNormal=ReadBitmap(szProfile,hps);
            sprintf(szProfile,"%secke1msk.bmp",szError);
            hbmButtonNormalMsk=ReadBitmap(szProfile,hps);
            sprintf(szProfile,"%seckeein1.bmp",szError);
            hbmButtonHilite=ReadBitmap(szProfile,hps);
            sprintf(szProfile,"%seein1msk.bmp",szError);
            hbmButtonHiliteMsk=ReadBitmap(szProfile,hps);
            sprintf(szProfile,"%secke2.bmp",szError);
            hbmButtonNormal2=ReadBitmap(szProfile,hps);
            sprintf(szProfile,"%secke2msk.bmp",szError);
            hbmButtonNormalMsk2=ReadBitmap(szProfile,hps);
            sprintf(szProfile,"%seckeein2.bmp",szError);
            hbmButtonHilite2=ReadBitmap(szProfile,hps);
            sprintf(szProfile,"%seein2msk.bmp",szError);
            hbmButtonHiliteMsk2=ReadBitmap(szProfile,hps);
            WinReleasePS(hps);
            /* Reenable features */
            if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
              {
                if( DosQueryProcAddr( hMod, 0, "enable", &pfnWndProc ) == NO_ERROR )
                  {
                    pfnWndProc((ULONG)bButtonDecor,(ULONG)BUTTONPROC);/* Enable buttons */
                    pfnWndProc((ULONG)bStatic,(ULONG)STATICPROC);/* Static text controls */
                    pfnWndProc((ULONG)bFrameBackground,(ULONG)FRAMEPROC);/* Enable frame backgrounds */
                    pfnWndProc((ULONG)bFrameCntrl,(ULONG)FRAMECNTRLREPL);/* Replace frame controls */
                  }
                else {
                  PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Can't load enable function from WNDPROCS.DLL!", 0UL );
                }
                DosFreeModule( hMod );
              }
            else {
              PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
            }
            
            /* refresh the page */
            WinQueryWindowRect(hwnd,&rcl);
            WinInvalidateRect(hwnd,&rcl,TRUE);
            break;
          }
        return( ( MPARAM )0 );
      }
      break;
    case WM_CONTROL:
      {
        switch( SHORT1FROMMP( mp1 ) )
          {
          case IDCB_ENABLEBUTTONS:
            {                   
              WinEnableWindow(WinWindowFromID( hwnd, IDPB_UNDO ),TRUE);
              bButtonDecor = WinQueryButtonCheckstate( hwnd, IDCB_ENABLEBUTTONS );
              if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
                {
                  if( DosQueryProcAddr( hMod, 0, "enable", &pfnWndProc ) == NO_ERROR )
                    {
                      pfnWndProc((ULONG)bButtonDecor,(ULONG)BUTTONPROC);/* Enable buttons */
                    }
                  else {
                    PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Can't load enable function from WNDPROCS.DLL!", 0UL );
                  }
                  DosFreeModule( hMod );
                }
              else {
                PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
              }
            }
            break;
          case IDCB_ENABLETEXTCONTROLS:
            {                   
              WinEnableWindow(WinWindowFromID( hwnd, IDPB_UNDO ),TRUE);
              bStatic = WinQueryButtonCheckstate( hwnd, IDCB_ENABLETEXTCONTROLS );
              if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
                {
                  if( DosQueryProcAddr( hMod, 0, "enable", &pfnWndProc ) == NO_ERROR )
                    {
                      pfnWndProc((ULONG)bStatic,(ULONG)STATICPROC);/* Static text controls */
                    }
                  else {
                    PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Can't load enable function from WNDPROCS.DLL!", 0UL );
                  }
                  DosFreeModule( hMod );
                }
              else {
                PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
              }
            }
            break;
          case IDCB_FRAMEBACKGROUND:
            {                   
              WinEnableWindow(WinWindowFromID( hwnd, IDPB_UNDO ),TRUE);
              bFrameBackground = WinQueryButtonCheckstate( hwnd, IDCB_FRAMEBACKGROUND );
              if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
                {
                  if( DosQueryProcAddr( hMod, 0, "enable", &pfnWndProc ) == NO_ERROR )
                    {
                      pfnWndProc((ULONG)bFrameBackground,(ULONG)FRAMEPROC);/* Enable frame backgrounds */
                    }
                  else {
                    PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Can't load enable function from WNDPROCS.DLL!", 0UL );
                  }
                  DosFreeModule( hMod );
                }
              else {
                PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
              }
            }
            break;
          case IDCB_REPLACEFRAMECTRL:
            {                   
              WinEnableWindow(WinWindowFromID( hwnd, IDPB_UNDO ),TRUE);
              bFrameCntrl = WinQueryButtonCheckstate( hwnd, IDCB_REPLACEFRAMECTRL );
              if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
                {
                  if( DosQueryProcAddr( hMod, 0, "enable", &pfnWndProc ) == NO_ERROR )
                    {
                      pfnWndProc((ULONG)bFrameCntrl,(ULONG)FRAMECNTRLREPL);/* Replace frame controls */
                    }
                  else {
                    PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Can't load enable function from WNDPROCS.DLL!", 0UL );
                  }
                  DosFreeModule( hMod );
                }
              else {
                PSUTErrorFunc( hwnd, "Error", "ThemesMainDlgProc", "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
              }
            }
            break;
          }
        /* refresh the page */
        WinQueryWindowRect(hwnd,&rcl);
        WinInvalidateRect(hwnd,&rcl,TRUE);
      }
      break;
    case WM_DESTROY:
      {
        if(hIni) {
          rc=PrfWriteProfileData(   hIni,
                                    "ButtonDecor",
                                    "enable",
                                    &bButtonDecor,
                                    sizeof( bButtonDecor ) );
          rc&=PrfWriteProfileData(   hIni,
                                     "FrameCntrl",
                                     "enable",
                                     &bFrameCntrl,
                                     sizeof(bFrameCntrl ) );
          rc&=PrfWriteProfileData(   hIni,
                                     "StaticText",
                                     "enable",
                                     &bStatic,
                                     sizeof(bStatic ) );
          rc&=PrfWriteProfileData(   hIni,
                                     "FrameBackground",
                                     "enable",
                                     &bFrameBackground,
                                     sizeof(bFrameBackground ) );
          /* Save path to themes directory */
          WinQueryWindowText(WinWindowFromID(hwnd,IDEF_PICTUREDIR ),sizeof(szError),szError);
          rc&=PrfWriteProfileString(   hIni,
                                       "Themes",
                                       "ThemesDir",
                                       szError );
          if(!rc)
            {
              PSUTErrorFunc(  hwnd,
                              "Error",
                              "ThemesMainDlgProc",
                              "Failed to write profile data.",
                              0UL );
            }
					
          PrfCloseProfile( hIni );
        }
        /* Free bitmaps */
        if(hbmButtonNormal)
          GpiDeleteBitmap(hbmButtonNormal);
        if(hbmButtonNormalMsk)
          GpiDeleteBitmap(hbmButtonNormalMsk);
        if(hbmButtonHilite)
          GpiDeleteBitmap(hbmButtonHilite);
        if(hbmButtonHiliteMsk)
          GpiDeleteBitmap(hbmButtonHiliteMsk);
        if(hps)WinReleasePS(hps);			
        if(hbmButtonNormal2)
          GpiDeleteBitmap(hbmButtonNormal2);
        if(hbmButtonNormalMsk2)
          GpiDeleteBitmap(hbmButtonNormalMsk2);
        if(hbmButtonHilite2)
          GpiDeleteBitmap(hbmButtonHilite2);
        if(hbmButtonHiliteMsk2)
          GpiDeleteBitmap(hbmButtonHiliteMsk2);

        /* Reload new settings in WNDPROCS.DLL */
        /* Load WNDPROCS.DLL */
        if( DosLoadModule( szError, sizeof( szError ), "WNDPROCS", &hMod ) == NO_ERROR )
          {
            if( DosQueryProcAddr( hMod, 0, "changePics", &pfnWndProc ) == NO_ERROR )
              {
                pfnWndProc((ULONG)bButtonDecor,(ULONG)4);/* change the pics */
              }
            else {
              PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "Can't load changePics function from WNDPROCS.DLL!", 0UL );
            }
            DosFreeModule( hMod );
          }
        else {
          PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "Can't load WNDPROCS.DLL. Check your installation.", 0UL );
        }
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


    $Id: themesmaindlgproc.c,v 1.1 1999/06/15 20:48:00 ktk Exp $


    $Log: themesmaindlgproc.c,v $
    Revision 1.1  1999/06/15 20:48:00  ktk
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

