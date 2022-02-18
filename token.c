//
// Created by Zening Chen on 1/31/22.
//
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char **parseStr(char *inStr)
{
    char **ret = (char **)malloc(sizeof(char **));
    char **t = ret;
    char *p;
    p = strtok(inStr, " ");
    *t++ = p;
    while (p != NULL)
    {
        p = strtok(NULL, " ");
        *t++ = p;
    }
    *t = (char *)NULL;

    return ret;
}

/*
check if pipe is contained in cmd
returns:
-2: multiple pipes found
-1: no pipe found
others: the location of pipe sign
*/
int checkPipe(char *inStr)
{
    int flag = 0;
    int ret = -1;
    char *start = inStr;
    while (*inStr != '\0')
    {
        if (*inStr == '|')
        {
            if (flag == 0)
            {
                flag = 1;
                ret = inStr - start;
            }
            else
            {
                return -1;
            }
        }
        inStr++;
    }
    return ret;
}

int parseRawCMD(char *inStr, char** left, char** right)    {
    int pipeIndex = checkPipe(inStr);
    if (pipeIndex == -1)    {
        *left = inStr;
        return 1;
    }
    int n = strlen(inStr);
    char *src = (char *)malloc(2000 * sizeof(char));
    int t = pipeIndex - 1;
    while (t > -1 && inStr[t] == ' ')
        t--;

    strncpy(src, inStr, t + 1);
    char *dest = (char *)malloc(2000 * sizeof(char));
    int leftStart = pipeIndex + 1;
    while (leftStart < n && inStr[leftStart] == ' ')
        leftStart++;
    int rightStart = n - 1;
    while (rightStart > pipeIndex && inStr[rightStart] == ' ')
        rightStart--;

    strncpy(dest, inStr + leftStart, rightStart - leftStart + 1);
    *left = src;
    *right = dest;
    return 2;
}

/*
 * return the index of & sign if exists
 * otherwise return -1
 * */
int parseAndSign(char *inStr)   {
    int n = strlen(inStr);
    if (*(inStr + n - 1) == '&')
        return n - 1;
    return -1;
}

