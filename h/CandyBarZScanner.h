/* $Id: CandyBarZScanner.h,v 1.1 1999/06/15 20:48:01 ktk Exp $ */


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define CBZ_INVALID_CHARACTER    0
#define CBZ_EOF                  1
#define CBZ_BLOCKNAME            2
#define CBZ_IDENTIFIER           3
#define CBZ_EQUALS               4
#define CBZ_STRING               5
#define CBZ_LONGINT              6
#define CBZ_STARTPARAMS          7
#define CBZ_ENDPARAMS            8
#define CBZ_PLUGINNAME           9
#define CBZ_PLUGINSTATE          10

#define CBZ_RESWORDBASE          9  //index where reserved words start

int ScannerGetNextToken(FILE * fp, char lexeme[]);
