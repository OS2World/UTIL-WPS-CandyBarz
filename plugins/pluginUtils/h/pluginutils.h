/* $Id: pluginutils.h,v 1.6 2000/10/04 20:03:57 cwohlgemuth Exp $ */

#ifndef PLUGINUTILS_H
#define PLUGINUTILS_H

#define INCL_PM
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define OPEN_DEFAULT       0

#define SCALE_LARGEST       0x00000001L
#define SCALE_SMALLEST      0x00000002L
#define SCALE_HORIZONTAL    0x00000004L
#define SCALE_VERTICAL      0x00000008L


#define CBZ_TITLEBAR          0x00000001UL
#define CBZ_FRAME             0x00000002UL
#define CBZ_PUSHBUTTON        0x00000004UL
#define CBZ_RADIOBUTTON       0x00000008UL
#define CBZ_CHECKBOX          0x00000010UL
#define CBZ_FRAMEBRDR         0x00000020UL
#define CBZ_MINMAX            0x00000040UL
#define CBZ_MENU              0x00000080UL
#define CBZ_NOTEBOOK          0x00000100UL
#define CBZ_SCROLLBAR         0x00000200UL
#define CBZ_CONTAINER         0x00000400UL
#define CBZ_SYSTEM            0x00000800UL

#define DLGID_COLORCHANGE           300  // dialog for setting color components, soon to go away
#define STID_RED                    301  // red text
#define SLID_RED                    302  // red slider
#define STID_GREEN                  303  // green text
#define SLID_GREEN                  304  // green slider
#define STID_BLUE                   305  // blue text
#define SLID_BLUE                   306  // blue slider
#define VSID_COLORPREVIEW           307  // preview window
#define PBID_COLOROK                308  // ok button
#define PBID_COLORCANCEL            309  // cancel button
#define SPID_RED                    310  // red spinner
#define SPID_GREEN                  311  // green spinner
#define SPID_BLUE                   312  // blue spinner

// for passing/receiving color to/from color change dlg
typedef struct
{
    USHORT cbFix;               // size
    ULONG ulRgb;                // color
}
CLRCHANGE;


// structure to return information about the plugin
typedef struct
{
    char PluginName[64];
    char PluginAuthor[64];
    char PluginDate[16];
    long lWindowTypes;
}
PLUGIN_INFO;

typedef struct
{
    char *key;
    char *value;
}
KEY_VALUE;

// structure used for painting
typedef struct
{
    HWND hwnd;                  // hwnd to draw to
    HPS hps;                    // hps to draw in
    HPS hpsWin;
    RECTL rectlWindow;          // window rectangle
    RECTL rectlUpdate;          // update rectangle
    RECTL rectlText;            // text Rectangle
    char szTitleText[CCHMAXPATH];      // titlebar text
}
PSPAINT;

BOOL CBZGetColor(HWND hwnd, long *lColor);
BOOL CBZSaveDefault(HWND hwnd, PVOID pData, long cbData, char pluginName[], char sharedMemName[]);
BOOL CBZSaveCustom(HWND hwnd, PVOID pData, long cbData, char pluginName[], char exeName[], BOOL bActive);
MRESULT EXPENTRY ColorChangeDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

int GetInstallDir(char * installDir, int size);
long StringToLong(char str[]);
void AddEnabledAttribute(char retStr[], int value);
void AddStringAttribute(char retStr[], char key[], char value[]);
void AddLongAttribute(char retStr[], char key[], int value);

HBITMAP PSBMBLTToSize(HAB hab, HPS hps, HBITMAP hbm, float fScale, ULONG ulOptions);


#endif //PLUGINTUILS_H
