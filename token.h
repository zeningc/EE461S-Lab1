//
// Created by Zening Chen on 2/1/22.
//

#ifndef LAB1_TOKEN_H
#define LAB1_TOKEN_H

int checkPipe(char *inStr);
int parseRawCMD(char *inStr, char** left, char** right);
char **parseStr(char *inStr);
int parseAndSign(char *inStr);
#endif //LAB1_TOKEN_H
