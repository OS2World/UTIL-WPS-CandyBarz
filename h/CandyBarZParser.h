/* $Id: CandyBarZParser.h,v 1.1 1999/06/15 20:48:01 ktk Exp $ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "CandyBarZScanner.h"
#include <os2.h>

// This parser recoginzes the following context free grammar.
//
// <Block> -->
// fill the rest in here...

typedef struct
{
    char *key;
    char *value;
}
KEY_VALUE;

typedef struct
{
    char szPluginName[9];
    int cbAttributes;
    int enabledState;

    KEY_VALUE *keyValue;
}
PLUGIN_ATTRIBUTES;

typedef struct
{
    int cbPlugins;
    PLUGIN_ATTRIBUTES *pPluginAttributes;
}
PLUGIN_LIST;

BOOL runParser(char filename[], char block[], PLUGIN_LIST * pList, char szErr[]);
BOOL getThemeInfo(char filename[], PLUGIN_LIST * pList, char szErr[]);

BOOL CBZFreePLUGIN_LIST(PLUGIN_LIST * List);
