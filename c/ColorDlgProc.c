
/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/
#include "setup.h"

// dialog proc for default colors pages of notebook
MRESULT EXPENTRY ColorDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CBZSHARE    *pCBZShare;
    char        szProfile[CCHMAXPATH];
    HWND        hwndControl;
    HAB         hab = WinQueryAnchorBlock(hwnd);
    HINI        hIni;
    ULONG       ulRc;
    short       sControlId;
    short       sHelpId;
    int         index;
    char        szError[CCHMAXPATH];
    char        pszFile[CCHMAXPATH];
    static HWND hwndPreview;
    char        szString[256];

    switch (msg)
    {
        case WM_INITDLG:
        {
            //Load the control strings from the string table
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
            SetControlText(hab, WinWindowFromID(hwnd, PBID_APPLY), IDS_APPLY_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_SAVE), IDS_SAVE_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_UNDO), IDS_UNDO_BUTTON);

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

                //saves to candybar.ini
                case PBID_SAVE:
                {
                    // If we haven't already applied, apply
                    if (WinIsWindowEnabled(WinWindowFromID(hwnd, PBID_APPLY)))
                    {
                        WinSendMsg(hwnd,
                                   WM_COMMAND,
                                   MPFROMSHORT(PBID_APPLY),
                                   MPFROM2SHORT(CMDSRC_OTHER, FALSE));
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
                                      "ColorDlgProc",
                                      "Failed to query CandyBarZ profile information from OS/2 User "
                                      "INI file",
                                      0UL);
                        break;
                    }

                    if ((hIni = PrfOpenProfile(hab, szProfile)) == NULLHANDLE)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "ColorDlgProc",
                                      "Failed to open CandyBarZ profile",
                                      0UL);
                        break;
                    }

                    SaveSettings(hwnd, hIni,  "", "");

                    PrfCloseProfile(hIni);

                    // disable undo, apply, save
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_APPLY), FALSE);
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_SAVE), FALSE);
                    WinEnableWindow(WinWindowFromID(hwnd, PBID_UNDO), FALSE);
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

    $Id: ColorDlgProc.c,v 1.3 2000/03/26 06:51:21 enorman Exp $

    $Log: ColorDlgProc.c,v $
    Revision 1.3  2000/03/26 06:51:21  enorman
    resyncing with my local tree.

    Revision 1.2  1999/12/29 19:38:09  enorman
    Resync'ed with my local tree

    Revision 1.1  1999/06/15 20:47:57  ktk
    Import

    Revision 1.14  1998/09/30 05:08:34  mwagner
    Made the PMView fix optional

    Revision 1.13  1998/09/22 12:12:45  mwagner
     Fixed some minor bugs

    Revision 1.10  1998/09/15 10:10:03  enorman
    Split the dlgProc into two procedures. One for the active
    page and one for the inactive page.
    Changed many references to menu related things to references to the new controls

    Revision 1.9  1998/09/12 10:06:16  pgarner
    Added copyright notice

    Revision 1.8  1998/09/12 07:35:46  mwagner
    Added support for bitmap images

    Revision 1.5  1998/08/31 19:35:41  enorman
    Finished the implementation of 3D Text

    Revision 1.4  1998/08/10 03:00:01  enorman
    Added alot of code to support 3D text

    Revision 1.3  1998/06/29 15:32:10  mwagner
    Matts S3 and PMView fixes

    Revision 1.2  1998/06/14 11:33:20  mwagner
    Added a call to 'PrfCloseProfile( hIni )' at the end of processing the
    WM_INITDLG message.

    Revision 1.1  1998/06/08 14:18:56  pgarner
    Initial Checkin

*************************************************************************************************************/
