/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#include "setup.h"

int main(int argc, char *argv[])
{
    HAB         hab;
    HMQ         hmq;

    hab = WinInitialize(0UL);
    hmq = WinCreateMsgQueue(hab, 100UL);

    // main setup dialog
    if (WinDlgBox(HWND_DESKTOP, NULLHANDLE, SetupDlgProc, NULLHANDLE, DLGID_SETUP, 0) == DID_ERROR)
    {
        WinDestroyMsgQueue(hmq);
        WinTerminate(hab);
        return (1);
    }

    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);

    return (0);
}
// main dialog proc
MRESULT EXPENTRY SetupDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HATOMTBL    hAtomTbl = WinQuerySystemAtomTable();
    ATOM        Atom;
    HWND        hwndHelp;
    HWND        hwndNoteBook;
    ULONG       ulPageID;
    HWND        hwndDialog;
    char        szProfile[CCHMAXPATH];
    HELPINIT    helpInit = {0};
    HAB         hab = WinQueryAnchorBlock(hwnd);
    ULONG       ulRc;
    HMODULE     hMod;
    char        szError[256];
    static      HPOINTER icon;

    char        szString[256];
    switch (msg)
    {
        case WM_INITDLG:
        {
            //Load the Strings for this dialog from the StringTable
            SetControlText(hab, hwnd, IDS_TITLE);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_HELP), IDS_HELP_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_EXIT), IDS_EXIT_BUTTON);

            // to broadcast changes
            if ((Atom = WinAddAtom(hAtomTbl, PSTB_COLORCHANGE_ATOM)) == 0)
            {
                WinMessageBox(HWND_DESKTOP,
                              hwnd,
                      "Failed to add COLORCHANGE atom to system atom table",
                              "Error!",
                              0UL,
                              MB_CANCEL | MB_ICONEXCLAMATION);
                WinDismissDlg(hwnd, DID_ERROR);
            }
            // init help system
            helpInit.cb = sizeof(helpInit);
            helpInit.phtHelpTable = (PHELPTABLE) (HLPTBL_SETUP | 0xFFFF0000);
            WinLoadString(hab, NULLHANDLE, IDS_HELP_TITLE, 256, szString);
            helpInit.pszHelpWindowTitle = szString;
            helpInit.fShowPanelId = CMIC_HIDE_PANEL_ID;
            WinLoadString(hab, NULLHANDLE, IDS_HELP_FILE, 256, szString);
            helpInit.pszHelpLibraryName = szString;

            if ((hwndHelp = WinCreateHelpInstance(hab, &helpInit)) == NULLHANDLE ||
                    helpInit.ulReturnCode)
            {
                PSUTErrorFunc(NULLHANDLE, "Error", "SetupDlgProc", "Cannot load help", 0UL);
            }
            else if (!WinAssociateHelpInstance(hwndHelp, hwnd))
            {
                PSUTErrorFunc(hwnd, "Error", "SetupDlgProc", "Failed to associate help instance", 0UL);
                WinDestroyHelpInstance(hwndHelp);
            }

            // set up notebook
            hwndNoteBook = WinWindowFromID(hwnd, NBID_SETUP);

            WinSendMsg(hwndNoteBook,
                       BKM_SETNOTEBOOKCOLORS,
                       MPFROMLONG(SYSCLR_DIALOGBACKGROUND),
                       MPFROMLONG(BKA_BACKGROUNDPAGECOLORINDEX));
            WinSendMsg(hwndNoteBook,
                       BKM_SETNOTEBOOKCOLORS,
                       MPFROMLONG(SYSCLR_DIALOGBACKGROUND),
                       MPFROMLONG(BKA_BACKGROUNDMAJORCOLORINDEX));
            WinSendMsg(hwndNoteBook,
                       BKM_SETDIMENSIONS,
                       MPFROM2SHORT(100, 30),
                       MPFROMSHORT(BKA_MAJORTAB));

            // if profile exists (ie, candybarz is installed)
            if (PrfQueryProfileString(HINI_USERPROFILE,
                                      "CandyBarZ",
                                      "Profile",
                                      NULL,
                                      szProfile,
                                      sizeof(szProfile)))
            {
                // load candybarz. this is also a test to see if we're installed and that machine
                // was rebooted after install. candybar.dll cannot be loaded for the first time
                // except by pmshell, so if machine wasn't rebooted, this will fail
                if (DosLoadModule(szError, sizeof(szError), "CANDYBAR", &hMod) == NO_ERROR)
                {

                        // insert default themes page
                        if ((ulPageID = (ULONG) WinSendMsg(hwndNoteBook,
                                                           BKM_INSERTPAGE,
                                                           (MPARAM) NULL,
                          MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_STATUSTEXTON |
                                       BKA_MAJOR | BKA_MINOR,
                                       BKA_LAST))) == 0L)
                        {
                            WinDismissDlg(hwnd, FALSE);
                        }

                        if ((hwndDialog = WinLoadDlg(hwndNoteBook,
                                                     hwndNoteBook,
                                                     ColorDlgProc,
                                                     NULLHANDLE,
                                                     DLGID_DEFAULTS,
                                             MPFROMSHORT(0))) == NULLHANDLE)
                        {
                            WinDismissDlg(hwnd, FALSE);

                        }

                        // set page info for Defaults page
                        WinSendMsg(hwndNoteBook,
                                   BKM_SETPAGEDATA,
                                   MPFROMLONG(ulPageID),
                                   MPFROMSHORT(DLGID_DEFAULTS));
                        WinLoadString(hab, NULLHANDLE, IDS_DEFAULTS_TAB, 256, szString);
                        WinSendMsg(hwndNoteBook,
                                   BKM_SETTABTEXT,
                                   MPFROMLONG(ulPageID),
                                   MPFROMP(szString));
                        WinSendMsg(hwndNoteBook,
                                   BKM_SETPAGEWINDOWHWND,
                                   MPFROMLONG(ulPageID),
                                   MPFROMHWND(hwndDialog));

                        // custom themes page
                        if ((ulPageID = (ULONG) WinSendMsg(hwndNoteBook,
                                                           BKM_INSERTPAGE,
                                                           (MPARAM) NULL,
                                             MPFROM2SHORT(BKA_AUTOPAGESIZE |
                                                          BKA_MAJOR,
                                                          BKA_LAST))) == 0L)
                        {
                            WinDismissDlg(hwnd, FALSE);
                        }
                        if ((hwndDialog = WinLoadDlg(hwndNoteBook,
                                                     hwndNoteBook,
                                                     CustomPageProc,
                                                     NULLHANDLE,
                                                     DLGID_SETUPPAGE2,
                                                     NULL)) == NULLHANDLE)
                        {
                            WinDismissDlg(hwnd, FALSE);
                        }
                        // set page info
                        WinSendMsg(hwndNoteBook,
                                   BKM_SETPAGEDATA,
                                   MPFROMLONG(ulPageID),
                                   MPFROMSHORT(DLGID_SETUPPAGE2));
                        WinLoadString(hab, NULLHANDLE, IDS_CUSTOM_TAB, 256, szString);
                        WinSendMsg(hwndNoteBook,
                                   BKM_SETTABTEXT,
                                   MPFROMLONG(ulPageID),
                                   MPFROMP(szString));
                        WinSendMsg(hwndNoteBook,
                                   BKM_SETPAGEWINDOWHWND,
                                   MPFROMLONG(ulPageID),
                                   MPFROMHWND(hwndDialog));

                        // about page
                        if ((ulPageID = (ULONG) WinSendMsg(hwndNoteBook,
                                                           BKM_INSERTPAGE,
                                                           (MPARAM) NULL,
                                             MPFROM2SHORT(BKA_AUTOPAGESIZE |
                                                          BKA_MAJOR,
                                                          BKA_LAST))) == 0L)
                        {
                            WinDismissDlg(hwnd, FALSE);
                        }
                        if ((hwndDialog = WinLoadDlg(hwndNoteBook,
                                                     hwndNoteBook,
                                                     NULL,
                                                     NULLHANDLE,
                                                     DLGID_ABOUT,
                                                     NULL)) == NULLHANDLE)
                        {
                            WinDismissDlg(hwnd, FALSE);
                        }
                        
                        WinSetWindowText(WinWindowFromID(hwndDialog,STID_WHO),AUTHORS);
                        
                        // set page info
                        WinSendMsg(hwndNoteBook,
                                   BKM_SETPAGEDATA,
                                   MPFROMLONG(ulPageID),
                                   MPFROMSHORT(DLGID_ABOUT));
                        WinLoadString(hab, NULLHANDLE, IDS_ABOUT_TAB, 256, szString);
                        WinSendMsg(hwndNoteBook,
                                   BKM_SETTABTEXT,
                                   MPFROMLONG(ulPageID),
                                   MPFROMP(szString));
                        WinSendMsg(hwndNoteBook,
                                   BKM_SETPAGEWINDOWHWND,
                                   MPFROMLONG(ulPageID),
                                   MPFROMHWND(hwndDialog));

                    DosFreeModule(hMod);
                }
            }
            // install page
            if ((ulPageID = (ULONG) WinSendMsg(hwndNoteBook,
                                               BKM_INSERTPAGE,
                                               (MPARAM) NULL,
                                  MPFROM2SHORT(BKA_AUTOPAGESIZE | BKA_MAJOR,
                                               BKA_LAST))) == 0L)
            {
                WinDismissDlg(hwnd, FALSE);
            }
            if ((hwndDialog = WinLoadDlg(hwndNoteBook,
                                         hwndNoteBook,
                                         InstallDlgProc,
                                         NULLHANDLE,
                                         DLGID_INSTALL,
                                         NULL)) == NULLHANDLE)
            {
                WinDismissDlg(hwnd, FALSE);
            }
            WinSendMsg(hwndNoteBook,
                       BKM_SETPAGEDATA,
                       MPFROMLONG(ulPageID),
                       MPFROMSHORT(DLGID_INSTALL));
            WinLoadString(hab, NULLHANDLE, IDS_INSTALL_TAB, 256, szString);
            WinSendMsg(hwndNoteBook,
                       BKM_SETTABTEXT,
                       MPFROMLONG(ulPageID),
                       MPFROMP(szString));
            WinSendMsg(hwndNoteBook,
                       BKM_SETPAGEWINDOWHWND,
                       MPFROMLONG(ulPageID),
                       MPFROMHWND(hwndDialog));

            /* set the dialog's icon */
            icon = WinLoadPointer(HWND_DESKTOP, NULLHANDLE, DLGID_SETUP);
            WinSendMsg(hwnd, WM_SETICON, MPFROMLONG(icon), NULL);

        }
        break;

        case WM_COMMAND:
        {
            switch (SHORT1FROMMP(mp1))
            {
                case PBID_EXIT:
                {
                    // clean up
                    hwndHelp = WinQueryHelpInstance(hwnd);
                    Atom = WinFindAtom(hAtomTbl, PSTB_COLORCHANGE_ATOM);

                    if (Atom)
                    {
                        WinDeleteAtom(hAtomTbl, Atom);
                    }
                    if (hwndHelp)
                    {
                        WinDestroyHelpInstance(hwndHelp);
                    }
                    WinDismissDlg(hwnd, PBID_EXIT);
                }
                break;

            }
            return ((MPARAM) 0);
        }
        break;

        case WM_HELP:
        {
            // display help
            hwndHelp = WinQueryHelpInstance(hwnd);
            if (hwndHelp)
            {
                ulRc = (ULONG) WinSendMsg(hwndHelp,
                                          HM_DISPLAY_HELP,
                                          MPFROM2SHORT(HLPIDX_SETUP, NULL),
                                          MPFROMSHORT(HM_RESOURCEID));
                if (ulRc)
                {
                    printf("help error\n");
                }
            }

            return ((MPARAM) 0);
        }
        break;

        case WM_DESTROY:
        {
            // clean up
            hwndHelp = WinQueryHelpInstance(hwnd);
            Atom = WinFindAtom(hAtomTbl, PSTB_COLORCHANGE_ATOM);

            if (Atom)
            {
                WinDeleteAtom(hAtomTbl, Atom);
            }
            if (hwndHelp)
            {
                WinDestroyHelpInstance(hwndHelp);
            }

            WinDestroyPointer(icon);

        }
        break;
    }

    return (WinDefDlgProc(hwnd, msg, mp1, mp2));
}


// update status line for install
void SetStatusLine(HWND hwnd, char *pszText)
{
    HWND        hwndMLE;

    hwndMLE = WinWindowFromID(hwnd, MLEID_STATUS);
    WinSendMsg(hwndMLE, MLM_INSERT, MPFROMP(pszText), (MPARAM) 0);

    return;
}

// custom file dlg proc so that user can choose a directory rather than a file
MRESULT EXPENTRY INIDestFileDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HWND        hwndControl;
    FILEDLG     *pfd;

    // disable files listbox
    hwndControl = WinWindowFromID(hwnd, DID_FILES_LB);
    WinEnableWindow(hwndControl, FALSE);
    // enable ok, because even current directory is valid
    hwndControl = WinWindowFromID(hwnd, DID_OK_PB);
    WinEnableWindow(hwndControl, TRUE);

    switch (msg)
    {
        case WM_COMMAND:
        {
            switch (COMMANDMSG(&msg)->cmd)
            {
                case DID_OK_PB:    // ok
                {
                    // info is already stored in filedlg structure from caller, so just
                    // set return code
                    if ((pfd = (FILEDLG *) WinQueryWindowULong(hwnd, QWL_USER)) == NULL)
                    {
                        WinDismissDlg(hwnd, DID_ERROR);
                    }
                    pfd->lReturn = DID_OK;
                    WinDismissDlg(hwnd, DID_OK);
                }
                break;

                // all other cases handled by default file dlg proc

                default:
                {
                }
                break;
            }
        }
        break;
    }

    return (WinDefFileDlgProc(hwnd, msg, mp1, mp2));
}

// proc for custom page
MRESULT EXPENTRY CustomPageProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    char        szError[2048];
    HAB         hab = WinQueryAnchorBlock(hwnd);
    HWND        hwndControl;
    HINI        hIni;
    char        szProfile[CCHMAXPATH];
    ULONG       cbKeys;
    char        *pszKeys;
    char        *pszCurrent;
    PFNWP       pfnLb;
    TCUST       tc = {0};
    long        sIndex;
    char        szTitle[MAX_CUSTOM_TITLE_SIZE];
    HATOMTBL    hAtomTbl = WinQuerySystemAtomTable();
    ATOM        Atom;
    HWND        hwndHelp;
    ULONG       ulRc;
    HWND        hwndFocus;
    short       sControlId;
    short       sHelpId;
    char        szTitleOrig[MAX_CUSTOM_TITLE_SIZE];
    char        szString[256];

    switch (msg)
    {
        case WM_INITDLG:
        {
            //Load control strings from the stringtable
            SetControlText(hab, WinWindowFromID(hwnd, PBID_CUSTOMADD), IDS_ADD_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_CUSTOMEDIT), IDS_EDIT_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_CUSTOMCLONE), IDS_CLONE_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_CUSTOMREMOVE), IDS_REMOVE_BUTTON);

            // subclass listbox so we can use dbl click to edit entry
            hwndControl = WinWindowFromID(hwnd, LBID_CUSTOMLIST);
            pfnLb = WinSubclassWindow(hwndControl, ListBoxDblClickProc);
            WinSendMsg(hwndControl, MYLBM_INIT, MPFROMP(pfnLb), (MPARAM) 0);

            // open profile
            if (!PrfQueryProfileString(HINI_USERPROFILE,
                                       "CandyBarZ",
                                       "Profile",
                                       NULL,
                                       szProfile,
                                       sizeof(szProfile)))
            {
                sprintf(szError,
                  "Failed to query CandyBarZ->Profile key. WinGetLastError "
                        "returned %x\n",
                        WinGetLastError(hab));
                PSUTErrorFunc(hwnd, "Error", "CustomPageProc", szError, 0UL);
                break;
            }
            if ((hIni = PrfOpenProfile(hab, szProfile)) == NULLHANDLE)
            {
                PSUTErrorFunc(hwnd, "Error", "CustomPageProc", "Failed to open profile", 0UL);
                break;
            }
            // get keys in custom colors app
            if (!PrfQueryProfileSize(hIni, "CustomOptions", NULL, &cbKeys) || !cbKeys)
            {
                PrfCloseProfile(hIni);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMCLONE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMREMOVE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMEDIT), FALSE);
                break;
            }
            if (DosAllocMem((PPVOID) & pszKeys, cbKeys, PAG_READ | PAG_WRITE | PAG_COMMIT) != NO_ERROR)
            {
                PSUTErrorFunc(hwnd,
                              "Error",
                              "CustomPageProc",
                              "Failed to allocate space for profile keys",
                              0UL);
                PrfCloseProfile(hIni);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMCLONE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMREMOVE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMEDIT), FALSE);
                return (MRFROMLONG(TRUE));
            }
            memset(pszKeys, 0, cbKeys);
            if (!PrfQueryProfileString(hIni, "CustomOptions", NULL, NULL, pszKeys, cbKeys))
            {
                PSUTErrorFunc(hwnd,
                              "Error",
                              "CustomPageProc",
                      "Failed to query CustomColors from CandyBarZ profile",
                              0UL);
                DosFreeMem(pszKeys);
                PrfCloseProfile(hIni);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMCLONE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMREMOVE), FALSE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMEDIT), FALSE);
                return (MRFROMLONG(TRUE));
            }
            PrfCloseProfile(hIni);

            // parse keys and place them in listbox
            hwndControl = WinWindowFromID(hwnd, LBID_CUSTOMLIST);
            pszCurrent = pszKeys;
            while (*pszCurrent)
            {
                WinInsertLboxItem(hwndControl, LIT_SORTASCENDING, pszCurrent);
                pszCurrent += strlen(pszCurrent) + 1;
            }

            // select first item
            if (WinSendMsg(hwndControl, LM_QUERYITEMCOUNT, (MPARAM) 0, (MPARAM) 0))
            {
                WinSendMsg(hwndControl, LM_SELECTITEM, MPFROMSHORT(0), MPFROMSHORT(TRUE));
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMCLONE), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMEDIT), TRUE);
                WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMREMOVE), TRUE);
            }

            DosFreeMem(pszKeys);
        }
        break;

        case WM_COMMAND:
        {
            switch (COMMANDMSG(&msg)->cmd)
            {
                case PBID_CUSTOMADD:  // add new config
                {
                    tc.cbFix = sizeof(tc);
                    tc.bCloning = FALSE;

                    // display dlg for customizing
                    if (WinDlgBox(HWND_DESKTOP,
                                  hwnd,
                                  CustomizeDlgProc,
                                  NULLHANDLE,
                                  DLGID_CUSTOMIZE,
                                  &tc) == PBID_CUSTSAVE)
                    {
                        // if we saved, insert new entry
                        hwndControl = WinWindowFromID(hwnd, LBID_CUSTOMLIST);
                        WinInsertLboxItem(hwndControl, LIT_SORTASCENDING, tc.szTitle);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMCLONE), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMEDIT), TRUE);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_CUSTOMREMOVE), TRUE);
                    }
                }
                break;

                case PBID_CUSTOMEDIT:
                {
                    // get name of entry we're editing
                    tc.cbFix = sizeof(tc);
                    tc.bCloning = FALSE;

                    hwndControl = WinWindowFromID(hwnd, LBID_CUSTOMLIST);
                    sIndex = (long) WinSendMsg(hwndControl,
                                               LM_QUERYSELECTION,
                                               MPFROMSHORT(LIT_FIRST),
                                               (MPARAM) 0);
                    if (sIndex == LIT_NONE)  // if nothing selected, do nothing

                    {
                        break;
                    }

                    WinSendMsg(hwndControl,
                               LM_QUERYITEMTEXT,
                               MPFROM2SHORT(sIndex, sizeof(tc.szTitle)),
                               tc.szTitle);
                    // save this so title can be updated after edit
                    strcpy(szTitleOrig, tc.szTitle);

                    // display edit dlg
                    if (WinDlgBox(HWND_DESKTOP,
                                  hwnd,
                                  CustomizeDlgProc,
                                  NULLHANDLE,
                                  DLGID_CUSTOMIZE,
                                  &tc) == PBID_CUSTSAVE)
                    {
                        // if we saved, see if the title has changed. if so, update listbox
                        // entry
                        if (strcmp(szTitleOrig, tc.szTitle))
                        {
                            WinDeleteLboxItem(hwndControl, (long) sIndex);
                            WinInsertLboxItem(hwndControl, LIT_SORTASCENDING, tc.szTitle);
                        }
                    }
                }
                break;

                case PBID_CUSTOMCLONE:
                {
                    // get name of entry we're editing
                    tc.cbFix = sizeof(tc);
                    tc.bCloning = TRUE;

                    hwndControl = WinWindowFromID(hwnd, LBID_CUSTOMLIST);
                    sIndex = (long) WinSendMsg(hwndControl,
                                               LM_QUERYSELECTION,
                                               MPFROMSHORT(LIT_FIRST),
                                               (MPARAM) 0);
                    if (sIndex == LIT_NONE)  // if nothing selected, do nothing

                    {
                        break;
                    }

                    WinSendMsg(hwndControl,
                               LM_QUERYITEMTEXT,
                               MPFROM2SHORT(sIndex, sizeof(tc.szTitle)),
                               tc.szTitle);

                    // display edit dlg
                    if (WinDlgBox(HWND_DESKTOP,
                                  hwnd,
                                  CustomizeDlgProc,
                                  NULLHANDLE,
                                  DLGID_CUSTOMIZE,
                                  &tc) == PBID_CUSTSAVE)
                    {
                        WinInsertLboxItem(hwndControl, LIT_SORTASCENDING, tc.szTitle);
                    }
                }
                break;

                case PBID_CUSTOMREMOVE:  // remove an entry
                {
                    // the customize dlg broadcasts for add and edit, but we don't use it
                    // for remove, so get atom
                    if ((Atom = WinFindAtom(hAtomTbl, PSTB_COLORCHANGE_ATOM)) == 0)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "CustomPageProc",
                                      "Failed to locate COLORCHANGE atom",
                                      0UL);
                        break;
                    }

                    // get id/text of deleted entry
                    hwndControl = WinWindowFromID(hwnd, LBID_CUSTOMLIST);
                    sIndex = (long) WinSendMsg(hwndControl,
                                               LM_QUERYSELECTION,
                                               MPFROMSHORT(LIT_FIRST),
                                               (MPARAM) 0);
                    if (sIndex == LIT_NONE)  // if nothing selected, leave

                    {
                        break;
                    }

                    WinSendMsg(hwndControl,
                               LM_QUERYITEMTEXT,
                               MPFROM2SHORT(sIndex, sizeof(szTitle)),
                               szTitle);
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
                                      "CustomPageProc",
                               "Failed to query CandyBarZ profile location",
                                      0UL);
                        break;
                    }
                    if ((hIni = PrfOpenProfile(hab, szProfile)) == NULLHANDLE)
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "CustomPageProc",
                                      "Failed to open CandyBarZ profile",
                                      0UL);
                        break;
                    }
                    // delete key for deleted entry
                    if (!PrfWriteProfileData(hIni, "CustomOptions", szTitle, NULL, 0))
                    {
                        PSUTErrorFunc(hwnd,
                                      "Error",
                                      "CustomPageProc",
                                      "Failed to delete profile key",
                                      0UL);
                        PrfCloseProfile(hIni);
                        break;
                    }

                    {
                        char pBuffer[CCHMAXPATH * 3 + 1];
                        char pTest[MAX_CUSTOM_TITLE_SIZE + 1];
                        char *pStr;
                        strcpy(pTest, szTitle);
                        strcat(pTest, "_");
                        PrfQueryProfileString(hIni, "CustomOptionsData", NULL, NULL, pBuffer, CCHMAXPATH * 3);
                        pStr = pBuffer;
                        do
                        {
                            if ( strncmp(pStr, pTest, strlen(pTest)) == 0)
                            {
                                // delete key for deleted entry
                                if (!PrfWriteProfileData(hIni, "CustomOptionsData", pStr, NULL, 0))
                                {
                                    PSUTErrorFunc(hwnd,
                                        "Error",
                                        "Failed to Delete Custom Key",
                                        pStr,
                                        0UL);
                                    PrfCloseProfile(hIni);
                                    break;
                                }
                            }
                            pStr += strlen(pStr) + 1;
                        }while (strlen(pStr) > 0);
                    }
                    PrfCloseProfile(hIni);

                    // broadcast change
                    WinBroadcastMsg(HWND_DESKTOP,
                                    Atom,
                                    (MPARAM) 0,
                                    (MPARAM) 0,
                                    BMSG_POST | BMSG_DESCENDANTS);

                    WinBroadcastMsg(HWND_OBJECT,
                                    Atom,
                                    (MPARAM) 0,
                                    (MPARAM) 0,
                                    BMSG_POST | BMSG_DESCENDANTS);

                    // delete entry from listbox
                    sIndex = (long) WinSendMsg(hwndControl,
                                               LM_QUERYSELECTION,
                                               MPFROMSHORT(LIT_FIRST),
                                               (MPARAM) 0);
                    WinSendMsg(hwndControl, LM_DELETEITEM, MPFROMSHORT(sIndex), (MPARAM) 0);
                }
                break;
            }

            return ((MPARAM) 0);
        }
        break;

        case WM_HELP:
        {
            hwndHelp = WinQueryHelpInstance(hwnd);

            if (hwndHelp)
            {
                // get id of control with focus
                hwndFocus = WinQueryFocus(HWND_DESKTOP);
                sControlId = WinQueryWindowUShort(hwndFocus, QWS_ID);
                switch (sControlId)
                {
                    case PBID_CUSTOMADD:
                    case PBID_CUSTOMEDIT:
                    {
                        sHelpId = HLPIDX_ADDEDIT;
                    }
                    break;

                    case PBID_CUSTOMCLONE:
                    {
                        sHelpId = HLPIDX_CLONE;
                    }
                    break;

                    case PBID_CUSTOMREMOVE:
                    {
                        sHelpId = HLPIDX_REMOVE;
                    }
                    break;

                    default:
                    {
                        sHelpId = HLPIDX_PAGE2;
                    }
                    break;
                }
                // display help
                ulRc = (ULONG) WinSendMsg(hwndHelp,
                                          HM_DISPLAY_HELP,
                                          MPFROM2SHORT(sHelpId, NULL),
                                          MPFROMSHORT(HM_RESOURCEID));
                if (ulRc)
                {
                    printf("help error\n");
                }
            }

            return ((MPARAM) 0);
        }
        break;

    }
    return (WinDefDlgProc(hwnd, msg, mp1, mp2));
}

// lets us use dbl click to edit entry
MRESULT EXPENTRY ListBoxDblClickProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PFNWP       pfnOrig;
    HWND        hwndParent;

    switch (msg)
    {
        case MYLBM_INIT:           // store original proc
        {
            if ((pfnOrig = (PFNWP) mp1) == NULL)
            {
                return ((MRESULT) FALSE);
            }
            WinSetWindowPtr(hwnd, QWL_USER, (PVOID) pfnOrig);

            return ((MRESULT) TRUE);
        }
        break;

        case WM_BUTTON1DBLCLK:     // edit entry
        {
            hwndParent = WinQueryWindow(hwnd, QW_PARENT);

            WinSendMsg(hwndParent,
                       WM_COMMAND,
                       MPFROMSHORT(PBID_CUSTOMEDIT),
                       MPFROM2SHORT(CMDSRC_OTHER, FALSE));
        }
        break;

        // all else handled by original proc
    }

    pfnOrig = (PFNWP) WinQueryWindowPtr(hwnd, QWL_USER);

    return (pfnOrig(hwnd, msg, mp1, mp2));
}

/*************************************************************************************************************

     $Id: setup.c,v 1.3 2000/09/24 21:02:42 cwohlgemuth Exp $

     $Log: setup.c,v $
     Revision 1.3  2000/09/24 21:02:42  cwohlgemuth
     Added about page.

     Revision 1.2  2000/03/26 06:51:22  enorman
     resyncing with my local tree.

     Revision 1.1  1999/06/15 20:47:58  ktk
     Import

     Revision 1.12  1998/10/17 19:32:20  enorman
     Made some changes to eliminate PSRT.DLL

     Revision 1.11  1998/09/27 23:31:09  enorman
     Added help for the color cloning

     Revision 1.10  1998/09/26 07:05:22  enorman
     Added code for custom color cloning

     Revision 1.9   1998/09/23 21:13:36 enorman
     I changed setup.c to make it load an icon for the cbsetup main
     dialog

     Revision 1.8   1998/09/22 12:08:33 mwagner
     Added better dialog support

     Revision 1.7   1998/09/15 22:09:50 enormon
     Fixed a type that prevented this from compiling

     Revision 1.6   1998/09/15 10:09:20 enorman
     Added code to load the two new dialog pages

     Revision 1.5   1998/09/12 10:08:19 pgarner
     Added copyright notice

     Revision 1.4   1998/09/12 07:35:47 mwagner
     Added support for bitmap images

     Revision 1.1   1998/06/08 14:18:57 pgarner
     Initial Checkin

*************************************************************************************************************/
