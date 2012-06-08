/* $Id: infodlgproc.c,v 1.2 2000/10/18 14:53:03 cwohlgemuth Exp $ */

#include "themeBuilder.h"

//Dialog Window Procedure for the Info Notebook page
MRESULT EXPENTRY InfoDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    ULONG       ulRc;
    HWND        hwndHelp;
    HWND        hwndFocus = WinQueryFocus( HWND_DESKTOP );
    short       sControlId;
    short       sHelpId;

    switch (msg)
    {
        case WM_INITDLG:
        {
            char dateStr[32];
            DATETIME dateTime = {0};
            char months[][4] = {"Jan", "Feb", "Mar",
                                "Apr", "May", "Jun",
                                "Jul", "Aug", "Sep",
                                "Oct", "Nov", "Dec"};
            WinSendMsg(WinWindowFromID(hwnd, EFID_THEMENAME), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, EFID_THEMEAUTHOR), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, EFID_THEMEDATE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, EFID_THEMEVERSION), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);
            WinSendMsg(WinWindowFromID(hwnd, EFID_THEMEFILE), EM_SETTEXTLIMIT, (MPARAM) CCHMAXPATH, (MPARAM) 0);

            DosGetDateTime(&dateTime);
            sprintf(dateStr, "%d %s, %d", dateTime.day, months[dateTime.month - 1], dateTime.year);
            WinSetWindowText(WinWindowFromID(hwnd, EFID_THEMEDATE), dateStr);
        }
        break;

        case CBZ_SAVE:
        {
            FILE *fp;
            char tmpText[1024];
            fp = (FILE *) PVOIDFROMMP(mp1);

            fprintf(fp, "[INFO]\n");
            fprintf(fp, "{\n");

            WinQueryWindowText(WinWindowFromID(hwnd, EFID_THEMENAME), 1024, tmpText);
            fprintf(fp, "  Name = \"%s\"\n", tmpText);
            WinQueryWindowText(WinWindowFromID(hwnd, EFID_THEMEAUTHOR), 1024, tmpText);
            fprintf(fp, "  Author = \"%s\"\n", tmpText);
            WinQueryWindowText(WinWindowFromID(hwnd, EFID_THEMEDATE), 1024, tmpText);
            fprintf(fp, "  Date = \"%s\"\n", tmpText);
            WinQueryWindowText(WinWindowFromID(hwnd, EFID_THEMEVERSION), 1024, tmpText);
            fprintf(fp, "  Version = \"%s\"\n", tmpText);

            fprintf(fp, "}\n\n");
        }
        break;

        case WM_HELP:
        {
            // display help
            hwndHelp = WinQueryHelpInstance(hwnd);
            if (hwndHelp)
            {
                sControlId = WinQueryWindowUShort(hwndFocus, QWS_ID);

                switch (sControlId)
                {
                    case EFID_THEMENAME:
                    case EFID_THEMEAUTHOR:
                    case EFID_THEMEDATE:
                    case EFID_THEMEVERSION:
                    case EFID_THEMEFILE:
                    default:
                    {
                        sHelpId = HLPIDX_INFOPAGE;
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
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}
