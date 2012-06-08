
/******************************************************************************

  Copyright Netlabs, 1998-2000 

  http://www.netlabs.org

  (C) E. Norman, C. Wohlgemuth

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

#define INCL_WINWORKPLACE
#include "plugin.h"
#include <stdio.h>

char tmpText[100];
LONG lPres;

//Returns the PLUGIN_INFO structure for the plugin
void CBZPluginInfo(PLUGIN_INFO * pluginInfo)
{
    strcpy(pluginInfo->PluginName, "CandyBarZ System colors and fonts");
    strcpy(pluginInfo->PluginAuthor, "CandyBarZ Team @ OS/2 Netlabs");
    strcpy(pluginInfo->PluginDate, "Sep. 21, 2000");
    pluginInfo->lWindowTypes =  CBZ_SYSTEM ;
    return;
}

//called to allocate memory and setup initial settings
BOOL CBZInit(HINI hIni, char szName[], char szClass[], PVOID * pData)
{
    PLUGINSHARE *pPluginShare;
    PLUGINSHARE *pPluginData;
    char szReadName[128];
    char szShareName[64];
    ULONG szData;

    /* if(*pData!=NULL)
       DosBeep(100,1300);*/

    // alloc window words for the plugin's data
    if ((DosAllocMem((PPVOID) (pData),
                     sizeof(PLUGINSHARE),
                     PAG_READ | PAG_WRITE | PAG_COMMIT)) != NO_ERROR)
    {
        return FALSE;
    }

    //initialize the plugin's data to zeros.
    memset(*pData, 0, sizeof(PLUGINSHARE));

    return TRUE;
}

//change current settings.
BOOL CBZApply(char szClass[], KEY_VALUE * kv, int count, int enabledState, PVOID pData)
{
    int i;
    PLUGINSHARE *pPluginShare;
    char szShareName[32];
    char szSaveName[32];
    BOOL bDefaultApply = FALSE;
    LONG lSysColors[41];
    int a;
    char *chrKey;
    HINI hIni;
    char szProfile[CCHMAXPATH];
    
    pPluginShare = (PLUGINSHARE *) pData;

    // query location of candybarz profile
    if (PrfQueryProfileString(HINI_USERPROFILE,
                               "CandyBarZ",
                               "Profile",
                               NULL,
                               szProfile,
                               sizeof(szProfile)))
      if ((hIni = PrfOpenProfile(NULLHANDLE, szProfile)) == NULLHANDLE)
        {
          return (FALSE);
        }

    //loop through each key/value pair and set appropriate parameters
    for (i = 0, a=0; i < count; i++)
      {
        /* Save PM_Colors in candybarz.ini for later use in Save function */
        if(!strncmp(kv[i].key,"PMColors",8)) {
          chrKey=kv[i].key;
          chrKey+=8;
          PrfWriteProfileString(hIni,"PM_Colors",chrKey,kv[i].value);
        }
        
        else if (strcmp(kv[i].key, "SysColor") == 0)
          {
            //StringToLong() returns 0 on error...which is as good as any value for an error condition?
            lSysColors[a]=StringToLong(kv[i].value);
            a++;
          }
        else if(!strncmp(kv[i].key,"PMSystemFonts",13)) {
          chrKey=kv[i].key;
          chrKey+=13;
          PrfWriteProfileString(hIni,"PMSystemFonts",chrKey,kv[i].value);
        }
      } //end for loop

    //if null, this is a default apply, so set syscolors.
    if (pPluginShare == NULL)
    {
      /* We do not reach this if in theme builder */

      if(a==SYSCLR_CSYSCOLORS)
        WinSetSysColors(HWND_DESKTOP,LCOL_PURECOLOR,LCOLF_CONSECRGB,SYSCLR_SHADOWHILITEBGND, a, lSysColors);/* Set all colors */
      /* At the moment no setting of fonts */
      WinBroadcastMsg(HWND_DESKTOP,WM_PRESPARAMCHANGED,MPFROMLONG(PP_FONTNAMESIZE),0,BMSG_POST);
    }
    else
      {

        pPluginShare->bActiveEnabled = TRUE;
      }

    if (hIni)
      PrfCloseProfile(hIni);

    return TRUE;
}

//save current settings to the given ini file
BOOL CBZSave(HINI hIni, char szClass[], PVOID pData, char szCustomName[])
{
    PLUGINSHARE *pPluginShare;
    //    PLUGINSHARE plugin;

    char keyValue[CCHMAXPATH];
    char *ptrMem;
    ULONG ulSize;
    
    char  *key;

   
    /* Query size of buffer to hold all key names */
    if(!PrfQueryProfileSize(hIni,"PM_Colors",NULL,&ulSize)) {
      return FALSE;
    }
    
    DosAllocMem((PVOID*)&ptrMem,ulSize,PAG_COMMIT|PAG_READ|PAG_WRITE);

   
    if(!ptrMem) {
      return FALSE;
    }

    /* Get all key names */
    if(!PrfQueryProfileString(hIni,"PM_Colors",NULL,NULL,ptrMem,ulSize)) {
      DosFreeMem(ptrMem);
      return FALSE;
    }
    key=ptrMem;
    

    while(*key!=0 && key!=NULL)
      {
        PrfQueryProfileString(hIni,"PM_Colors",key,NULL,keyValue,sizeof(keyValue));
        PrfWriteProfileString(HINI_USERPROFILE,"PM_Colors",key,keyValue);        
        key=strrchr(key,0);
        key++;
      }

    DosFreeMem(ptrMem);

    /* Query size of buffer to hold all key names for fonts */
    if(!PrfQueryProfileSize(hIni,"PM_SystemFonts",NULL,&ulSize)) {
      return FALSE;
    }
    
    DosAllocMem((PVOID*)&ptrMem,ulSize,PAG_COMMIT|PAG_READ|PAG_WRITE);

   
    if(!ptrMem) {
      return FALSE;
    }

    /* Get all key names */
    if(!PrfQueryProfileString(hIni,"PM_SystemFonts",NULL,NULL,ptrMem,ulSize)) {
      DosFreeMem(ptrMem);
      return FALSE;
    }
    key=ptrMem;
    

    while(*key!=0 && key!=NULL)
      {
        PrfQueryProfileString(hIni,"PM_SystemFonts",key,NULL,keyValue,sizeof(keyValue));
        PrfWriteProfileString(HINI_USERPROFILE,"PM_SystemFonts",key,keyValue);        
        key=strrchr(key,0);
        key++;
      }

    DosFreeMem(ptrMem);

#if 0
    //write pPluginShare to the ini file!
    strcpy(szSaveName, szClass);
    strcat(szSaveName, "_SYSCOLORPlugin");
    plugin.bActiveEnabled=TRUE;
    PrfWriteProfileData(hIni,
                        "UserOptionsData",
                        szSaveName,
                        &plugin,
                        sizeof(PLUGINSHARE));
#endif
    return (TRUE);
}

//cleanup resources generated by CBZInit
BOOL CBZDestroy(HWND hwnd, PVOID pData)
{
    PLUGINSHARE *pPluginData;

    pPluginData = (PLUGINSHARE *) pData;

    if (pPluginData == NULL)
        return (TRUE);

    DosFreeMem(pPluginData);
    return (TRUE);
}

//Draw the Plugin effect into the given presentation space
BOOL CBZPluginRender(PSPAINT * pPaint, PVOID pData, short sActive)
{
    return FALSE;
}

//Called before the default wnd procedure if you wish to provide additional
//  functionality.  (i.e. modify what happens with mouse clicks, drag/drop, etc...
BOOL CBZPluginWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2,
                        PVOID pData, PFNWP pfnOrigWndProc, MRESULT *rc)
{

    return FALSE; //false indicates that we haven't processed the message!

}

BOOL saveCurrentColors(char * fileName)
{
  FILE *file;
  int count;
  ULONG ulSize;
  char *ptrMem;
  char  *key;
  char keyValue[CCHMAXPATH];  
  
  if((file=fopen(fileName,"w"))==NULL)
    return FALSE;
  
  /* Write the Head of the skript file */
  count=fwrite(REXXHEAD,sizeof(char),sizeof(REXXHEAD),file);


  /* Query System Colors */

  /* Query size of buffer to hold all key names */
  if(!PrfQueryProfileSize(HINI_USERPROFILE,"PM_Colors",NULL,&ulSize)) {
    fclose(file);
    return FALSE;
  }

  ptrMem=malloc(ulSize);

  if(!ptrMem) {
    fclose(file);
    return FALSE;
  }
  /* Get all key names */
  if(!PrfQueryProfileString(HINI_USERPROFILE,"PM_Colors",NULL,NULL,ptrMem,ulSize)) {
    fclose(file);
    free(ptrMem);
    return FALSE;
  }
  key=ptrMem;
  
  
  while(strlen(key)!=0 && key!=NULL)
    {
      PrfQueryProfileString(HINI_USERPROFILE,"PM_Colors",key,NULL,keyValue,sizeof(keyValue));
      fwrite(CALLSYSINI1,sizeof(char),strlen(CALLSYSINI1),file);
      fwrite(key,sizeof(char),strlen(key),file);
      fwrite(CALLSYSINI2,sizeof(char),strlen(CALLSYSINI2),file);
      fwrite(keyValue,sizeof(char),strlen(keyValue),file);
      fwrite(CALLSYSINI3,sizeof(char),strlen(CALLSYSINI3),file);
      key=strrchr(key,0);
      key++;
    }
  free(ptrMem);

  /* Query System fonts */ 
  /* Query size of buffer to hold all key names */
  if(!PrfQueryProfileSize(HINI_USERPROFILE,"PM_SystemFonts",NULL,&ulSize)) {
    fclose(file);
    return FALSE;
  }

  ptrMem=malloc(ulSize);

  if(!ptrMem) {
    fclose(file);
    return FALSE;
  }
  /* Get all key names */
  if(!PrfQueryProfileString(HINI_USERPROFILE,"PM_SystemFonts",NULL,NULL,ptrMem,ulSize)) {
    fclose(file);
    free(ptrMem);
    return FALSE;
  }
  key=ptrMem;
  
  while(strlen(key)!=0 && key!=NULL)
    {
      PrfQueryProfileString(HINI_USERPROFILE,"PM_SystemFonts",key,NULL,keyValue,sizeof(keyValue));
      fwrite(CALLSYSINI4,sizeof(char),strlen(CALLSYSINI4),file);
      fwrite(key,sizeof(char),strlen(key),file);
      fwrite(CALLSYSINI2,sizeof(char),strlen(CALLSYSINI2),file);
      fwrite(keyValue,sizeof(char),strlen(keyValue),file);
      fwrite(CALLSYSINI3,sizeof(char),strlen(CALLSYSINI3),file);
      key=strrchr(key,0);
      key++;
    }
  free(ptrMem);
  
  /* Close skript */
  fclose(file);

  return TRUE;
};

//A Dlg procedure if the plugin has selectable settings.
MRESULT EXPENTRY CBZPluginSetupDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    short sControlId;
    PLUGINSHARE *pPluginData;
 	HOBJECT  hObject;
    char installDir[CCHMAXPATH];
    FILEDLG filedlg;
    HWND hwndDlg;

    switch (msg)
    {
    case WM_INITDLG:
      {
        if ((pPluginData = (PLUGINSHARE *) mp2) == NULL)
          {
            //Error msg..
            WinDismissDlg(hwnd, DID_ERROR);
            return (MRFROMLONG(FALSE));
          }
            WinSetWindowPtr(hwnd, QWL_USER, pPluginData);  // store window words


            WinSetWindowText(WinWindowFromID(hwnd,MLEID_SYSCOLOR),SYSCOLORTEXT);            

      }
      break;
    case WM_COMMAND:
      {
        sControlId = COMMANDMSG(&msg)->cmd;
        
        switch (sControlId)
              {

                case PBID_OK:
                {
                    ULONG attrFound;

                    if ((pPluginData = (PLUGINSHARE *) WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
                    {
                        //Error message?
                        break;
                    }

                    //update!
                    WinDismissDlg(hwnd, PBID_OK);
                }
                break;

                case PBID_CANCEL:
                {
                    //don't update shared Memory!
                    WinDismissDlg(hwnd, PBID_CANCEL);
                }
                break;
               case PBID_SAVE:
                {
                  //popup file change dialog!
                  memset(&filedlg, 0, sizeof(FILEDLG));  // init filedlg struct
                  
                  filedlg.cbSize = sizeof(FILEDLG);
                  filedlg.fl = FDS_SAVEAS_DIALOG|FDS_CENTER;
                  filedlg.pszTitle = "Choose a name for Rxx skript";
                  strcpy(filedlg.szFullFile, "recolor.cmd");
                  
                  if ((hwndDlg = WinFileDlg(HWND_DESKTOP, hwnd, &filedlg)) == NULLHANDLE)
                    {
                      return (FALSE);
                    }
                  
                  if (filedlg.lReturn != DID_OK)  // selected one
                    
                    {
                      break;
                    }
                  if(saveCurrentColors(filedlg.szFullFile))
                    WinMessageBox(HWND_DESKTOP,
                                  hwnd,         
                                  "Rexx skript to restore the current color setup succesfully created",          
                                  "Syscolor plugin",                      
                                  12345,            /* Window ID */
                                  MB_OK |
                                  MB_MOVEABLE |
                                  MB_INFORMATION |
                                  MB_OK);                  /* Style     */
                  else
                    WinMessageBox(HWND_DESKTOP,
                                  hwnd,         
                                  "Can't create the Rexx skript. Current color setup will be lost when changed with the layout palette!",          
                                  "Syscolor plugin",                      
                                  12345,            /* Window ID */
                                  MB_OK |
                                  MB_MOVEABLE |
                                  MB_WARNING |
                                  MB_DEFBUTTON1);                  /* Style     */
                }
                break;

              case PBID_LAYOUTPALETTE:
                {
                  // Open the colorpalette 
                  hObject=WinQueryObject("<WP_HIRESCLRPAL>");
                  if((hObject=WinQueryObject("<WP_SCHPAL96>"))!=NULLHANDLE) {
                    WinOpenObject(hObject, OPEN_DEFAULT ,TRUE);
                  }
                  else {//Error, can't open the palette
                    /*  Show an error msg.						   */
                    WinMessageBox(HWND_DESKTOP,
                                  hwnd,         
                                  "Can't open layout palette",          
                                  "Syscolor plugin",                      
                                  12345,            /* Window ID */
                                  MB_OK |
                                  MB_MOVEABLE |
                                  MB_ICONEXCLAMATION |
                                  MB_DEFBUTTON1);                  /* Style     */
                  }
                }
                break;
                    
                    

            }
            return ((MPARAM) 0);
        }

        case WM_CONTROL:
        {
            switch (SHORT1FROMMP(mp1))
            {

            }                   //end switch

            return ((MRESULT) 0);
        }                       //end WM_CONTROL

        break;

    }                           //end switch

    return (WinDefDlgProc(hwnd, msg, mp1, mp2));
}

BOOL CBZWriteAttributes(char *retStr, PVOID pData)
{
    PLUGINSHARE *pPluginData;
    char keyValue[CCHMAXPATH];
    ULONG ulSize;
    char *ptrMem;
    char  *key;
    char user[CCHMAXPATH];
    char system[CCHMAXPATH];
    int a;


    /* Query size of buffer to hold all key names */
    if(!PrfQueryProfileSize(HINI_USERPROFILE,"PM_Colors",NULL,&ulSize))
      return FALSE;

    ptrMem=malloc(ulSize);

    if(!ptrMem)
      return FALSE;

    /* Get all key names */
    if(!PrfQueryProfileString(HINI_USERPROFILE,"PM_Colors",NULL,NULL,ptrMem,ulSize)) {
      free(ptrMem);
      return FALSE;
    }
    key=ptrMem;
    
    /* Put this at the top */
    AddEnabledAttribute(retStr, 0);

    while(strlen(key)!=0 && key!=NULL)
      {
        PrfQueryProfileString(HINI_USERPROFILE,"PM_Colors",key,NULL,keyValue,sizeof(keyValue));
        strcpy(system,"PMColors");
        strcat(system,key);
        AddStringAttribute(retStr,system,keyValue);
        key=strrchr(key,0);
        key++;
      }
    free(ptrMem);

    for(a=SYSCLR_SHADOWHILITEBGND;a<SYSCLR_SHADOWHILITEBGND+SYSCLR_CSYSCOLORS;a++) {
      AddLongAttribute(retStr,"SysColor", WinQuerySysColor(HWND_DESKTOP,a,0));
    }

    /* System fonts */
    /* Query size of buffer to hold all key names */
    if(!PrfQueryProfileSize(HINI_USERPROFILE,"PM_SystemFonts",NULL,&ulSize))
      return FALSE;

    ptrMem=malloc(ulSize);

    if(!ptrMem)
      return FALSE;

    /* Get all key names */
    if(!PrfQueryProfileString(HINI_USERPROFILE,"PM_SystemFonts",NULL,NULL,ptrMem,ulSize)) {
      free(ptrMem);
      return FALSE;
    }
    key=ptrMem;
    
    while(strlen(key)!=0 && key!=NULL)
      {
        PrfQueryProfileString(HINI_USERPROFILE,"PM_SystemFonts",key,NULL,keyValue,sizeof(keyValue));
        strcpy(system,"PMSystemFonts");
        strcat(system,key);
        AddStringAttribute(retStr,system,keyValue);
        key=strrchr(key,0);
        key++;
      }
    free(ptrMem);


    return (TRUE);

};



