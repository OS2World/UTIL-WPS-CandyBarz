/* $Id: GlobalsDlgProc.c,v 1.1 1999/06/15 20:47:58 ktk Exp $ */

#include "setup.h"
// dialog proc for Active default colors page of notebook
MRESULT EXPENTRY GlobalsDlgProc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
	 char                szProfile[ CCHMAXPATH ];
	 HINI                hIni;
	 BOOL                bEnabled;
	 HATOMTBL				hAtomTbl = WinQuerySystemAtomTable();
	 ATOM                Atom;
	 PSTBSHARE				*ptbShare;
	 HAB						hab = WinQueryAnchorBlock( hwnd );
	 ULONG					cbData;

	 switch( msg )
	 {
		  case WM_INITDLG:
		  {
				// locate profile
				if( !PrfQueryProfileString( HINI_USERPROFILE,
													 "CandyBarZ",
													 "Profile",
													 NULL,
													 szProfile,
													 sizeof( szProfile ) ) )
				{
					 PSUTErrorFunc( hwnd, "Error", "ActiveColorDlgProc", "Failed to query profile location", 0UL );
					 WinPostMsg( NULLHANDLE, WM_QUIT, ( MPARAM )0, ( MPARAM )0 );
					 return( ( MRESULT )FALSE );
				}

				// open it
				if( ( hIni = PrfOpenProfile( hab, szProfile ) ) == NULLHANDLE )
				{
					 PSUTErrorFunc( hwnd, "Error", "ActiveColorDlgProc", "Failed to open profile", 0UL );
					 WinPostMsg( NULLHANDLE, WM_QUIT, ( MPARAM )0, ( MPARAM )0 );
					 return( ( MRESULT )FALSE );
				}

				// see if pmview fix is enabled
				cbData = sizeof( bEnabled );
				if( PrfQueryProfileData( hIni, "UserOptions", "PMVIEW", &bEnabled, &cbData ) &&
					 bEnabled )
				{
					 WinCheckButton( hwnd, CHID_PMVIEW, TRUE );
				}

				// check to see if we're enabled
				cbData = sizeof( bEnabled );
				if( PrfQueryProfileData( hIni, "UserOptions", "Enabled", &bEnabled, &cbData ) && bEnabled )
				{
					 WinCheckButton( hwnd, CHID_ENABLECB, TRUE );
				}

				// check to see if S3 fix is enabled
				cbData = sizeof( bEnabled );
				if( PrfQueryProfileData( hIni, "UserOptions", "S3Fix", &bEnabled, &cbData ) && bEnabled )
				{
					 WinCheckButton( hwnd, CHID_ENABLES3, TRUE );
				}
				PrfCloseProfile( hIni );


		  }
		  break;

		  case WM_CONTROL:
		  {
				switch( SHORT1FROMMP( mp1 ) )
				{

					 case CHID_ENABLECB:
					 {
						  bEnabled = WinQueryButtonCheckstate( hwnd, CHID_ENABLECB );

						  // confirm
						  if( WinMessageBox(  HWND_DESKTOP,
													 hwnd,
													 "This action requires a reboot to load/unload CandyBar.dll"
														  "\nContinue?",
													 "Excuse me, but...",
													 0UL,
													 MB_OKCANCEL | MB_ICONASTERISK ) == MBID_CANCEL )
						  {
								WinCheckButton( hwnd, CHID_ENABLECB, !bEnabled );
								return( ( MRESULT )0 );
						  }

						  // update profile
						  if( !PrfQueryProfileString( HINI_USERPROFILE,
																"CandyBarZ",
																"Profile",
																NULL,
																szProfile,
																sizeof( szProfile ) ) )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ActiveColorDlgProc",
													 "Failed to query CandyBarZ profile location",
													 0UL );
								WinCheckButton( hwnd, CHID_ENABLECB, !bEnabled );
								return( ( MRESULT )0 );
						  }
						  if( ( hIni = PrfOpenProfile( hab, szProfile ) ) == NULLHANDLE )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ActiveColorDlgProc",
													 "Failed to open profile",
													 0UL );
								WinCheckButton( hwnd, CHID_ENABLECB, !bEnabled );
								return( ( MRESULT )0 );
						  }
						  if( !PrfWriteProfileData(	hIni,
																"UserOptions",
																"Enabled",
																&bEnabled,
																sizeof( bEnabled ) ) )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ActiveColorDlgProc",
													 "Failed to write profile data",
													 0UL );
								WinCheckButton( hwnd, CHID_ENABLECB, !bEnabled );
								PrfCloseProfile( hIni );
								return( ( MRESULT )0 );
						  }
						  PrfCloseProfile( hIni );
					 }
					 break;

					 case CHID_ENABLES3: // S3 workaround
					 {
						  bEnabled = WinQueryButtonCheckstate( hwnd, CHID_ENABLES3 );

						  // update profile
						  if( !PrfQueryProfileString( HINI_USERPROFILE,
																"CandyBarZ",
																"Profile",
																NULL,
																szProfile,
																sizeof( szProfile ) ) )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ActiveColorDlgProc",
													 "Failed to query CandyBarZ profile location",
													 0UL );
								WinCheckButton( hwnd, CHID_ENABLES3, !bEnabled );
								return( ( MRESULT )0 );
						  }
						  if( ( hIni = PrfOpenProfile( hab, szProfile ) ) == NULLHANDLE )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ActiveColorDlgProc",
													 "Failed to open profile",
													 0UL );
								WinCheckButton( hwnd, CHID_ENABLES3, !bEnabled );
								return( ( MRESULT )0 );
						  }
						  if( !PrfWriteProfileData(	hIni,
																"UserOptions",
																"S3Fix",
																&bEnabled,
																sizeof( bEnabled ) ) )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ActiveColorDlgProc",
													 "Failed to write profile data",
													 0UL );
								WinCheckButton( hwnd, CHID_ENABLES3, !bEnabled );
								PrfCloseProfile( hIni );
								return( ( MRESULT )0 );
						  }
						  PrfCloseProfile( hIni );

						  // write change to shared mem
						  if( DosGetNamedSharedMem( ( PPVOID )&ptbShare, PSTB_SHARE, PAG_READ | PAG_WRITE )
								!= NO_ERROR )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ActiveColorDlgProc",
													 "Failed to retrieve CandyBarZ shared memory",
													 0UL );
								break;
						  }
//                    ptbShare->bS3Fix = bEnabled;
						  DosFreeMem( ptbShare );
					 }
					 break;

					 case CHID_PMVIEW:
					 {
						  bEnabled = WinQueryButtonCheckstate( hwnd, CHID_PMVIEW );

						  // update profile
						  if( !PrfQueryProfileString( HINI_USERPROFILE,
																"CandyBarZ",
																"Profile",
																NULL,
																szProfile,
																sizeof( szProfile ) ) )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ColorDlgProc",
													 "Failed to query CandyBarZ profile location",
													 0UL );
								WinCheckButton( hwnd, CHID_PMVIEW, !bEnabled );
								return( ( MRESULT )0 );
						  }
						  if( ( hIni = PrfOpenProfile( hab, szProfile ) ) == NULLHANDLE )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ColorDlgProc",
													 "Failed to open profile",
													 0UL );
								WinCheckButton( hwnd, CHID_PMVIEW, !bEnabled );
								return( ( MRESULT )0 );
						  }
						  if( !PrfWriteProfileData(	hIni,
																"UserOptions",
																"PMVIEW",
																&bEnabled,
																sizeof( bEnabled ) ) )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ColorDlgProc",
													 "Failed to write profile data",
													 0UL );
								WinCheckButton( hwnd, CHID_PMVIEW, !bEnabled );
								PrfCloseProfile( hIni );
								return( ( MRESULT )0 );
						  }
						  PrfCloseProfile( hIni );
						  if( ( Atom = WinFindAtom( hAtomTbl, PSTB_COLORCHANGE_ATOM ) ) == 0 )
						  {
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ColorDlgProc",
													 "Failed to locate COLORCHANGE atom",
													 0UL );
								break;
						  }
						  // broadcast to desktop children
						  if( !WinBroadcastMsg(   HWND_DESKTOP,
														  Atom,
														  ( MPARAM )0,
														  ( MPARAM )0,
														  BMSG_POST | BMSG_DESCENDANTS ) )
						  {
								char szError[ 256 ];

								sprintf(    szError,
												"Failed to broadcast atom:  error = %x",
												WinGetLastError( hab ) );
								PSUTErrorFunc(  hwnd,
													 "Error",
													 "ColorDlgProc",
													 szError,
													 0UL );
						  }
						  // broadcast to hidden windows
						  WinBroadcastMsg(	 HWND_OBJECT,
													 Atom,
													 ( MPARAM )0,
													 ( MPARAM )0,
													 BMSG_POST | BMSG_DESCENDANTS );
					 }
					 break;
				}

				return( ( MRESULT )0 );
		  }
		  break;
	 }

	return ( WinDefDlgProc(hwnd,msg,mp1,mp2) );
}
