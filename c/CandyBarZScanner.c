/* $Id: CandyBarZScanner.c,v 1.1 1999/06/15 20:47:56 ktk Exp $ */

#include "CandyBarZScanner.h"

//helper functions for the GetNextToken function.
void ScannerSkipWhiteSpace(FILE * fp);
void ScannerEatComment(FILE * fp);
int ScannerBlockName(FILE * fp, char lexeme[]);
int ScannerIdentifier(FILE * fp, char lexeme[]);
int ScannerEquals(FILE * fp, char lexeme[]);
int ScannerString(FILE * fp, char lexeme[]);
int ScannerIntLiteral(FILE * fp, char lexeme[]);
int ScannerStartParameters(FILE * fp, char lexeme[]);
int ScannerEndParameters(FILE * fp, char lexeme[]);
int ScannerReservedWord(char lexeme[]);

int ScannerGetNextToken(FILE * fp, char lexeme[])
{
    char ch;

    ScannerSkipWhiteSpace(fp);

    ch = fgetc(fp);
    if (feof(fp))
        return CBZ_EOF;

    ungetc(ch, fp);

    do
    {
        if (ch == ';')
        {
            //comment starts here!  ends @ end of line
            ScannerEatComment(fp);
            ScannerSkipWhiteSpace(fp);
            ch = fgetc(fp);
            if (feof(fp))
                return CBZ_EOF;
            ungetc(ch, fp);
        }
    }
    while (ch == ';');

    switch (ch)
    {
    case '[':
        //start of a block definition
        return ScannerBlockName(fp, lexeme);

    case '=':
        return ScannerEquals(fp, lexeme);

    case '"':
        return ScannerString(fp, lexeme);

    case '{':
        return ScannerStartParameters(fp, lexeme);

    case '}':
        return ScannerEndParameters(fp, lexeme);

    default:
        if (isalpha(ch))
            return ScannerIdentifier(fp, lexeme);

        if (isdigit(ch))
            return ScannerIntLiteral(fp, lexeme);

        lexeme[0] = fgetc(fp);
        lexeme[1] = '\0';
    }

    return CBZ_INVALID_CHARACTER;

}

void ScannerSkipWhiteSpace(FILE * fp)
{
    char tmpChar;

    tmpChar = fgetc(fp);
    while (!feof(fp))
    {
        if ((tmpChar == '\n') || (tmpChar == '\t') || (tmpChar == ' '))
            tmpChar = fgetc(fp);
        else
        {
            ungetc(tmpChar, fp);
            return;
        }
    }
}

void ScannerEatComment(FILE * fp)
{
    while (!feof(fp) && (fgetc(fp) != '\n')) ;
}

int ScannerBlockName(FILE * fp, char lexeme[])
{
    char ch;
    int count = 0;

    fgetc(fp);                  //consume the '['

    while (!feof(fp) && ((ch = fgetc(fp)) != ']'))
    {
        lexeme[count] = ch;
        count++;
    }
    lexeme[count] = '\0';

    return CBZ_BLOCKNAME;
}

int ScannerIdentifier(FILE * fp, char lexeme[])
{
    char ch;
    int count = 0;

    while (!feof(fp) && (isalpha(ch = fgetc(fp))))
    {
        lexeme[count] = ch;
        count++;
    }
    lexeme[count] = '\0';

    return ScannerReservedWord(lexeme);  //run it through the reserved word checker

}

int ScannerEquals(FILE * fp, char lexeme[])
{
    char ch;
    int count = 0;

    if (!feof(fp) && ((ch = fgetc(fp)) == '='))
    {
        lexeme[count] = ch;
        count++;
    }
    lexeme[count] = '\0';

    return CBZ_EQUALS;
}

int ScannerString(FILE * fp, char lexeme[])
{
    char ch;
    int count = 0;

    fgetc(fp);                  //consume the '"'

    while (!feof(fp) && ((ch = fgetc(fp)) != '"'))
    {
        lexeme[count] = ch;
        count++;
    }
    lexeme[count] = '\0';

    return CBZ_STRING;
}

int ScannerIntLiteral(FILE * fp, char lexeme[])
{
    char ch;
    int count = 0;

    while (!feof(fp) && (isdigit(ch = fgetc(fp))))
    {
        lexeme[count] = ch;
        count++;
    }
    lexeme[count] = '\0';

    return CBZ_LONGINT;
}

int ScannerStartParameters(FILE * fp, char lexeme[])
{
    lexeme[0] = fgetc(fp);
    lexeme[1] = '\0';

    return CBZ_STARTPARAMS;
}

int ScannerEndParameters(FILE * fp, char lexeme[])
{
    lexeme[0] = fgetc(fp);
    lexeme[1] = '\0';

    return CBZ_ENDPARAMS;
}

int ScannerReservedWord(char lexeme[])
{
    int i;
    static char resWord[4][16] =
    {
        {"PluginName"},
        {"EnabledForState"}};

    for (i = 0; i < 4; i++)
    {
        if (strcmp(lexeme, resWord[i]) == 0)
            return i + CBZ_RESWORDBASE;
    }

    return CBZ_IDENTIFIER;      //not a reserved word

}
