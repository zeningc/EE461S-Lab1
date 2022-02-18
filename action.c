//
// Created by Zening Chen on 2/1/22.
//
#include "token.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>



void executeOneChild(char *inStr, int r, int rfd, int w, int wfd) {
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
            int fd;
            if (flag == STDIN_FILENO)
                fd = open(*t, O_RDONLY, 0644);
            else
                fd = open(*t, O_CREAT | O_WRONLY, 0644);
            if (fd == -1)   {
                printf("%s: No such file or directory\n", *t);
                exit(EXIT_FAILURE);
            }

            dup2(fd, flag);
            flag = -1;
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

    // exec() will not return if success
    if(execvp(parseRet[0], parseRet) == -1)
        exit(EXIT_FAILURE);
    // will never execute
    exit(EXIT_SUCCESS);
}

void executeTwoChild(char *left, char *right) {
    int pipefd[2];
    pipe(pipefd);
    pid_t cpid = fork();
    if (cpid == 0)  {
        close(pipefd[0]);
        executeOneChild(left, 0, 0, 1, pipefd[1]);
        // will never execute the following line when success
        close(pipefd[1]);
        exit(EXIT_FAILURE);
    }
    else    {
        cpid = fork();
        if (cpid == 0) {
            close(pipefd[1]);
            executeOneChild(right, 1, pipefd[0], 0, 0);
            // will never execute the following line when success
            close(pipefd[0]);
            exit(EXIT_FAILURE);
        } else {
            close(pipefd[0]);
            close(pipefd[1]);
            while (waitpid(-1, (int *)NULL, WNOHANG) != -1);
            exit(EXIT_SUCCESS);
        }
    }
}

void grantTerminalControl(pid_t pid)   {
    if (tcsetpgrp(STDIN_FILENO, pid) != 0)
        perror("tcsetpgrp() error");
    if (tcsetpgrp(STDOUT_FILENO, pid) != 0)
        perror("tcsetpgrp() error");
    if (tcsetpgrp(STDERR_FILENO, pid) != 0)
        perror("tcsetpgrp() error");
}
