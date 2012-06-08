/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

	C.W. 22.09.98

******************************************************************************/


#include "setup.h"

/* Button bitmaps for settings pages */
extern HBITMAP hbmButtonNormal;
extern HBITMAP hbmButtonNormalMsk;
extern HBITMAP hbmButtonHilite;
extern HBITMAP hbmButtonHiliteMsk;
extern HBITMAP hbmButtonNormal2;
extern HBITMAP hbmButtonNormalMsk2;
extern HBITMAP hbmButtonHilite2;
extern HBITMAP hbmButtonHiliteMsk2;
extern ULONG ulCornerCB2;


static PFNWP oldContainerProc;

/* For DnD of bitmap-files on container windows */
/* DnD not implemented yet */
MRESULT EXPENTRY ContainerProc2( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
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
				case COID_BUTTONNORMAL2:
					/* Draw bitmap centered */ 
					if(!hbmButtonNormal2)break;/* Catch error */
					GpiQueryBitmapParameters(hbmButtonNormal2,&bihBitmap);
					ptl.x=(rcl.xRight-rcl.xLeft-bihBitmap.cx)/2;
					ptl.y=(rcl.yTop-rcl.yBottom-bihBitmap.cy)/2;
					WinDrawBitmap(hps,hbmButtonNormal2,NULL,&ptl,0,0,DBM_NORMAL);
					break;
				case COID_BUTTONNORMALMSK2:
					if(!hbmButtonNormalMsk2)break;/* Catch error */
					GpiQueryBitmapParameters(hbmButtonNormalMsk2,&bihBitmap);
					ptl.x=(rcl.xRight-rcl.xLeft-bihBitmap.cx)/2;
					ptl.y=(rcl.yTop-rcl.yBottom-bihBitmap.cy)/2;
					WinDrawBitmap(hps,hbmButtonNormalMsk2,NULL,&ptl,0,0,DBM_NORMAL);
					break;
				case COID_BUTTONHILITE2:
					/* Draw bitmap centered */ 
					if(!hbmButtonHilite2)break;/* Catch error */
					GpiQueryBitmapParameters(hbmButtonHilite2,&bihBitmap);
					ptl.x=(rcl.xRight-rcl.xLeft-bihBitmap.cx)/2;
					ptl.y=(rcl.yTop-rcl.yBottom-bihBitmap.cy)/2;
					WinDrawBitmap(hps,hbmButtonHilite2,NULL,&ptl,0,0,DBM_NORMAL);
					break;
				case COID_BUTTONHILITEMSK2:
					if(!hbmButtonHiliteMsk2)break;/* Catch error */
					GpiQueryBitmapParameters(hbmButtonHiliteMsk2,&bihBitmap);
					ptl.x=(rcl.xRight-rcl.xLeft-bihBitmap.cx)/2;
					ptl.y=(rcl.yTop-rcl.yBottom-bihBitmap.cy)/2;
					WinDrawBitmap(hps,hbmButtonHiliteMsk2,NULL,&ptl,0,0,DBM_NORMAL);
					break;
				}
			WinEndPaint(hps);
			return 0;
		default:
			break;
		}
	return( oldContainerProc( hwnd, msg, mp1, mp2 ) );	
}


/* Dialog procedure for button decoration page */
MRESULT EXPENTRY ButtonDecorDlgProc2( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
	HWND            hwndFocus;
	ULONG           ulRc;
	char            szError[ 1024 ];
	static BOOL            bButtonDecor;
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
						PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc2", "Failed to query profile location", 0UL );
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
				if( !PrfQueryProfileData( hIni, "ButtonDecor", "btCorner2", &ulCornerCB2, &cbData ) )
					ulCornerCB2=CORNER_BOTTOMRIGHT;/* default corner */
				
				/* Check button */
				WinCheckButton( hwnd, ulCornerCB2, TRUE );

				/* disable UNDO button */
				WinEnableWindow(WinWindowFromID( hwnd, PBID_PICDIRECTORYUNDO ),FALSE);

				/* Subclass the container windows. We need this later for DnD */
				oldContainerProc=WinSubclassWindow(WinWindowFromID(hwnd,COID_BUTTONNORMAL2),ContainerProc2);
				WinSubclassWindow(WinWindowFromID(hwnd,COID_BUTTONNORMALMSK2),ContainerProc2);
				WinSubclassWindow(WinWindowFromID(hwnd,COID_BUTTONHILITE2),ContainerProc2);
				WinSubclassWindow(WinWindowFromID(hwnd,COID_BUTTONHILITEMSK2),ContainerProc2);
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
						/* Query bitmap position */
						cbData=sizeof(ulCornerCB2);
						if( !PrfQueryProfileData( hIni, "ButtonDecor", "btCorner2", &ulCornerCB2, &cbData ) ) {
							PSUTErrorFunc( hwnd, "Error", "ButtonDecorDlgProc", "can't find previous position in profile!", 0UL );
							break;
						}
						/* check corner button */
						WinCheckButton( hwnd, ulCornerCB2, TRUE );
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
							ulCornerCB2=CORNER_TOPRIGHT;
						}
						break;
					case ARID_BUTTONTOPLEFT:
						{
							/* enable UNDO button */
							WinEnableWindow(WinWindowFromID( hwnd, PBID_PICDIRECTORYUNDO ),TRUE);
							ulCornerCB2=CORNER_TOPLEFT;
						}
						break;						
					case ARID_BUTTONBOTTOMRIGHT:
						{
							/* enable UNDO button */
							WinEnableWindow(WinWindowFromID( hwnd, PBID_PICDIRECTORYUNDO ),TRUE);
							ulCornerCB2=CORNER_BOTTOMRIGHT;
						}
						break;
					case ARID_BUTTONBOTTOMLEFT:
						{
							/* enable UNDO button */
							WinEnableWindow(WinWindowFromID( hwnd, PBID_PICDIRECTORYUNDO ),TRUE);
							ulCornerCB2=CORNER_BOTTOMLEFT;
						}
						break;						
					}
			}
			break;
		case WM_DESTROY:
			{
				if(hIni) {
					PrfCloseProfile( hIni );
				}
			}
			break;
		case WM_HELP:
			{
				/*	HWND    hwndHelp = WinQueryHelpInstance( hwnd );
				
				if( hwndHelp )
					{
						hwndFocus = WinQueryFocus( HWND_DESKTOP );
						sControlId = WinQueryWindowUShort( hwndFocus, QWS_ID );
						switch( sControlId )
							{
							case PBID_ACTIVEMENU:
							case PBID_INACTIVEMENU:
								{
									sHelpId = HLPIDX_CUSTMENU;
								}
								break;
								
							case EFID_CUSTTITLE:
								{
									sHelpId = HLPIDX_CUSTTITLE;
								}
								break;
								
							case PBID_CHANGEACTIVETOP:
							case PBID_CHANGEACTIVEBOTTOM:
							case PBID_CHANGEINACTIVETOP:
							case PBID_CHANGEINACTIVEBOTTOM:
								{
									sHelpId = HLPIDX_TOPBOTTOM;
								}
								break;
								
							case PBID_FILLDOWNACTIVE:
							case PBID_FILLUPACTIVE:
							case PBID_FILLDOWNINACTIVE:
							case PBID_FILLUPINACTIVE:
								{
									sHelpId = HLPIDX_FILLBUTTONS;
								}
								break;
								
							case CHID_ACTIVEHORZ:
							case CHID_INACTIVEHORZ:
								{
									sHelpId = HLPIDX_HORIZONTAL;
								}
								break;
								
							case CHID_ACTIVEBORDER:
							case CHID_INACTIVEBORDER:
								{
									sHelpId = HLPIDX_DRAWBORDER;
								}
								break;
								
							case PBID_UNDOACTIVE:
							case PBID_UNDOINACTIVE:
								{
									sHelpId = HLPIDX_CUSTUNDO;
								}
								break;
								
							case CHID_ENABLECB:
							case CHID_ENABLECUST:
								{
									sHelpId = HLPIDX_CHKBOXES;
								}
								break;
								
							case PBID_CUSTSAVE:
							case PBID_CUSTCANCEL:
								{
									sHelpId = HLPIDX_SAVECANCEL;
								}
								break;
								
							default:
								{
									sHelpId = HLPIDX_CUSTOMIZE;
								}
								break;
							}
						
						ulRc = ( ULONG )WinSendMsg( hwndHelp,
																				HM_DISPLAY_HELP,
																				MPFROM2SHORT( sHelpId, NULL),
																				MPFROMSHORT( HM_RESOURCEID ) );
						if( ulRc )
							{
								printf( "help error\n" );
							}
					}*/
				
				return( ( MPARAM )0 );
			}
			break;
			
			
    }
	return( WinDefDlgProc( hwnd, msg, mp1, mp2 ) );
}



/*************************************************************************************************************


    $Id: buttondecordlgproc2.c,v 1.1 1999/06/15 20:47:58 ktk Exp $


    $Log: buttondecordlgproc2.c,v $
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

