//
// Created by Zening Chen on 2/1/22.
//

#ifndef LAB1_ACTION_H
#define LAB1_ACTION_H
#include "job.h"
int executeOneChild(char *inStr, int r, int rfd, int w, int wfd);
int executeTwoChild(char *left, char *right);
void grantTerminalControl(pid_t pid);

#endif //LAB1_ACTION_H
