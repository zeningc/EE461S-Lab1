//
// Created by Zening Chen on 2/1/22.
//
#include "token.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>




int executeOneChild(char *inStr, int r, int rfd, int w, int wfd) {
    char **parseRet = parseStr(inStr);
    char **t = parseRet;
    int breakIndex = -1;
    int flag = -1;
    int index = 0;
    while (*t != NULL) {
        if (strcmp(*t, ">") == 0) {
            flag = STDOUT_FILENO;
            w = 0;
            if (breakIndex == -1)
                breakIndex = index;
        } else if (strcmp(*t, "<") == 0) {
            flag = STDIN_FILENO;
            r = 0;
            if (breakIndex == -1)
                breakIndex = index;
        } else if (strcmp(*t, "2>") == 0) {
            flag = STDERR_FILENO;
            if (breakIndex == -1)
                breakIndex = index;
        } else if (flag != -1) {
            int fd = open(*t, O_CREAT | O_RDWR, 0644);
            if (fd == -1)
                return -1;

            dup2(fd, flag);
        }
        t++;
        index++;
    }

    if (breakIndex != -1) {
        *(parseRet + breakIndex) = (char *) NULL;
    }

    if (r == 1) {
        dup2(rfd, STDIN_FILENO);
    }

    if (w == 1) {
        dup2(wfd, STDOUT_FILENO);
    }
    execvp(parseRet[0], parseRet);

    return 0;
}

int executeTwoChild(char *left, char *right) {
    int pipefd[2];
    pipe(pipefd);
    pid_t cpid = fork();
    if (cpid == 0) {
        close(pipefd[1]);
        int err = executeOneChild(right, 1, pipefd[0], 0, 0);
        close(pipefd[0]);
        exit(0);
    } else {
        close(pipefd[0]);
        int err = executeOneChild(left, 0, 0, 1, pipefd[1]);
        wait((int *) NULL);
        close(pipefd[1]);
    }
    return 0;
}

void grantTerminalControl(pid_t pid)   {
    if (tcsetpgrp(STDIN_FILENO, pid) != 0)
        perror("tcsetpgrp() error");
    if (tcsetpgrp(STDOUT_FILENO, pid) != 0)
        perror("tcsetpgrp() error");
    if (tcsetpgrp(STDERR_FILENO, pid) != 0)
        perror("tcsetpgrp() error");
}
