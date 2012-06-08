/* $Id: pluginutils.c,v 1.3 2000/10/04 20:01:40 cwohlgemuth Exp $ */

#include "pluginUtils.h"

#if 0
/* This function launches the wrapper <wrapperExe>         */
/*  with the params given as a hidden vio-session          */
/* in PSZ parameter.                                       */

ULONG launchPMWrapper(PSZ parameter, PSZ wrapperExe, PSZ pszTitle)
{
  STARTDATA startData={0};
  APIRET rc;
  PID pid;
  ULONG ulSessionID=0;
  char chrLoadError[CCHMAXPATH];
  
  memset(&startData,0,sizeof(startData));
  startData.Length=sizeof(startData);
 
  startData.Related=SSF_RELATED_CHILD;
  startData.FgBg=SSF_FGBG_BACK;
  startData.TraceOpt=SSF_TRACEOPT_NONE;
  startData.PgmTitle=pszTitle;
    
  startData.PgmName=wrapperExe;
  startData.InheritOpt=SSF_INHERTOPT_SHELL;
  startData.SessionType=SSF_TYPE_DEFAULT;
  startData.PgmControl=SSF_CONTROL_INVISIBLE;
  startData.InitXPos=0;
  startData.InitYPos=0;
  startData.InitXSize=0;
  startData.InitYSize=0;
  startData.ObjectBuffer=chrLoadError;
  startData.ObjectBuffLen=(ULONG)sizeof(chrLoadError);

  startData.PgmInputs=parameter;

  rc=DosStartSession(&startData,&ulSessionID,&pid);
   
  return 0;   
}
#endif


int GetInstallDir(char * installDir, int size)
{

  *installDir=0;

  // query location of candybarz profile
  if (!PrfQueryProfileString(HINI_USERPROFILE,
                             "CandyBarZ",
                             "BasePath",
                             NULL,
                             installDir,
                             size))
    {
      return (0);
    }

  return 1;  

}

long StringToLong(char str[])
{
    int i, len;
    long result = 0;
    len = strlen(str);

    for (i = 0; i < len; i++)
    {
        //make sure the string contains only digits. If not return zero
        if ((str[i] < '0') || (str[i] > '9'))
            return 0;

        result += (str[i] - '0') * (long) (pow(10.0, (double) (len - i - 1)) + 0.5);
    }
    return result;
}

void AddEnabledAttribute(char retStr[], int value)
{
    char tmpText[32];
    strcat(retStr, "    EnabledForState = ");
    strcat(retStr, itoa(value, tmpText, 10));
    strcat(retStr, "\n");
}

void AddStringAttribute(char retStr[], char key[], char value[])
{
    char tmpText[32];
    strcat(retStr, "    ");
    strcat(retStr, key);
    strcat(retStr, " = \"");
    strcat(retStr, value);
    strcat(retStr, "\"\n");
}

void AddLongAttribute(char retStr[], char key[], int value)
{
    char tmpText[32];
    strcat(retStr, "    ");
    strcat(retStr, key);
    strcat(retStr, " = \"");
    strcat(retStr, itoa(value, tmpText, 10));
    strcat(retStr, "\"\n");
}

BOOL CBZGetColor(HWND hwnd, long *lColor)
{
  /*    CLRCHANGE clrCh;
        HMODULE hMod;
        clrCh.cbFix = (USHORT) sizeof(clrCh);
        clrCh.ulRgb = *lColor;
        
        DosQueryModuleHandle("CBZUTIL", &hMod);
        //popup the colorchange dialog
        if (WinDlgBox(HWND_DESKTOP,
        hwnd,
        ColorChangeDlgProc,
        hMod,
        DLGID_COLORCHANGE,
        &clrCh) != PBID_COLOROK)
        {
        return (FALSE);
        }
        
        //update the color
        *lColor = clrCh.ulRgb;*/
  return (TRUE);
}

BOOL CBZSaveDefault(HWND hwnd, PVOID pData, long cbData, char pluginName[], char sharedMemName[])
{
/*   HINI  hIni;
   char  szProfile[ CCHMAXPATH ];
   HAB   hab = WinQueryAnchorBlock(hwnd);
   char  szKey[ CCHMAXPATH ];
   PVOID pPluginShare;

   // open profile
   if( !PrfQueryProfileString(  HINI_USERPROFILE,
   "CandyBarZ",
   "Profile",
   NULL,
   szProfile,
   sizeof( szProfile ) ) )
   {
   //copy to some error string here
   return ( FALSE );
   }

   if( ( hIni = PrfOpenProfile( hab, szProfile ) ) == NULLHANDLE )
   {
   //copy to some error string here
   return ( FALSE );
   }

   // get shared mem
   if( DosGetNamedSharedMem( ( PPVOID )&pPluginShare, sharedMemName, PAG_READ ) != NO_ERROR )
   {
   //copy to some error string here
   PrfCloseProfile( hIni );
   return ( FALSE );
   }

   strcpy(szKey, pluginName);
   strcat(szKey, "Plugin");

   // write shared mem
   if( !PrfWriteProfileData(   hIni,
   "UserOptions",
   szKey,
   pPluginShare,
   sizeof(PLUGINSHARE) ))
   {
   //copy to some error string here
   DosFreeMem( pPluginShare);
   PrfCloseProfile( hIni );
   return ( FALSE );
   }

   // clean up
   DosFreeMem( pPluginShare );
   PrfCloseProfile( hIni );
 */
    return (TRUE);
}

BOOL CBZSaveCustom(HWND hwnd, PVOID pData, long cbData, char pluginName[], char exeName[], BOOL bActive)
{
/*
   HINI  hIni;
   char  szProfile[ CCHMAXPATH ];
   HAB   hab = WinQueryAnchorBlock(hwnd);
   char  szKey[ CCHMAXPATH ];

   // open profile
   if( !PrfQueryProfileString(  HINI_USERPROFILE,
   "CandyBarZ",
   "Profile",
   NULL,
   szProfile,
   sizeof( szProfile ) ) )
   {
   //copy something to an error string?
   return ( FALSE );
   }

   if( ( hIni = PrfOpenProfile( hab, szProfile ) ) == NULLHANDLE )
   {
   //copy something to an error string?
   return ( FALSE );
   }

   if (bActive)
   strcpy(szKey, "Active");
   else
   strcpy(szKey, "Inactive");
   strcat(szKey, pluginName);

   // write shared mem
   if( !PrfWriteProfileData(   hIni,
   szKey,
   exeName,
   pData,
   cbData ))
   {
   //copy something to an error string?
   DosFreeMem( pPluginShare);
   PrfCloseProfile( hIni );
   break;
   }

   // clean up
   DosFreeMem( pPluginShare );
   PrfCloseProfile( hIni );
 */
    return (TRUE);
}



HBITMAP PSBMBLTToSize(HAB hab, HPS hps, HBITMAP hbm, float fScale, ULONG ulOptions)
{
    BITMAPINFOHEADER2 bif2Before;
    BITMAPINFOHEADER2 bif2After;
    SIZEL slAfter;
    ULONG ulScreenWidth;
    ULONG ulScreenHeight;
    HDC hdcAfter;
    HPS hpsAfter;
    HBITMAP hbmAfter;
    POINTL aptl[4];
    float fAspect;
    HBITMAP hbmOld;

    if ((hbmOld = GpiSetBitmap(hps, hbm)) == HBM_ERROR)
    {
        return (NULLHANDLE);
    }

    memset(&bif2Before, 0, sizeof(bif2Before));  // get current info

    bif2Before.cbFix = sizeof(bif2Before);
    if (GpiQueryBitmapInfoHeader(hbm, &bif2Before) != TRUE)
    {
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    ulScreenWidth = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);  // screen width

    ulScreenHeight = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);  // screen height

    fAspect = (bif2Before.cx * 1.0) / (bif2Before.cy * 1.0);  // calculate aspect ratio

    if (ulOptions & SCALE_LARGEST)
    {
        if (bif2Before.cx > bif2Before.cy)
        {
            slAfter.cx = (LONG) (ulScreenWidth * fScale);
            slAfter.cy = (LONG) (slAfter.cx / fAspect);
        }
        else
        {
            slAfter.cy = (LONG) (ulScreenHeight * fScale);
            slAfter.cx = (LONG) (slAfter.cy * fAspect);
        }
    }
    else if (ulOptions & SCALE_SMALLEST)
    {
        if (bif2Before.cx < bif2Before.cy)
        {
            slAfter.cx = (LONG) (ulScreenWidth * fScale);
            slAfter.cy = (LONG) (slAfter.cx / fAspect);
        }
        else
        {
            slAfter.cy = (LONG) (ulScreenHeight * fScale);
            slAfter.cx = (LONG) (slAfter.cy * fAspect);
        }
    }
    else if (ulOptions & SCALE_HORIZONTAL)
    {
        slAfter.cx = (LONG) (ulScreenWidth * fScale);
        if (ulOptions & SCALE_VERTICAL)  // basically, screw the aspect

        {
            slAfter.cy = (LONG) (ulScreenHeight * fScale);
        }
        else
        {
            slAfter.cy = (LONG) (slAfter.cx / fAspect);
        }
    }
    else if (ulOptions & SCALE_VERTICAL)
    {
        slAfter.cy = (LONG) (ulScreenHeight * fScale);
        slAfter.cx = (LONG) (slAfter.cy * fAspect);
    }
    else
    {
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    if ((hdcAfter = DevOpenDC(hab, OD_MEMORY, "*", 0L, (PDEVOPENDATA) NULL, NULLHANDLE))
            == NULLHANDLE)
    {
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }
    if ((hpsAfter = GpiCreatePS(hab,
                                hdcAfter,
                                &slAfter,
           PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC)) == NULLHANDLE)
    {
        DevCloseDC(hdcAfter);
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    memset(&bif2After, 0, sizeof(bif2After));
    bif2After.cbFix = sizeof(bif2After);
    bif2After.cx = slAfter.cx;
    bif2After.cy = slAfter.cy;
    bif2After.cBitCount = bif2Before.cBitCount;
    bif2After.cPlanes = bif2Before.cPlanes;

    if ((hbmAfter = GpiCreateBitmap(hpsAfter,  // create bitmap
                                     (PBITMAPINFOHEADER2) & (bif2After),
                                    (ULONG) FALSE,
                                    NULL,
                                    NULL)) == NULLHANDLE)
    {
        GpiDestroyPS(hpsAfter);
        DevCloseDC(hdcAfter);
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    if (GpiSetBitmap(hpsAfter, hbmAfter) == HBM_ERROR)
    {
        GpiDeleteBitmap(hbmAfter);
        GpiDestroyPS(hpsAfter);
        DevCloseDC(hdcAfter);
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    aptl[0].x = 0;
    aptl[0].y = 0;
    aptl[1].x = slAfter.cx;
    aptl[1].y = slAfter.cy;
    aptl[2].x = 0;
    aptl[2].y = 0;
    aptl[3].x = bif2Before.cx;
    aptl[3].y = bif2Before.cy;

    if (GpiBitBlt(hpsAfter,
                  hps,
                  4L,
                  aptl,
                  ROP_SRCCOPY,
                  BBO_IGNORE) == GPI_ERROR)
    {
        GpiSetBitmap(hpsAfter, NULLHANDLE);
        GpiDeleteBitmap(hbmAfter);
        GpiDestroyPS(hpsAfter);
        DevCloseDC(hdcAfter);
        GpiSetBitmap(hps, hbmOld);
        return (NULLHANDLE);
    }

    GpiSetBitmapDimension(hbmAfter, &slAfter);

    GpiSetBitmap(hpsAfter, NULLHANDLE);
    GpiDestroyPS(hpsAfter);
    DevCloseDC(hdcAfter);
    GpiSetBitmap(hps, hbmOld);

    return (hbmAfter);
}














