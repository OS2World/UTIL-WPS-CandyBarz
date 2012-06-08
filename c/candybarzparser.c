/* $Id: candybarzparser.c,v 1.2 2000/09/06 14:36:12 cwohlgemuth Exp $ */


#include "CandyBarZParser.h"

FILE *fp;
int currentToken;
char currentLexeme[1024];
char szError[1024];

int maxAttrCount;
int currentAttrIndex;
int maxPluginCount;
int currentPluginIndex;
PLUGIN_LIST *pPluginList;

BOOL ParserMatch(int);
BOOL ParserBlock(void);
BOOL ParserPluginEnabledState(void);
BOOL ParserPlugin(void);
BOOL ParserPluginName(void);
BOOL ParserPluginParams(void);
BOOL ParserKeyValuePair(void);

long QueryPluginCount(void);
long QueryAttributeCount(void);

BOOL CBZFreePLUGIN_LIST(PLUGIN_LIST * List)
{
    int i, j;

    //delete all the attributes in the plugin/key/value list
    for (j = 0; j < List->cbPlugins; j++)
    {
        if (List->pPluginAttributes)
        {
            for (i = 0; i < List->pPluginAttributes[j].cbAttributes; i++)
            {
                if (List->pPluginAttributes[j].keyValue)
                {
                    if (List->pPluginAttributes[j].keyValue[i].key)
                        free(List->pPluginAttributes[j].keyValue[i].key);
                    if (List->pPluginAttributes[j].keyValue[i].value)
                        free(List->pPluginAttributes[j].keyValue[i].value);
                }
            }
            if (List->pPluginAttributes[j].keyValue)
                free(List->pPluginAttributes[j].keyValue);
        }
    }
    if (List->pPluginAttributes)
        free(List->pPluginAttributes);

}

BOOL ParserMatch(int token)
{
    if (token != currentToken)
    {
        //error message is added by calling procedure.
        return FALSE;
    }
    else
    {
        //get the next token...
        currentToken = ScannerGetNextToken(fp, currentLexeme);
        return TRUE;
    }

}

BOOL getThemeInfo(char filename[], PLUGIN_LIST * pList, char szErr[])
{
    long count;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        sprintf(szErr, "Unable to open file. (%s)", filename);
        return FALSE;
    }

    maxPluginCount = 1;
    pPluginList = pList;
    pPluginList->cbPlugins = 1; //initially there are no plugins!

    pPluginList->pPluginAttributes = (PLUGIN_ATTRIBUTES *) malloc(sizeof(PLUGIN_ATTRIBUTES) * maxPluginCount);
    pPluginList->pPluginAttributes[0].keyValue = NULL;
    pPluginList->pPluginAttributes[0].cbAttributes = 0;
    currentPluginIndex = 0;

    while (1)
    {
        currentToken = ScannerGetNextToken(fp, currentLexeme);
        if (currentToken == CBZ_BLOCKNAME)
        {
            if (strcmp(currentLexeme, "INFO") == 0)
            {
                currentToken = ScannerGetNextToken(fp, currentLexeme);
                if (!ParserMatch(CBZ_STARTPARAMS))
                {
                    sprintf(szErr, "Expecting " "{" ", found %s", currentLexeme);
                    if (fp)
                        fclose(fp);
                    return FALSE;
                }
                maxAttrCount = QueryAttributeCount();
                pPluginList->pPluginAttributes[currentPluginIndex].cbAttributes = maxAttrCount;
                if (maxAttrCount <= 0)
                {
                    pPluginList->pPluginAttributes[currentPluginIndex].keyValue = NULL;
                    if (!ParserMatch(CBZ_ENDPARAMS))
                    {
                        sprintf(szError, "Expecting " "}" ", found %s", currentLexeme);
                        return FALSE;
                    }
                    return TRUE;
                }

                pPluginList->pPluginAttributes[currentPluginIndex].keyValue = (KEY_VALUE *) malloc(sizeof(KEY_VALUE) * maxAttrCount);
                currentAttrIndex = 0;

                for (count = 0; count < maxAttrCount; count++)
                {
                    if (!ParserKeyValuePair())
                        return FALSE;
                }
                if (fp)
                    fclose(fp);
                return TRUE;
            }
        }
        else if ((currentToken == CBZ_INVALID_CHARACTER) || (currentToken == CBZ_EOF))
        {
            if (currentToken == CBZ_INVALID_CHARACTER)
                sprintf(szErr, "Unknown Character Encountered (%s)", currentLexeme);
            else
                sprintf(szErr, "End of File Encountered", currentLexeme);

            if (fp)
                fclose(fp);
            return FALSE;
        }
    }

    //don't think you can get here... but just in case...
    if (fp)
        fclose(fp);
    return TRUE;

}

BOOL runParser(char filename[], char block[], PLUGIN_LIST * pList, char szErr[])
{
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        sprintf(szErr, "Unable to open file. (%s)", filename);
        return FALSE;
    }

    pPluginList = pList;
    memset(pPluginList,0,sizeof(PLUGIN_LIST));//initially there are no plugins!

    while (1)
    {
        currentToken = ScannerGetNextToken(fp, currentLexeme);
        if (currentToken == CBZ_BLOCKNAME)
        {
            if (strcmp(currentLexeme, block) == 0)
            {
                currentToken = ScannerGetNextToken(fp, currentLexeme);
                if (!ParserBlock())
                {
                    strcpy(szErr, szError);
                    if (fp)
                        fclose(fp);
                    return FALSE;
                }

                if (fp)
                    fclose(fp);
                return TRUE;
            }
        }
        else if ((currentToken == CBZ_INVALID_CHARACTER) || (currentToken == CBZ_EOF))
        {
            if (fp)
                fclose(fp);

            if (currentToken == CBZ_INVALID_CHARACTER)
            {
                sprintf(szErr, "Unknown Character Encountered (%s)", currentLexeme);
                return FALSE;
            }
            else
            {
                sprintf(szErr, "End of File Encountered", currentLexeme);
                return TRUE;
            }

        }
    }

    //don't think you can get here... but just in case...
    if (fp)
        fclose(fp);
    return TRUE;
}
long QueryPluginCount(void)
{
    int tempToken = currentToken;
    char tempLexeme[1024];
    long lCount = 0;
    fpos_t pos;

    fgetpos(fp, &pos);

    while (tempToken != CBZ_BLOCKNAME && tempToken != CBZ_EOF)
    {
        if (tempToken == CBZ_PLUGINNAME)
            lCount++;
        tempToken = ScannerGetNextToken(fp, tempLexeme);
    }
    fsetpos(fp, &pos);
    return lCount;
}

long QueryAttributeCount(void)
{
    int tempToken = currentToken;
    char tempLexeme[1024];
    long lCount = 0;
    fpos_t pos;

    fgetpos(fp, &pos);

    while (tempToken != CBZ_ENDPARAMS && tempToken != CBZ_EOF)
    {
        if (tempToken == CBZ_IDENTIFIER)
        {
            lCount++;
        }
        tempToken = ScannerGetNextToken(fp, tempLexeme);
    }
    fsetpos(fp, &pos);
    return lCount;
}

BOOL ParserBlock(void)
{
    long lPluginCount;
    int count;

    lPluginCount = QueryPluginCount();

    maxPluginCount = lPluginCount;
    pPluginList->cbPlugins = maxPluginCount;
    if (maxPluginCount <= 0)
    {
        pPluginList->pPluginAttributes = NULL;
        return TRUE;
    }

    pPluginList->pPluginAttributes = (PLUGIN_ATTRIBUTES *) malloc(sizeof(PLUGIN_ATTRIBUTES) * maxPluginCount);
    currentPluginIndex = 0;

    for (count = 0; count < maxPluginCount; count++)
    {
        if (!ParserPlugin())
            return FALSE;
    }
    return TRUE;
}

BOOL ParserPlugin(void)
{
    switch (currentToken)
    {
    case CBZ_PLUGINNAME:
        {
            if (!ParserPluginName())
                return FALSE;
            if (!ParserPluginParams())
                return FALSE;
            currentPluginIndex++;
        }
        break;

    default:
        {
            sprintf(szError, "Expecting " "PluginName" " found %s", currentLexeme);
            return FALSE;
        }
        break;
    }
    return TRUE;
}

BOOL ParserPluginName(void)
{
    switch (currentToken)
    {
    case CBZ_PLUGINNAME:
        {
            if (!ParserMatch(CBZ_PLUGINNAME))
            {
                sprintf(szError, "Expecting " "PluginName" ", found %s", currentLexeme);
                return FALSE;
            }

            if (!ParserMatch(CBZ_EQUALS))
            {
                sprintf(szError, "1. Expecting " "=" ", found %s", currentLexeme);
                return FALSE;
            }

            //copy plugin name & increment current plugin count
            if (currentToken == CBZ_IDENTIFIER)
            {
                strncpy(pPluginList->pPluginAttributes[currentPluginIndex].szPluginName, currentLexeme, 8);
            }
            pPluginList->pPluginAttributes[currentPluginIndex].szPluginName[8] = '\0';  //slap a null on it just in case.

            if (!ParserMatch(CBZ_IDENTIFIER))
            {
                sprintf(szError, "Expecting a PluginName, found %s", currentLexeme);
                return FALSE;
            }
        }
        break;

    default:
        {
            sprintf(szError, "Expecting " "PluginName" " found %s", currentLexeme);
            return FALSE;
        }
        break;
    }
    return TRUE;
}

BOOL ParserPluginParams(void)
{
    long count;

    switch (currentToken)
    {
    case CBZ_STARTPARAMS:
        {
            if (!ParserMatch(CBZ_STARTPARAMS))
            {
                sprintf(szError, "Expecting " "{" ", found %s", currentLexeme);
                return FALSE;
            }
            maxAttrCount = QueryAttributeCount();
            printf("AttributeCount for plugin #%d = %d!\n", currentPluginIndex, maxAttrCount);
            pPluginList->pPluginAttributes[currentPluginIndex].cbAttributes = maxAttrCount;
            if (maxAttrCount <= 0)
            {
                pPluginList->pPluginAttributes[currentPluginIndex].keyValue = NULL;
                if (!ParserMatch(CBZ_ENDPARAMS))
                {
                    sprintf(szError, "Expecting " "}" ", found %s", currentLexeme);
                    return FALSE;
                }
                return TRUE;
            }

            pPluginList->pPluginAttributes[currentPluginIndex].keyValue = (KEY_VALUE *) malloc(sizeof(KEY_VALUE) * maxAttrCount);
            currentAttrIndex = 0;

            if (!ParserPluginEnabledState())
                return FALSE;
            for (count = 0; count < maxAttrCount; count++)
            {
                if (!ParserKeyValuePair())
                    return FALSE;
            }

            if (!ParserMatch(CBZ_ENDPARAMS))
            {
                sprintf(szError, "Expecting " "}" ", found %s", currentLexeme);
                return FALSE;
            }
        }
        break;

    default:
        {
            sprintf(szError, "Expecting " "{" ", found %s", currentLexeme);
            return FALSE;
        }
        break;
    }
    return TRUE;
}

BOOL ParserPluginEnabledState(void)
{
    switch (currentToken)
    {
    case CBZ_PLUGINSTATE:
        {
            if (!ParserMatch(CBZ_PLUGINSTATE))
            {
                sprintf(szError, "Expecting " "EnabledForState" ", found %s", currentLexeme);
                return FALSE;
            }
            if (!ParserMatch(CBZ_EQUALS))
            {
                sprintf(szError, "Expecting " "=" ", found %s", currentLexeme);
                return FALSE;
            }

            if (currentToken == CBZ_LONGINT)
            {
                pPluginList->pPluginAttributes[currentPluginIndex].enabledState = atoi(currentLexeme);
            }
            if (!ParserMatch(CBZ_LONGINT))
            {
                sprintf(szError, "Expecting an Integer, found %s", currentLexeme);
                return FALSE;
            }

        }
        break;

    default:
        {
            sprintf(szError, "Expecting " "EnabledForState" " found %s\n", currentLexeme);
            return FALSE;
        }
        break;
    }
    return TRUE;
}

BOOL ParserKeyValuePair(void)
{
    switch (currentToken)
    {
    case CBZ_IDENTIFIER:
        {
            if (currentAttrIndex >= maxAttrCount)
            {
                sprintf(szError, "Too many Attributes! Increase AttributeCount.");
                return FALSE;
            }
            pPluginList->pPluginAttributes[currentPluginIndex].keyValue[currentAttrIndex].key = (char *) malloc(sizeof(char) * (strlen(currentLexeme) + 1));
            strcpy(pPluginList->pPluginAttributes[currentPluginIndex].keyValue[currentAttrIndex].key, currentLexeme);

            if (!ParserMatch(CBZ_IDENTIFIER))
            {
                sprintf(szError, "Expecting an AttributeName, found %s", currentLexeme);
                return FALSE;
            }
            if (!ParserMatch(CBZ_EQUALS))
            {
                sprintf(szError, "Expecting " "=" ", found %s", currentLexeme);
                return FALSE;
            }
            if (currentToken == CBZ_STRING)
            {
                pPluginList->pPluginAttributes[currentPluginIndex].keyValue[currentAttrIndex].value = (char *) malloc(sizeof(char) * (strlen(currentLexeme) + 1));
                strcpy(pPluginList->pPluginAttributes[currentPluginIndex].keyValue[currentAttrIndex].value, currentLexeme);

                currentAttrIndex++;
            }

            if (!ParserMatch(CBZ_STRING))
            {
                sprintf(szError, "Expecting a String (in quotes), found %s", currentLexeme);
                free(pPluginList->pPluginAttributes[currentPluginIndex].keyValue[currentAttrIndex].key);
                return FALSE;
            }
        }
        break;

    case CBZ_ENDPARAMS:
        {
            //do nothing.
            return TRUE;
        }
        break;

    default:
        {
            if (currentToken == CBZ_EOF)
                sprintf(szError, "Expecting an AttributeName, found End Of File!");
            else
                sprintf(szError, "Expecting an AttributeName found %s", currentLexeme);
            return FALSE;
        }
        break;
    }
    return TRUE;
}
