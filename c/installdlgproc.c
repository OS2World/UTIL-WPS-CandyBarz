/******************************************************************************

  Copyright Netlabs, 1998, this code may not be used for any military purpose

******************************************************************************/

#include "setup.h"

MRESULT EXPENTRY InstallDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    char szError[2048];
    HAB hab = WinQueryAnchorBlock(hwnd);
    ULONG ulRc;
    char szFile[CCHMAXPATH];
    ULONG ulBootDrive;
    char szConfigSys[CCHMAXPATH];
    FILE *fp, *fp2;
    char *pszSysDlls;
    ULONG cbSysDlls;
    char szDir[CCHMAXPATH];
    char *pszCandyBar;
    char szItemText[CCHMAXPATH];
    HWND hwndHelp;
    HWND hwndFocus;
    short sControlId;
    short sHelpId;
    char *ch;
    TIB *pTib;
    PIB *pPib;
    char drive[3];
    BOOL bLibPathFound = FALSE;
    char szString[256];

    switch (msg)
    {
        case WM_INITDLG:
        {
            //Load control strings from the stringtable
            SetControlText(hab, WinWindowFromID(hwnd, STID_STATUS), IDS_STATUS_LABEL);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_INSTALL), IDS_INSTALL_BUTTON);
            SetControlText(hab, WinWindowFromID(hwnd, PBID_UNINSTALL), IDS_UNINSTALL_BUTTON);

            if (!PrfQueryProfileString(HINI_USERPROFILE,
                                       "CandyBarZ",
                                       "Profile",
                                       NULL,
                                       szFile,
                                       sizeof(szFile)))
            {
                WinEnableWindow(WinWindowFromID(hwnd, PBID_UNINSTALL), FALSE);
            }
        }
        break;

        case WM_COMMAND:
        {
            switch (COMMANDMSG(&msg)->cmd)
            {
                case PBID_INSTALL:
                {

                    //determine the current directory, and save in szDir
                    DosGetInfoBlocks(&pTib, &pPib);
                    DosQueryModuleName(pPib->pib_hmte, sizeof(szDir), szDir);
                    _splitpath(szDir, drive, szFile, NULL, NULL);
                    _makepath(szDir, drive, szFile, NULL, NULL);

                    SetStatusLine(hwnd, "Backup of config.sys saved as config.cbz\n");

                    if ((ulRc = DosQuerySysInfo(QSV_BOOT_DRIVE,
                                                QSV_BOOT_DRIVE,
                                                &ulBootDrive,
                                          sizeof(ulBootDrive))) != NO_ERROR)
                    {
                        sprintf(szError,
                                "Failed to query boot drive. Return code from DosQuerySysInfo is %lu\n",
                                ulRc);
                        SetStatusLine(hwnd, szError);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INSTALL), FALSE);
                        break;
                    }
                    sprintf(szConfigSys, "%c:\\CONFIG.SYS", ulBootDrive + 'A' - 1);

                    strncpy(szFile, szConfigSys, 10);
                    szFile[10] = '\0';
                    strcat(szFile, "CBZ");
                    DosCopy(szConfigSys, szFile, DCPY_EXISTING);

                    if ((fp = fopen(szFile, "r")) == NULL)
                    {
                        sprintf(szError, "Failed to open %s for reading.\n", szConfigSys);
                        SetStatusLine(hwnd, szError);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INSTALL), FALSE);
                        break;
                    }

                    if ((fp2 = fopen(szConfigSys, "w")) == NULL)
                    {
                        sprintf(szError, "Failed to open %s for writing.\n", szConfigSys);
                        SetStatusLine(hwnd, szError);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INSTALL), FALSE);
                        break;
                    }

                    do
                    {
                        if (!fgets(szError, sizeof(szError), fp))
                        {
                            if (feof(fp))
                            {
                                if (bLibPathFound)
                                {
                                    break;
                                }
                                sprintf(szError, "LIBPATH not found in %s\n", szConfigSys);
                                SetStatusLine(hwnd, szError);
                            }
                            else
                            {
                                sprintf(szError, "Read failure on %s\n", szConfigSys);
                                SetStatusLine(hwnd, szError);
                            }
                            fclose(fp);
                            fclose(fp2);

                            WinEnableWindow(WinWindowFromID(hwnd, PBID_INSTALL), FALSE);
                            return (WinDefDlgProc(hwnd, msg, mp1, mp2));
                        }

                        if (!strnicmp(szError, "LIBPATH", sizeof("LIBPATH") - 1))
                        {
                            strncpy(szFile, szDir, strlen(szDir) - 1);
                            szFile[strlen(szDir) - 1] = '\0';

                            if (strstr(szError, szFile) == NULL)
                            {
                                strncpy(szFile, szError, strlen(szError) - 1);
                                szFile[strlen(szError) - 1] = '\0';
                                fputs(szFile, fp2);
                                if (szFile[strlen(szFile) - 1] != ';')
                                    fputs(";", fp2);
                                strncpy(szFile, szDir, strlen(szDir) - 1);
                                szFile[strlen(szDir) - 1] = '\0';
                                fputs(szFile, fp2);
                                fputs(";\n", fp2);
                                bLibPathFound = TRUE;
                            }
                            else
                                fputs(szError, fp2);
                        }
                        else
                            fputs(szError, fp2);

                    }
                    while (!feof(fp));
                    fclose(fp);
                    fclose(fp2);

                    SetStatusLine(hwnd, "Adding the CandyBarZ Base Path to LIBPATH\n");

                    SetStatusLine(hwnd, "Adding path to CandyBar.ini to USER INI file\n");
                    //complete path for ini file.
                    _makepath(szFile, NULL, szDir, "CandyBar", "ini");
                    if (!PrfWriteProfileString(HINI_USERPROFILE,
                                               "CandyBarZ",
                                               "Profile",
                                               szFile))
                    {
                        sprintf(szError,
                                "Failed to update USER INI file. WinGetLastError returned %x\n",
                                WinGetLastError(hab));
                        SetStatusLine(hwnd, szError);
                        break;
                    }

                    SetStatusLine(hwnd, "Adding CandyBarZ base path to USER INI file\n");
                    _makepath(szFile, NULL, szDir, NULL, NULL);
                    if (!PrfWriteProfileString(HINI_USERPROFILE,
                                               "CandyBarZ",
                                               "BasePath",
                                               szFile))
                    {
                        sprintf(szError,
                                "Failed to update USER INI file. WinGetLastError returned %x\n",
                                WinGetLastError(hab));
                        SetStatusLine(hwnd, szError);
                        break;
                    }

                    SetStatusLine(hwnd, "Adding CandyBarZ to SYS_DLLS->LoadPerProcess\n");

                    if (!PrfQueryProfileSize(HINI_USERPROFILE,
                                             "SYS_DLLS",
                                             "LoadPerProcess",
                                             &cbSysDlls))
                    {
                        sprintf(szError,
                                "Failed to query SYS_DLLS->LoadPerProcess key size. WinGetLastError "
                                "returned %x\n",
                                WinGetLastError(hab));
                        SetStatusLine(hwnd, szError);
                        break;
                    }
                    if ((ulRc = DosAllocMem((PPVOID) & pszSysDlls,
                                        cbSysDlls + sizeof("CANDYBAR") + 32,
                            PAG_READ | PAG_WRITE | PAG_COMMIT)) != NO_ERROR)
                    {
                        sprintf(szError,
                                "Failed to allocate memory for profile data. DosAllocMem returned "
                                "%lu\n",
                                ulRc);
                        SetStatusLine(hwnd, szError);
                        break;
                    }
                    memset(pszSysDlls, 0, cbSysDlls + sizeof("CANDYBAR") + 32);

                    if (!PrfQueryProfileString(HINI_USERPROFILE,
                                               "SYS_DLLS",
                                               "LoadPerProcess",
                                               NULL,
                                               pszSysDlls,
                                               cbSysDlls))
                    {
                        sprintf(szError,
                                "Failed to query SYS_DLLS->LoadPerProcess key. WinGetLastError "
                                "returned %x\n",
                                WinGetLastError(hab));
                        SetStatusLine(hwnd, szError);
                        DosFreeMem(pszSysDlls);
                        break;
                    }

                    // Check to see if CandyBarZ and Object Desktop are both installed.  CandyBarZ
                    //     needs to be loaded before Object Desktop!
                    if (strstr(pszSysDlls, "CANDYBAR") && strstr(pszSysDlls, "OBJDWCTL"))
                    {
                        // If CANDYBAR is before OBJDWCTL in the SYS_DLL->LoadPerProcess string,
                        //    remove it from the string.  the next section will re-add it in the
                        //    proper place in the string.  i.e. CANDYBAR must be loaded before
                        //    OBJDWCTL for PMView to function properly
                        if (strstr(pszSysDlls, "OBJDWCTL") < strstr(pszSysDlls, "CANDYBAR"))
                        {
                            pszCandyBar = strstr(pszSysDlls, "CANDYBAR");

                            *pszCandyBar = 0;
                            memmove(pszCandyBar,
                                    pszCandyBar + strlen(" CANDYBAR"),
                                    cbSysDlls + 1 - strlen(pszSysDlls) - strlen("CANDYBAR"));

                        }
                    }

                    // If CANDYBAR isn't in the SYS_DLL->LoadPerProcess string, add it, making sure
                    //    it is before OBJDWCTL in the string.
                    if (!strstr(pszSysDlls, "CANDYBAR"))
                    {
                        // If Object Desktop is installed, insert CANDYBAR before OBJDWCTL
                        if ((pszCandyBar = strstr(pszSysDlls, "OBJDWCTL")) != NULL)
                        {
                            // shift the string starting at OBJDWCTL right 9 characters to make
                            //    room for CANDYBAR in the string
                            memmove(pszCandyBar + strlen("CANDYBAR "),
                                    pszCandyBar,
                                    strlen(pszCandyBar));
                            memcpy(pszCandyBar, "CANDYBAR ", sizeof("CANDYBAR ") - 1);
                        }
                        else    //Object Desktop NOT installed so just append to the end of the string.

                            sprintf(pszSysDlls + strlen(pszSysDlls), " CANDYBAR");

                        if (!PrfWriteProfileString(HINI_USERPROFILE,
                                                   "SYS_DLLS",
                                                   "LoadPerProcess",
                                                   pszSysDlls))
                        {
                            sprintf(szError,
                                    "Failed to update SYS_DLLS->LoadPerProcess. WinGetLastError "
                                    "returned %x\n",
                                    WinGetLastError(hab));
                            SetStatusLine(hwnd, szError);
                            DosFreeMem(pszSysDlls);
                            break;
                        }
                    }

                    DosFreeMem(pszSysDlls);

                    WinCreateObject("WPFolder", "CandyBarZ v1.30", "OBJECTID=<CandyBarZ_Folder>",
                                    "<WP_DESKTOP>", CO_UPDATEIFEXISTS);
                    _makepath(szFile, NULL, szDir, "CBSetup", "EXE");
                    sprintf(szItemText, "OBJECTID=<CandyBarZ_CBSetup>;EXENAME=%s", szFile);
                    WinCreateObject("WPProgram", "CandyBarZ\nSetup", szItemText,
                                    "<CandyBarZ_Folder>", CO_UPDATEIFEXISTS);
                    _makepath(szFile, NULL, szDir, "CBBuild", "EXE");
                    sprintf(szItemText, "OBJECTID=<CandyBarZ_ThemeBuilder>;EXENAME=%s", szFile);
                    WinCreateObject("WPProgram", "CandyBarZ\nThemeBuilder", szItemText,
                                    "<CandyBarZ_Folder>", CO_UPDATEIFEXISTS);

                    SetStatusLine(hwnd, "You must reboot your machine to load CandyBarZ");
                    WinMessageBox(HWND_DESKTOP,
                                  hwnd,
                           "You must reboot your machine to load CandyBarZ",
                                  "Installation Successful!",
                                  0,
                                  MB_OK);
                    WinPostMsg( hwnd, WM_QUIT, ( MPARAM )0, ( MPARAM )0 );
                }
                break;

                case PBID_UNINSTALL:
                {
                    if (WinMessageBox(HWND_DESKTOP,
                                      hwnd,
                            "Are you sure you want to uninstall CandyBarZ?",
                                      "Ohhh, Noooo!",
                                      0,
                                      MB_YESNO | MB_ICONASTERISK) == MBID_NO)
                    {
                        break;
                    }

                    SetStatusLine(hwnd, "Removing CandyBarZ Keys from User INI file\n");

                    // remove candybarz key
                    if (!PrfWriteProfileString(HINI_USERPROFILE,
                                               "CandyBarZ",
                                               "Profile",
                                               NULL))
                    {
                        sprintf(szError,
                                "Failed to remove CandyBarZ->Profile key. WinGetLastError "
                                "returned %x\n",
                                WinGetLastError(hab));
                        SetStatusLine(hwnd, szError);
                        break;
                    }

                    // remove candybarz key
                    if (!PrfWriteProfileString(HINI_USERPROFILE,
                                               "CandyBarZ",
                                               "BasePath",
                                               NULL))
                    {
                        sprintf(szError,
                                "Failed to remove CandyBarZ->Profile key. WinGetLastError "
                                "returned %x\n",
                                WinGetLastError(hab));
                        SetStatusLine(hwnd, szError);
                        break;
                    }

                    // remove loadperprocess
                    if (!PrfQueryProfileSize(HINI_USERPROFILE,
                                             "SYS_DLLS",
                                             "LoadPerProcess",
                                             &cbSysDlls))
                    {
                        sprintf(szError,
                                "Failed to query SYS_DLLS->LoadPerProcess key size. WinGetLastError "
                                "returned %x\n",
                                WinGetLastError(hab));
                        SetStatusLine(hwnd, szError);

                        break;
                    }
                    if ((ulRc = DosAllocMem((PPVOID) & pszSysDlls,
                                            cbSysDlls + 1,
                            PAG_READ | PAG_WRITE | PAG_COMMIT)) != NO_ERROR)
                    {
                        sprintf(szError,
                                "Failed to allocate memory for profile data. DosAllocMem returned "
                                "%lu\n",
                                ulRc);
                        SetStatusLine(hwnd, szError);
                        return (MRFROMLONG(TRUE));
                    }
                    memset(pszSysDlls, 0, cbSysDlls + 1);

                    if (!PrfQueryProfileString(HINI_USERPROFILE,
                                               "SYS_DLLS",
                                               "LoadPerProcess",
                                               NULL,
                                               pszSysDlls,
                                               cbSysDlls))
                    {
                        sprintf(szError,
                                "Failed to query SYS_DLLS->LoadPerProcess key. WinGetLastError "
                                "returned %x\n",
                                WinGetLastError(hab));
                        SetStatusLine(hwnd, szError);
                        DosFreeMem(pszSysDlls);
                        break;
                    }
                    if ((pszCandyBar = strstr(pszSysDlls, " CANDYBAR")) != NULL)
                    {
                        *pszCandyBar = 0;
                        memmove(pszCandyBar,
                                pszCandyBar + strlen(" CANDYBAR"),
                                cbSysDlls + 1 - strlen(pszSysDlls) - strlen("CANDYBAR"));
                        if (!PrfWriteProfileString(HINI_USERPROFILE,
                                                   "SYS_DLLS",
                                                   "LoadPerProcess",
                                                   pszSysDlls))
                        {
                            sprintf(szError,
                                    "Failed to update SYS_DLLS->LoadPerProcess. WinGetLastError "
                                    "returned %x\n",
                                    WinGetLastError(hab));
                            SetStatusLine(hwnd, szError);
                            DosFreeMem(pszSysDlls);
                            break;
                        }
                    }

                    DosFreeMem(pszSysDlls);



                    if ((ulRc = DosQuerySysInfo(QSV_BOOT_DRIVE,
                                                QSV_BOOT_DRIVE,
                                                &ulBootDrive,
                                          sizeof(ulBootDrive))) != NO_ERROR)
                    {
                        sprintf(szError,
                                "Failed to query boot drive. Return code from DosQuerySysInfo is %lu\n",
                                ulRc);
                        SetStatusLine(hwnd, szError);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INSTALL), FALSE);
                        break;
                    }
                    sprintf(szConfigSys, "%c:\\CONFIG.SYS", ulBootDrive + 'A' - 1);

                    DosCopy(szConfigSys, "CBSetup.tmp", DCPY_EXISTING);

                    if ((fp = fopen("CBSetup.tmp", "r")) == NULL)
                    {
                        sprintf(szError, "Failed to open %s for reading.\n", szConfigSys);
                        SetStatusLine(hwnd, szError);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INSTALL), FALSE);
                        break;
                    }

                    if ((fp2 = fopen(szConfigSys, "w")) == NULL)
                    {
                        sprintf(szError, "Failed to open %s for writing.\n", szConfigSys);
                        SetStatusLine(hwnd, szError);
                        WinEnableWindow(WinWindowFromID(hwnd, PBID_INSTALL), FALSE);
                        break;
                    }

                    do
                    {
                        if (!fgets(szError, sizeof(szError), fp))
                        {
                            if (feof(fp))
                            {
                                if (bLibPathFound)
                                {
                                    break;
                                }
                                sprintf(szError, "LIBPATH not found in %s\n", szConfigSys);
                                SetStatusLine(hwnd, szError);
                            }
                            else
                            {
                                sprintf(szError, "Read failure on %s\n", szConfigSys);
                                SetStatusLine(hwnd, szError);
                            }
                            fclose(fp);
                            fclose(fp2);

                            WinEnableWindow(WinWindowFromID(hwnd, PBID_INSTALL), FALSE);
                            return (WinDefDlgProc(hwnd, msg, mp1, mp2));
                        }

                        if (!strnicmp(szError, "LIBPATH", sizeof("LIBPATH") - 1))
                        {
                            //determine the current directory, and save in szDir
                            DosGetInfoBlocks(&pTib, &pPib);
                            DosQueryModuleName(pPib->pib_hmte, sizeof(szDir), szDir);
                            _splitpath(szDir, drive, szFile, NULL, NULL);
                            _makepath(szDir, drive, szFile, NULL, NULL);

                            strncpy(szFile, szDir, strlen(szDir) - 1);
                            szFile[strlen(szDir) - 1] = '\0';

                            if ( (ch = strstr(szError, szFile)) != NULL)
                            {
                                //candyBarZ directory was found in LIBPATH, so Remove it and output the rest.
                                memmove(ch,
                                        ch + strlen(szDir),
                                        strlen(ch) - strlen(szDir) + 1);

                                fputs(szError, fp2);
                                bLibPathFound = TRUE;
                            }
                            else
                                fputs(szError, fp2);
                        }
                        else
                            fputs(szError, fp2);

                    }
                    while (!feof(fp));
                    fclose(fp);
                    fclose(fp2);
                    DosDelete("CBSetup.tmp");
                    SetStatusLine(hwnd, "You may remove the CandyBarZ Directory after rebooting.\n");

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
                hwndFocus = WinQueryFocus(HWND_DESKTOP);
                sControlId = WinQueryWindowUShort(hwndFocus, QWS_ID);
                switch (sControlId)
                {
                    case MLEID_STATUS:
                    {
                        sHelpId = HLPIDX_STATUS;
                    }
                    break;

                    case PBID_INSTALL:
                    {
                        sHelpId = HLPIDX_INSTBUTTON;
                    }
                    break;

                    case PBID_UNINSTALL:
                    {
                        sHelpId = HLPIDX_UNINSTALL;
                    }
                    break;

                    default:
                    {
                        sHelpId = HLPIDX_INSTALL;
                    }
                    break;
                }

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

/*************************************************************************************************************

    $Id: installdlgproc.c,v 1.2 2000/03/26 06:51:22 enorman Exp $

    $Log: installdlgproc.c,v $
    Revision 1.2  2000/03/26 06:51:22  enorman
    resyncing with my local tree.

    Revision 1.1  1999/06/15 20:47:58  ktk
    Import

    Revision 1.8  1998/10/17 19:31:53  enorman
    Made some changes to eliminate PSRT.DLL

    Revision 1.7  1998/10/04 10:33:46  mwagner
    Removed message that says "After reboot, you may remove xxx"
    Added message box asking if Whack-A-Mole is installed and
    remove PSRT.DLL only if answer is no

    Revision 1.6  1998/09/12 10:07:44  pgarner
    Added copyright notice

    Revision 1.5  1998/09/12 07:35:46  mwagner
    Added support for bitmap images

    Revision 1.2  1998/06/14 11:42:20  mwagner
    Made a large number of changes so that an attempt is made to install the DLLs in the OS2\DLL directory.
    Use DosReplaceModule to delete existing DLLs upon installation, so only one reboot is required.
    Use DosReplaceModule to delete DLLs upon uninstallation, so user doesn't have to reboot and do it.

    Revision 1.1  1998/06/08 14:18:57  pgarner
    Initial Checkin

*************************************************************************************************************/
