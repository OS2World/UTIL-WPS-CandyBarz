
/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#include "setup.h"

// proc for custom setup dialog
MRESULT EXPENTRY CustomizeDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CBZSHARE    *pCBZShare;
    char        szProfile[CCHMAXPATH];
    HWND        hwndControl;
    HWND        hwndFocus;
    HAB         hab = WinQueryAnchorBlock(hwnd);
    HINI        hIni;
    ULONG       ulRc;
    HATOMTBL    hAtomTbl = WinQuerySystemAtomTable();
    ATOM        Atom;
    short       sControlId;
    short       sHelpId;
    int         index;
    char        szError[CCHMAXPATH];
    char        pszFile[CCHMAXPATH];
    static      HWND hwndPreview;
    TCUST       *ptc;
    char        szString[256];

    switch (msg)
    {
        case WM_INITDLG:
        {
            //Load the control strings from the string table
            SetControlText(hab, hwnd, IDS_CUSTOM_TITLE);
            SetControlText(hab, WinWindowFromID(hwnd, GBID_AVAILABLETHEMES), IDS_AVAILABLETHEMES_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_SELECTTHEME), IDS_SELECT_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_PREVIEWTHEME), IDS_PREVIEW_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, GBID_THEMEPROPERTIES), IDS_SELECTEDTHEME_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, STID_THEMENAMELABEL), IDS_THEMENAME_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, STID_THEMEAUTHORLABEL), IDS_THEMEAUTHOR_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, STID_THEMEVERSIONLABEL), IDS_THEMEVERSION_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, STID_THEMEDATELABEL), IDS_THEMEDATE_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, STID_THEMEFILELABEL), IDS_THEMEFILE_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, STID_THEMENAME), IDS_NONESPECIFIED_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, STID_THEMEAUTHOR), IDS_NONESPECIFIED_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, STID_THEMEVERSION), IDS_NONESPECIFIED_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, STID_THEMEDATE), IDS_NONESPECIFIED_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, STID_THEMEFILE), IDS_NONESPECIFIED_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, GBID_CURRENTTHEME), IDS_CURRENTTHEME_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, EFID_CURRENTTHEME), IDS_NONESPECIFIED_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_UNDO), IDS_UNDO_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_CUSTSAVE), IDS_SAVE_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_CUSTCANCEL), IDS_CANCEL_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, CHID_ENABLECB), IDS_ENABLECBZ_CHECKBOX);
            SetControlText(hab, WinWindowFromID(hwnd, CHID_ENABLECUST), IDS_ENABLECUST_CHECKBOX);
            SetControlText(hab, WinWindowFromID(hwnd, STID_CUSTTITLE), IDS_CUSTTITLE_LABEL);

            // get shared mem
            if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ) != NO_ERROR)
            {
                PSUTErrorFunc(hwnd, "Error", "ColorDlgProc", "Unable to get Shared Memory", 0UL);
                WinPostMsg(NULLHANDLE, WM_QUIT, (MPARAM) 0, (MPARAM) 0);
                return ((MRESULT) FALSE);
            }

            hwndControl = WinWindowFromID(hwnd, EFID_CURRENTTHEME);
            WinSendMsg(hwndControl, EM_SETTEXTLIMIT, (MPARAM) 128, (MPARAM) 0);
            SetCurrentThemeText(hwndControl, pCBZShare->szThemeFilename);

            LoadPlugins(hwnd, WinWindowFromID(hwnd, LBID_AVAILABLETHEMES), WinWindowFromID(hwnd, LBID_AVAILABLETHEMEFILES), TRUE);

            WinEnableWindow(WinWindowFromID(hwnd, PBID_UNDO), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, PBID_APPLY), FALSE);
            WinEnableWindow(WinWindowFromID(hwnd, PBID_SAVE), FALSE);

            hwndPreview = NULLHANDLE;
           // everything enabled by default
           WinCheckButton( hwnd, CHID_ENABLECB, TRUE );
           WinCheckButton( hwnd, CHID_ENABLECUST, TRUE );

           // just a pointer to the title string so it can be updated in listbox
           if( ( ptc = ( TCUST * )mp2 ) == NULL )
           {
                PSUTErrorFunc( hwnd, "Error", "CustomizeDlgProc", "Missing parameter", 0UL );
                WinDismissDlg( hwnd, DID_ERROR );
                return( MRFROMLONG( FALSE ) );
           }
           if (ptc->bCloning != TRUE)
                WinSetWindowText(WinWindowFromID(hwnd, EFID_CUSTTITLE), ptc->szTitle);

           WinSetWindowPtr( hwnd, QWL_USER, ptc );   // set window words



                    // open profile
                    if (!PrfQueryProfileString(HINI_USERPROFILE,
                                               "CandyBarZ",
                                               "Profile",
                                               NULL,
                                               szProfile,
                                               sizeof(szProfile)))
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "CustomizeDlgProc",
                                      "Failed to query CandyBarZ profile information from OS/2 User "
                                      "INI file",
                                      0UL);
                        break;
                    }

                    if ((hIni = PrfOpenProfile(hab, szProfile)) == NULLHANDLE)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "CustomizeDlgProc",
                                      "Failed to open CandyBarZ profile",
                                      0UL);
                        break;
                    }

                    if ( PrfQueryProfileString(hIni, "CustomOptions", ptc->szTitle, NULL, pszFile, CCHMAXPATH - 1)  > 0 )
                        SetCurrentThemeText(WinWindowFromID(hwnd, EFID_CURRENTTHEME), pszFile);

                    PrfCloseProfile(hIni);
        }
        break;

        case WM_COMMAND:
        {
            sControlId = COMMANDMSG(&msg)->cmd;

            switch (sControlId)
            {
                case PBID_PREVIEWTHEME:
                {
                    //find out which item is selected.
                    hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLETHEMES);
                    index = (long) WinSendMsg(hwndControl, LM_QUERYSELECTION,
                                              MPFROMSHORT((SHORT) LIT_FIRST),
                                              MPFROMLONG(0));

                    if (index == LIT_NONE)  //make sure something is selected!

                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "ColorDlgProc",
                                      "No Theme Selected",
                                      0UL);
                        break;
                    }

                    //Get the selected plugin DLL name from the AvailablePluginDLLS listbox
                    hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLETHEMEFILES);
                    WinSendMsg(hwndControl, LM_QUERYITEMTEXT,
                               MPFROM2SHORT((SHORT) index, CCHMAXPATH),
                               MPFROMP((PSZ) pszFile));

                    if ((hwndPreview == NULLHANDLE) || (!WinIsWindow(hab, hwndPreview)))
                    {
                        if ((hwndPreview = WinLoadDlg(HWND_DESKTOP,
                                                       hwnd,
                                                       PreviewDlgProc,
                                                       NULLHANDLE,
                                                       DLGID_PREVIEW,
                                                       NULL)) == NULLHANDLE)
                        {
                            PSUTErrorFunc(hwnd,
                                          "Error",
                                          "ColorDlgProc",
                                          "Couldn't load preview dialog",
                                          0UL);

                            break;
                        }

                        WinSendMsg(hwndPreview, CBZ_UPDATEPREVIEW, MPFROMP(pszFile), 0);
                    }
                    else
                    {
                        WinSendMsg(hwndPreview, CBZ_UPDATEPREVIEW, MPFROMP(pszFile), 0);
                        WinSetFocus(HWND_DESKTOP, hwndPreview);
                    }

                    WinShowWindow(hwndPreview, FALSE);
                    WinShowWindow(hwndPreview, TRUE);
                }
                break;

                case PBID_SELECTTHEME:
                {
                    //find out which item is selected.
                    hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLETHEMES);
                    index = (long) WinSendMsg(hwndControl, LM_QUERYSELECTION,
                                              MPFROMSHORT((SHORT) LIT_FIRST),
                                              MPFROMLONG(0));

                    if (index == LIT_NONE)  //make sure something is selected!

                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "ColorDlgProc",
                                      "No Theme Selected",
                                      0UL);
                        break;
                    }

                    //Get the selected plugin DLL name from the AvailablePluginDLLS listbox
                    hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLETHEMEFILES);
                    WinSendMsg(hwndControl, LM_QUERYITEMTEXT,
                               MPFROM2SHORT((SHORT) index, CCHMAXPATH),
                               MPFROMP((PSZ) pszFile));
                    SetCurrentThemeText(WinWindowFromID(hwnd, EFID_CURRENTTHEME), pszFile);

                    WinEnableWindow(WinWindowFromID(hwnd, PBID_APPLY), TRUE);
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_SAVE), TRUE);
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_UNDO), TRUE);

                }
                break;

                // undo restores original settings (since last apply or save)
                case PBID_UNDO:
                {
                    // get shared mem
                    if (DosGetNamedSharedMem((PPVOID) & pCBZShare, CBZ_SHARE, PAG_READ) != NO_ERROR)
                    {
                        PSUTErrorFunc(hwnd, "Error", "ColorDlgProc",
                                      "Unable to get Shared Memory", 0UL);
                        break;
                    }

                    hwndControl = WinWindowFromID(hwnd, EFID_CURRENTTHEME);
                    WinSendMsg(hwndControl, EM_SETTEXTLIMIT, (MPARAM) 128, (MPARAM) 0);
                    SetCurrentThemeText(hwndControl, pCBZShare->szThemeFilename);

                    WinEnableWindow(WinWindowFromID(hwnd, PBID_APPLY), FALSE);
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_SAVE), FALSE);
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_UNDO), FALSE);

                }
                break;

                case PBID_APPLY:
                {
                    //find out which item is selected.
                    hwndControl = WinWindowFromID(hwnd, EFID_CURRENTTHEME);
                    WinQueryWindowText(hwndControl, sizeof(pszFile), pszFile);
                    strcpy(pszFile, strrchr(pszFile, '(') + 1);
                    pszFile[strlen(pszFile) - 1] = '\0';
                    ApplyTheme(pszFile);

                    // disable undo, apply (save is still valid)
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_APPLY), FALSE);
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_UNDO), FALSE);
                }
                break;

                case PBID_CUSTCANCEL:
                {
                    WinDismissDlg( hwnd, PBID_CUSTCANCEL );
                }
                break;

                //saves to candybar.ini
                case PBID_CUSTSAVE:
                {
                    char szCustomTitle[MAX_CUSTOM_TITLE_SIZE];
                    ptc = WinQueryWindowPtr( hwnd, QWL_USER );   // get window words


                    // atom to broadcast changes
                    if( ( Atom = WinFindAtom( hAtomTbl, PSTB_COLORCHANGE_ATOM ) ) == 0 )
                    {
                        PSUTErrorFunc(  hwnd,
                                        "Error",
                                        "CustomizeDlgProc",
                                        "Failed to locate COLORCHANGE atom in system atom table",
                                        0UL );
                                        WinDismissDlg( hwnd, DID_ERROR );
                                        break;
                    }

/*
                    // if the title's been changed
                    if( *( pDlgData->szTitleOrig ) )
                    {
                        if( strcmp( pDlgData->szTitleOrig, pDlgData->pszTitle ) )
                        {
                            // delete info stored under old title
                            cbData = sizeof( customPluginSet );
                            if( PrfQueryProfileData(    hIni,
                                                        "CustomOptions",
                                                        pDlgData->szTitleOrig,
                                                        &( customPluginSet ),
                                                        &cbData ) )
                            {
                                PrfWriteProfileData( hIni, "CustomOptions", pDlgData->szTitleOrig, NULL, 0 );
                            }
                        }
                    }

*/


                    // get title string (a) to write to profile (b) to return to notebook
                    // for listbox update
                    hwndControl = WinWindowFromID( hwnd, EFID_CUSTTITLE );
                    WinQueryWindowText( hwndControl, MAX_CUSTOM_TITLE_SIZE, szCustomTitle );
                    if( !*( szCustomTitle ) )
                    {
                        WinMessageBox(  HWND_DESKTOP,
                                        hwnd,
                                        "Please enter a title for this custom scheme",
                                        "Excuse me, but...",
                                        0,
                                        MB_OK | MB_INFORMATION );
                        break;
                    }


                    // open profile
                    if (!PrfQueryProfileString(HINI_USERPROFILE,
                                               "CandyBarZ",
                                               "Profile",
                                               NULL,
                                               szProfile,
                                               sizeof(szProfile)))
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "CustomizeDlgProc",
                                      "Failed to query CandyBarZ profile information from OS/2 User "
                                      "INI file",
                                      0UL);
                        break;
                    }

                    if ((hIni = PrfOpenProfile(hab, szProfile)) == NULLHANDLE)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "CustomizeDlgProc",
                                      "Failed to open CandyBarZ profile",
                                      0UL);
                        break;
                    }

                    //find out which item is selected.
                    hwndControl = WinWindowFromID(hwnd, EFID_CURRENTTHEME);
                    WinQueryWindowText(hwndControl, sizeof(pszFile), pszFile);
                    strcpy(pszFile, strrchr(pszFile, '(') + 1);
                    pszFile[strlen(pszFile) - 1] = '\0';

                    // write shared mem
                    if (!PrfWriteProfileString(hIni,
                                            "CustomOptions",
                                            szCustomTitle,
                                            pszFile ) )
                    {
                        PSUTErrorFunc(hwnd,
                                    "Error",
                                    "CustomizeDlgProc",
                                    "Failed to write Custom data",
                                    0UL);
                        return FALSE;
                    }

                    SaveSettings(hwnd, hIni,  szCustomTitle, pszFile);

                    PrfCloseProfile(hIni);

                    // disable undo, apply, save
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_APPLY), FALSE);
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_SAVE), FALSE);
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_UNDO), FALSE);

                    // broadcast changes
                    WinBroadcastMsg(    HWND_DESKTOP,
                                        Atom,
                                        ( MPARAM )0,
                                        ( MPARAM )0,
                                        BMSG_POST | BMSG_DESCENDANTS );

                    WinBroadcastMsg(    HWND_OBJECT,
                                        Atom,
                                        ( MPARAM )0,
                                        ( MPARAM )0,
                                        BMSG_POST | BMSG_DESCENDANTS );

                    strcpy(ptc->szTitle, szCustomTitle);
                    WinDismissDlg( hwnd, PBID_CUSTSAVE );
                }
                break;
            }
            return ((MPARAM) 0);
        }
        break;

        case WM_CONTROL:
        {
            switch (SHORT1FROMMP(mp1))
            {
                //update the Selected Plugin Properties info
                case LBID_AVAILABLETHEMES:
                {
                    if (SHORT2FROMMP(mp1) == LN_SELECT)
                    {
                        //get the index of the selected plugin
                        hwndControl = WinWindowFromID(hwnd, LBID_AVAILABLETHEMES);
                        index = (long) WinSendMsg(hwndControl, LM_QUERYSELECTION,
                                             MPFROMSHORT((SHORT) LIT_FIRST),
                                                  MPFROMLONG(0));
                        if (index != LIT_NONE)
                        {
                            WinSendMsg(WinWindowFromID(hwnd, LBID_AVAILABLETHEMEFILES), LM_QUERYITEMTEXT,
                                       MPFROM2SHORT(index, sizeof(szError)),
                                       MPFROMP(szError));

                            UpdateControls(hwnd, szError);
                        }
                    }
                }
                break;
            }
            return ((MRESULT) 0);
        }
        break;
/*
       case WM_HELP:
   {
   HWND hwndHelp = WinQueryHelpInstance( hwnd );

   if( ( cbclr = ( CBCLR * )WinQueryWindowPtr( hwnd, QWL_USER ) ) == NULL )
   {
   PSUTErrorFunc(  hwnd,
   "Error",
   "ActiveColorDlgProc",
   "Failed to query window words",
   0UL );
   break;
   }

   if( hwndHelp )
   {
   // get id of control with focus
   hwndFocus = WinQueryFocus( HWND_DESKTOP );
   sControlId = WinQueryWindowUShort( hwndFocus, QWS_ID );

   switch( sControlId )
   {
       case PBID_CHANGETOP:
       case PBID_CHANGEBOTTOM:
   {
   sHelpId = HLPIDX_TOPBOTTOM;
   }
   break;

       case PBID_FILLDOWN:
       case PBID_FILLUP:
   {
   sHelpId = HLPIDX_FILLBUTTONS;
   }
   break;

       case CHID_HORZ:
   {
   sHelpId = HLPIDX_HORIZONTAL;
   }
   break;

       case CHID_BORDER:
   {
   sHelpId = HLPIDX_DRAWBORDER;
   }
   break;

       case PBID_APPLY:
       case PBID_SAVE:
   {
   sHelpId = HLPIDX_APPLYSAVE;
   }
   break;

       case PBID_UNDO:
   {
   sHelpId = HLPIDX_UNDODEF;
   }
   break;

       case CHID_ENABLECB:
   {
   sHelpId = HLPIDX_ENABLEDEF;
   }
   break;

       case CHID_USEFILE:
       case PBID_CHANGEFILE:
   {
   sHelpId = HLPIDX_FILES;
   }
   break;

       case CHID_3DTEXT:
       case PBID_SHADOWCOLOR:
   {
   sHelpId = HLPIDX_3D;
   }
   break;

       case CHID_ENABLES3:
   {
   sHelpId = HLPIDX_S3;
   }
   break;

       case CHID_PMVIEW:
   {
   sHelpId = HLPIDX_PMVIEW;
   }
   break;

   default:
   {
   sHelpId = HLPIDX_PAGE1;
   }
   break;
   }

   // display help
   ulRc = ( ULONG )WinSendMsg( hwndHelp,
   HM_DISPLAY_HELP,
   MPFROM2SHORT( sHelpId, NULL),
   MPFROMSHORT( HM_RESOURCEID ) );
   if( ulRc )
   {
   printf( "help error\n" );
   }
   }

   return( ( MPARAM )0 );
   }
   break;

       case WM_DESTROY:
   {
   //free up the memory allocated for the window words
   if( ( pDlgData = ( DLGDATA * )WinQueryWindowPtr( hwnd, QWL_USER ) ) == NULL )
   {
   PSUTErrorFunc(  hwnd,
   "Error",
   "ColorDlgProc",
   "Failed to query window words",
   0UL );
   break;
   }
   pTBData = pDlgData->pTBData;

   //free resources in the pTBData structure.
   for ( i = 0; i < pTBData->cbActivePlugins; i++ )
   {
   if ( pTBData->Plugins[ACTIVE][i].hModDll != NULLHANDLE )
   DosFreeModule( pTBData->Plugins[ACTIVE][i].hModDll );
   if ( pTBData->Plugins[ACTIVE][i].pData != NULL )
   DosFreeMem( pTBData->Plugins[ACTIVE][i].pData );
   }
   for ( i = 0; i < pTBData->cbInactivePlugins; i++ )
   {
   if ( pTBData->Plugins[INACTIVE][i].hModDll != NULLHANDLE )
   DosFreeModule( pTBData->Plugins[INACTIVE][i].hModDll );
   if ( pTBData->Plugins[INACTIVE][i].pData != NULL )
   DosFreeMem( pTBData->Plugins[INACTIVE][i].pData );
   }
   DosFreeMem( pTBData );

   DosFreeMem( pDlgData );
   WinSetWindowPtr( hwnd, QWL_USER, NULL );
   }
   break;
 */
    }
    return (WinDefDlgProc(hwnd, msg, mp1, mp2));


}

/*************************************************************************************************************

    $Id: CustomizeDlgProc.c,v 1.3 2000/03/26 06:51:21 enorman Exp $

    $Log: CustomizeDlgProc.c,v $
    Revision 1.3  2000/03/26 06:51:21  enorman
    resyncing with my local tree.

    Revision 1.2  1999/12/29 19:38:09  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:47:57  ktk
    Import

    Revision 1.15  1998/10/08 01:22:09  enorman
    I've fixed a problem with the custom colors dialog.
    When selecting the "Use File" checkbox, After loading the file,
    the checkbox was not getting checked.

    Revision 1.14  1998/09/26 18:02:34  mwagner
    Fixed bug that was causing crash when compiled with the O+ flag

    Revision 1.13  1998/09/26 07:05:35  enorman
    Added code for custom color cloning

    Revision 1.12  1998/09/23 21:13:58  enorman
    It seemed that some of the controls were not getting updated, so I fixed
    the problems I found.

    Revision 1.11  1998/09/22 12:12:22  mwagner
    Fixed some minor bugs

    Revision 1.8  1998/09/12 10:07:12  pgarner
    Added copyright notice

    Revision 1.7  1998/09/12 07:35:46  mwagner
    Added support for bitmap images

    Revision 1.4  1998/08/31 19:35:45  enorman
    Finished the implementation of 3D Text

    Revision 1.3  1998/08/10 02:58:14  enorman
    Added alot of code to support 3D text

    Revision 1.2  1998/06/14 11:37:29  mwagner
    Added a call to 'PrfCloseProfile( hIni )' during processing of the WM_INITDLG message

    Revision 1.1  1998/06/08 14:18:57  pgarner
    Initial Checkin

*************************************************************************************************************/
