/* $Id: themeReader.c,v 1.1 1999/06/15 20:48:00 ktk Exp $ */

#include <stdio.h>
#include <string.h>
#define INCL_DOS
#define INCL_DOSERRORS
#define INCL_PM
#include <os2.h>
#include "CandyBarZParser.h"
#include "PCandyBarZ.h"

typedef BOOL ( *_Optlink PARSEPFN )( HINI hIni, KEY_VALUE *kv, int count, int enabledState );

void forceRepaint(void)
{
   HATOMTBL            hAtomTbl = WinQuerySystemAtomTable();
   ATOM                Atom;

   // locate broadcast atom
   if( ( Atom = WinFindAtom( hAtomTbl, PSTB_COLORCHANGE_ATOM ) ) == 0 )
   {
      printf("Failed to locate the COLORCHANGE atom!\n");
      return;
   }


   // broadcast to desktop children
   if( !WinBroadcastMsg(   HWND_DESKTOP,
                           Atom,
                           ( MPARAM )0,
                           ( MPARAM )0,
                           BMSG_POST | BMSG_DESCENDANTS ) )
   {
      printf("Failed to Broadcast Atom!\n");
      return;
   }
   // broadcast to hidden windows
   WinBroadcastMsg(    HWND_OBJECT,
                       Atom,
                       ( MPARAM )0,
                       ( MPARAM )0,
                       BMSG_POST | BMSG_DESCENDANTS );


      printf("Broadcast Atom Complete!\n");

}

// try loading each plugin module.
// if opened sucessfully, call the plugin's CBZParseAttributes(hIni, keyValue, count) function
// the plugin is responsible for writing data to the ini file.
void test( PLUGIN_LIST *pList, char filename[] )
{
   HAB      hab = WinQueryAnchorBlock(HWND_DESKTOP);
   HINI     hIni;
   HMODULE  hMod;
   PARSEPFN pParseFunction;
   char     szError[32];
   char     szProfile[CCHMAXPATH +1];
   char     szPluginDir[CCHMAXPATH +1];
   char     szFile[CCHMAXPATH +1];
   char     *szBase;
   PSTBSHARE           *ptbShare;
   char     *tempPtr;
   int      i,j;

   // get profile name
   if( !PrfQueryProfileString(  HINI_USERPROFILE,
                               "CandyBarZ",
                               "Profile",
                               NULL,
                               szProfile,
                               CCHMAXPATH ) )
   {
   //error getting CandyBarZ profile.
      return;
   }

   // get profile name
   if( !PrfQueryProfileString(  HINI_USERPROFILE,
                               "CandyBarZ",
                               "BasePath",
                               NULL,
                               szPluginDir,
                               CCHMAXPATH ) )
   {
   //error getting CandyBarZ profile.
      return;
   }
   szBase = strdup(szPluginDir);
   strcat(szPluginDir, "plugins\\");
//   printf("Plugin Directory = %s\n\n", szPluginDir);

   if( ( hIni = PrfOpenProfile( hab, szProfile ) ) == NULLHANDLE )
   {
      return;
   }
/*
   // get it for writing, so let's hurry
   if( DosGetNamedSharedMem( ( PPVOID )&ptbShare, PSTB_SHARE, PAG_READ | PAG_WRITE )
         != NO_ERROR )
   {
      //couldn't get shared Memory
      return;
   }

   ptbShare->plugins.cbCount = pList->cbPlugins;
   strcpy(ptbShare->szThemeFilename, filename);
*/
   for (j = 0; j < pList->cbPlugins; j++)
   {
/*      strcpy(ptbShare->plugins.pluginDLL[j],
                  pList->pPluginAttributes[j].szPluginName);
*/
      printf(" Key/Value Pairs for ""%s""\n", pList->pPluginAttributes[j].szPluginName);
      printf("-=================================-\n");
      printf("-== This Plugin Enabled For: %2d ==-\n\n", pList->pPluginAttributes[j].enabledState);
      for ( i = 0; i < pList->pPluginAttributes[j].cbAttributes; i++ )
      {
         if (strstr(pList->pPluginAttributes[j].keyValue[i].value, "%CBZBaseDir%") != NULL)
         {
         //%CBZBaseDir% must be at the start of the string
            tempPtr = (char *)malloc(sizeof(char) * (strlen(szBase) + strlen(pList->pPluginAttributes[j].keyValue[i].value) - strlen("%CBZBaseDir%") + 1) );
            tempPtr[0] = '\0';
            strcat(tempPtr, szBase);
            sscanf(pList->pPluginAttributes[j].keyValue[i].value,  "%%CBZBaseDir%%\\%s", szProfile);
            strcat(tempPtr, szProfile);
            //replace %CBZBaseDir% with the base Directory
//            printf("tempPtr = %s\n", tempPtr);
            free( pList->pPluginAttributes[j].keyValue[i].value );
            pList->pPluginAttributes[j].keyValue[i].value = tempPtr;
         }
         printf("%-30s = %s\n", pList->pPluginAttributes[j].keyValue[i].key, pList->pPluginAttributes[j].keyValue[i].value);
      }
      printf("\n");

      //plugins might not be in LIBPATH, create full path here.
      _makepath(szFile, NULL, szPluginDir, pList->pPluginAttributes[j].szPluginName, "DLL");

//   printf("Plugin DLL = %s\n\n", szFile);


/*      if (DosLoadModule(szError, sizeof(szError), szFile, &hMod) == NO_ERROR)
      {
         if ( DosQueryProcAddr(hMod, 0, "CBZParseAttributes", (PFN *)&pParseFunction) == NO_ERROR )
         {
            if (pParseFunction == NULL)
            {
//               printf("function address is null!\n");
            }
            else
            {
               pParseFunction( hIni, (pList->pPluginAttributes[j].keyValue),
                            pList->pPluginAttributes[j].cbAttributes,
                            pList->pPluginAttributes[j].enabledState );

//               printf("Call To Plugin's procedure was sucessful!\n\n");
            }
         }
         else
         {
//            printf("Error Querying Procedure Address for CBZParseAttributes\n");
         }
         DosFreeModule(hMod);
      }
      else
      {
//         printf("Error loading plugin module\n");
      }
*/

   }

/*
   // write shared mem
   if( !PrfWriteProfileData(   hIni,
                               "UserOptions",
                               "DefaultPlugins",
                               ptbShare,
                               offsetof( PSTBSHARE, ulDataOffset ) ) )
   {
//      printf("Error writing profile data!\n");
   }

   free(szBase);
   DosFreeMem( ptbShare );
   PrfCloseProfile(hIni);
*/
}


void testButtons( PLUGIN_LIST *pList, char filename[] )
{
   HAB      hab = WinQueryAnchorBlock(HWND_DESKTOP);
   HINI     hIni;
   HMODULE  hMod;
   PARSEPFN pParseFunction;
   char     szError[32];
   char     szProfile[CCHMAXPATH +1];
   char     szPluginDir[CCHMAXPATH +1];
   char     szFile[CCHMAXPATH +1];
   char     *szBase;
   PSTBSHARE           *ptbShare;
   char     *tempPtr;
   int      i,j;

   for (j = 0; j < pList->cbPlugins; j++)
   {
      printf(" Key/Value Pairs for ""%s""\n", pList->pPluginAttributes[j].szPluginName);
      printf("-=================================-\n");
      printf("-== This Plugin Enabled For: %2d ==-\n\n", pList->pPluginAttributes[j].enabledState);
      for ( i = 0; i < pList->pPluginAttributes[j].cbAttributes; i++ )
      {
         if (strstr(pList->pPluginAttributes[j].keyValue[i].value, "%CBZBaseDir%") != NULL)
         {
         //%CBZBaseDir% must be at the start of the string
            tempPtr = (char *)malloc(sizeof(char) * (strlen(szBase) + strlen(pList->pPluginAttributes[j].keyValue[i].value) - strlen("%CBZBaseDir%") + 1) );
            tempPtr[0] = '\0';
            strcat(tempPtr, szBase);
            sscanf(pList->pPluginAttributes[j].keyValue[i].value,  "%%CBZBaseDir%%\\%s", szProfile);
            strcat(tempPtr, szProfile);
            //replace %CBZBaseDir% with the base Directory
//            printf("tempPtr = %s\n", tempPtr);
            free( pList->pPluginAttributes[j].keyValue[i].value );
            pList->pPluginAttributes[j].keyValue[i].value = tempPtr;
         }
         printf("%-30s = %s\n", pList->pPluginAttributes[j].keyValue[i].key, pList->pPluginAttributes[j].keyValue[i].value);
      }
      printf("\n");

   }

}


int main(int argc, char *argv[])
{
   PLUGIN_LIST List;
   char szError[1024];

   if (argc != 2)
   {
      printf("Usage: %s <theme.cbz>\n", argv[0]);
      exit(-1);
   }

   if (!runParser(argv[1], "BUTTONS", &List, szError) )
   {
     printf("Error: %s\n", szError);
     return 0;
   }
   printf("The requested block in the file parsed sucessfully\n");
   testButtons(&List, argv[1]);
//   forceRepaint();

   //need to free the memory for all the pointers in List here

   return 0;
}


