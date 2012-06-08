/* $Id: themebuilder.c,v 1.4 2000/09/24 20:37:53 cwohlgemuth Exp $ */

/******************************************************************************

  Copyright Netlabs, 1998-2000 

  http://www.netlabs.org

******************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "themeBuilder.h"

int main(int argc, char *argv[])
{
    HAB hab;
    HMQ hmq;

    hab = WinInitialize(0UL);
    hmq = WinCreateMsgQueue(hab, 100UL);

    // main setup dialog
    if (WinDlgBox(HWND_DESKTOP, NULLHANDLE, ThemeDlgProc, NULLHANDLE, DLGID_THEMESETUP, 0) == DID_ERROR)
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
MRESULT EXPENTRY ThemeDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    HWND                hwndHelp;
    HWND                hwndNoteBook;
    static HWND         hwndPreview = NULLHANDLE;
    HELPINIT            helpInit = {0};
    short               sControlId;
    short               sHelpId;
    ULONG               ulRc;
    static HPOINTER     icon;
    HWND                hwndFocus = WinQueryFocus( HWND_DESKTOP );

    switch (msg)
    {
        case WM_INITDLG:
        {
            // init help system
            helpInit.cb = sizeof(helpInit);
            helpInit.phtHelpTable = (PHELPTABLE) (HLPTBL_SETUP | 0xFFFF0000);
            helpInit.pszHelpWindowTitle = "CandyBarZ Help";
            helpInit.fShowPanelId = CMIC_HIDE_PANEL_ID;
            helpInit.pszHelpLibraryName = "CBSetup.hlp";

            if ((hwndHelp = WinCreateHelpInstance(WinQueryAnchorBlock(hwnd), &helpInit)) == NULLHANDLE ||
                    helpInit.ulReturnCode)
            {
                PSUTErrorFunc(hwnd, "Error", "ThemeDlgProc", "Cannot load help", 0UL);
            }
            else if (!WinAssociateHelpInstance(hwndHelp, hwnd))
            {
                PSUTErrorFunc(hwnd, "Error", "ThemeDlgProc", "Failed to associate help instance", 0UL);
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
            /* Set size of tabs */
            WinSendMsg(hwndNoteBook,
                       BKM_SETDIMENSIONS,
                       MPFROM2SHORT(100, 30),
                       MPFROMSHORT(BKA_MAJORTAB));
            //Add the NoteBook Pages
            AddNotebookPage(hwndNoteBook, DLGID_INFO, InfoDlgProc,
                            "~Info", "", "", TRUE, "INFO");
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "~System", "", "", TRUE, "SYSTEM");
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "~Titlebar", "", "", TRUE, "TITLEBAR");
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "~Frame Background", "", "", TRUE, "FRAME");
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "F~rame Border", "", "", TRUE, "FRAMEBRDR");
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "~PushButtons", "", "", TRUE, "PUSHBUTTON");
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "~RadioButtons", "", "", TRUE, "RADIOBUTTON");
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "~CheckBoxes", "", "", TRUE, "CHECKBOX");
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "M~in/Max", "", "", TRUE, "MINMAX");

            // No plugins for these yet.
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "~Menus", "", "", TRUE, "MENU");
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "~Notebook Background", "", "", TRUE, "NOTEBOOK");
            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "~Scrollbars", "", "", TRUE, "SCROLLBAR");

            AddNotebookPage(hwndNoteBook, DLGID_DEFAULTS, WindowClassDlgProc,
                            "C~ontainer Background", "", "", TRUE, "CONTAINER");

            // set the dialog's icon
            icon = WinLoadPointer(HWND_DESKTOP, NULLHANDLE, DLGID_THEMESETUP);
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

                    if (hwndHelp)
                    {
                        WinDestroyHelpInstance(hwndHelp);
                    }
                    WinDismissDlg(hwnd, PBID_EXIT);
                }
                break;

                case PBID_SAVE:
                {
                    HWND hwndChild, hwndNotebook;
                    ULONG pageID;
                    char filename[CCHMAXPATH];
                    FILE *fp;

                    hwndNotebook = WinWindowFromID(hwnd, NBID_SETUP);
                    //find the hwnd of the Info Page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, MPFROMLONG(0), MPFROM2SHORT(BKA_FIRST, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND, MPFROMLONG(pageID), MPFROMLONG(0));
                    //Make sure a ThemeName has been specified
                    WinQueryWindowText(WinWindowFromID(hwndChild, EFID_THEMENAME), CCHMAXPATH, filename);
                    if (strlen(filename) == 0)
                    {
                        PSUTErrorFunc(hwnd, "Error", "ThemeDlgProc", "Please Enter a Theme Name", 0UL);
                        break;
                    }
                    //Make sure a ThemeAuthor has been specified
                    WinQueryWindowText(WinWindowFromID(hwndChild, EFID_THEMEAUTHOR), CCHMAXPATH, filename);
                    if (strlen(filename) == 0)
                    {
                        PSUTErrorFunc(hwnd, "Error", "ThemeDlgProc", "Please Enter a Theme Author", 0UL);
                        break;
                    }
                    //Make sure a ThemeDate has been specified
                    WinQueryWindowText(WinWindowFromID(hwndChild, EFID_THEMEDATE), CCHMAXPATH, filename);
                    if (strlen(filename) == 0)
                    {
                        PSUTErrorFunc(hwnd, "Error", "ThemeDlgProc", "Please Enter a Theme Date", 0UL);
                        break;
                    }
                    //Make sure a ThemeVersion has been Specified
                    WinQueryWindowText(WinWindowFromID(hwndChild, EFID_THEMEVERSION), CCHMAXPATH, filename);
                    if (strlen(filename) == 0)
                    {
                        PSUTErrorFunc(hwnd, "Error", "ThemeDlgProc", "Please Enter a Theme Version", 0UL);
                        break;
                    }
                    //Make sure a filename has been specified
                    WinQueryWindowText(WinWindowFromID(hwndChild, EFID_THEMEFILE), CCHMAXPATH, filename);
                    if (strlen(filename) == 0)
                    {
                        PSUTErrorFunc(hwnd, "Error", "ThemeDlgProc", "Please Enter a Theme Filename", 0UL);
                        break;
                    }

                    //Open the output file for writing.
                    fp = fopen(filename, "w");
                    if (fp == NULL)
                    {
                        PSUTErrorFunc(hwnd, "Error", "ThemeDlgProc",
                           "Error Opening the theme file for writing", 0UL);
                        break;
                    }

                    fprintf(fp, ";Theme File Generated by CandyBarZ ThemeBuilder\n\n");

                    //hwndChild should already be the handle of the INFO page
                    while (hwndChild != NULLHANDLE)
                    {
                        //send a CBZ_SAVE message to each of the notebook pages.
                        //each page will save it's info to the (FILE *) pased in as mp1
                        WinSendMsg(hwndChild, CBZ_SAVE, MPFROMP(fp), (MPARAM) 0);
                        pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                        if (pageID == 0)
                            hwndChild = NULLHANDLE;
                        else
                            hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND, MPFROMLONG(pageID), MPFROMLONG(0));
                    }
                    //close the output file
                    fclose(fp);
                    //inform that the save was successful
                    PSUTErrorFunc(hwnd, "Info", "ThemeDlgProc", "Save Complete...", 0UL);
                }
                break;

                case PBID_PREVIEW:
                {

                    HWND hwndChild, hwndNotebook;
                    ULONG pageID;

                    //if the PreviewDialog hasn't been loaded, Load it here.
                    if (hwndPreview == NULLHANDLE)
                    {
                        hwndPreview = WinLoadDlg(HWND_DESKTOP,
                                                 hwnd,
                                                 WinDefDlgProc,  //PreviewDlgProc,
                                                  NULLHANDLE,
                                                 DLGID_PREVIEW,
                                                 NULL);
                    }
                    else
                        WinSetFocus(HWND_DESKTOP, hwndPreview);

                    hwndNotebook = WinWindowFromID(hwnd, NBID_SETUP);
                    //find the hwnd of the Info Page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, MPFROMLONG(0), MPFROM2SHORT(BKA_FIRST, 0));
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND, MPFROMLONG(pageID), MPFROMLONG(0));

                    //hwndChild should already be the handle of the INFO page
                    while (hwndChild != NULLHANDLE)
                    {
                        //send a CBZ_PREVIEW message to each of the notebook pages.
                        //each page will set the appropriate pointer in the preview window.
                        WinSendMsg(hwndChild, CBZ_PREVIEW, MPFROMHWND(hwndPreview), (MPARAM) 0);
                        pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                        if (pageID == 0)
                            hwndChild = NULLHANDLE;
                        else
                            hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND, MPFROMLONG(pageID), MPFROMLONG(0));
                    }


                    //make the PreviewWindow visable
                    WinEnableWindowUpdate(hwndPreview, FALSE);
                    WinShowWindow(hwndPreview, TRUE);
                }
                break;

                case PBID_OPEN:
                {
                    HWND hwndChild, hwndNotebook;
                    ULONG pageID;

                    FILEDLG filedlg;
                    //popup file change dialog!
                    memset(&filedlg, 0, sizeof(FILEDLG));  // init filedlg struct

                    filedlg.cbSize = sizeof(FILEDLG);
                    filedlg.fl = FDS_OPEN_DIALOG;
                    filedlg.pszTitle = "Choose an theme";
                    strcpy(filedlg.szFullFile, "*.cbz");

                    if (WinFileDlg(HWND_DESKTOP, hwnd, &filedlg) == NULLHANDLE)
                    {
                        PSUTErrorFunc(hwnd, "Error", "ThemeDlgProc",
                           "Error Opening the File Dialog", 0UL);
                        return (FALSE);
                    }

                    if (filedlg.lReturn != DID_OK)  // selected one

                    {
                        //cancel selected so do nothing
                        break;
                    }

                    hwndNotebook = WinWindowFromID(hwnd, NBID_SETUP);

                    //get the hwnd of the Info page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, MPFROMLONG(0), MPFROM2SHORT(BKA_FIRST, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND, MPFROMLONG(pageID), MPFROMLONG(0));
                    //set the ThemeFileName
                    WinSetWindowText(WinWindowFromID(hwndChild, EFID_THEMEFILE), filedlg.szFullFile);

                    //process the themeFile, setup the notebook pages
                    OpenThemeInfo(hwndChild, filedlg.szFullFile);

                    //Get the hwnd of the System setup  page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "SYSTEM");

                    //Get the hwnd of the Titlebar page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "TITLEBAR");
                    //Get the hwnd of the Frame page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "FRAME");

                    //Get the hwnd of the Frame page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "FRAMEBRDR");

                    //Get the hwnd of the PushButton page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "PUSHBUTTON");

                    //Get the hwnd of the RadioButton page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "RADIOBUTTON");

                    //Get the hwnd of the CheckBox page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "CHECKBOX");

                    //Get the hwnd of the CheckBox page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "MINMAX");

                    //Get the hwnd of the CheckBox page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "MENU");

                    //Get the hwnd of the page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "NOTEBOOK");

                    //Get the hwnd of the page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "SCROLLBAR");

                    //Get the hwnd of the page
                    pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
                             MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND,
                                         MPFROMLONG(pageID), MPFROMLONG(0));
                    OpenThemeClass(hwndChild, filedlg.szFullFile,
                                   ((DLGDATA *) WinQueryWindowPtr(hwndChild, QWL_USER))->pCBZData,
                                   "CONTAINER");

                    //Add calls for the remaining window classes here, changing the last parameter.
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
                sControlId = WinQueryWindowUShort(hwndFocus, QWS_ID);

                switch(sControlId)
                {
                    case PBID_SAVE:
                    {
                        sHelpId = HLPIDX_SAVE;
                    }
                    break;

                    default:
                    {
                        sHelpId = HLPIDX_SETUP;
                    }
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

        case WM_DESTROY:
        {
            HWND hwndChild, hwndNotebook;
            ULONG pageID;

            hwndNotebook = WinWindowFromID(hwnd, NBID_SETUP);
            //find the hwnd of the Info Page
            pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, MPFROMLONG(0), MPFROM2SHORT(BKA_FIRST, 0));
            pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
            hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND, MPFROMLONG(pageID), MPFROMLONG(0));

            //hwndChild should already be the handle of the INFO page
            while (hwndChild != NULLHANDLE)
            {
                //send a CBZ_NOPREVIEW message to each of the notebook pages.
                //each page will set the appropriate pointer in the preview window.
                WinSendMsg(hwndChild, CBZ_NOPREVIEW, MPFROMHWND(hwndPreview), (MPARAM) 0);
                pageID = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID, MPFROMLONG(pageID), MPFROM2SHORT(BKA_NEXT, 0));
                if (pageID == 0)
                    hwndChild = NULLHANDLE;
                else
                    hwndChild = (HWND) WinSendMsg(hwndNotebook, BKM_QUERYPAGEWINDOWHWND, MPFROMLONG(pageID), MPFROMLONG(0));
            }



            // clean up
            hwndHelp = WinQueryHelpInstance(hwnd);

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





