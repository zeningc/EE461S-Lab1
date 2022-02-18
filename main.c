#include <stdio.h>
#include <readline/readline.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "token.h"
#include "job.h"
#include "action.h"
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

stack *stk;

/*
 * handler will go before any wait()
 * */
static void sigchldHandler(int signum) {
    pid_t pid;
    int status = 0;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        job *j = findByPgid(stk, pid);
        if (j != NULL) {
            if (WEXITSTATUS(status) == EXIT_SUCCESS) {
                done(j);
                continue;
            }
        }
        status = 0;
    }
}


int main(void) {
    signal(SIGCHLD, sigchldHandler);
    // to enable tcsetpgrp()
    signal(SIGTTOU, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    stk = createStack();
    while (1) {


        char *inStr = readline("# ");
        if (inStr == NULL)
            exit(EXIT_SUCCESS);

        listDoneJobs(stk);
        removeDoneJobs(stk);
        if (strcmp(inStr, "jobs") == 0) {
            listAllJobs(stk);
            continue;
        }

        if (strcmp(inStr, "fg") == 0) {
            job *j = getFirstUndoneJob(stk);
            if (j == NULL)
                continue;
            listFgJob(j);
            if (j->status == 1) {
                kill(-j->pgid, SIGCONT);
                run(j);
            }
            grantTerminalControl(j->pgid);
            int status = 0;

            // wait for the child to exit / stop
            waitpid(j->pgid, &status, WUNTRACED);
            // after the child process exit, should take control of stdin stdout stderr again
            grantTerminalControl(getpid());
            int exitSig = WSTOPSIG(status);
            if (exitSig == SIGTSTP) {
                stop(j);
                continue;
            }
            removeFromStack(j);
            continue;
        }

        if (strcmp(inStr, "bg") == 0) {
            job *j = getFirstStopJob(stk);
            if (j == NULL)
                continue;
            listBgJob(stk, j);
            run(j);
            kill(-j->pgid, SIGCONT);
            continue;
        }
        char *cmd = (char *) malloc(3000 * sizeof(char));
        strcpy(cmd, inStr);
        // test if the cmd contains &
        int andSignIndex = parseAndSign(inStr);
        if (andSignIndex != -1) {
            *(cmd + andSignIndex) = '\0';
        }


        char *left;
        char *right;
        // assign left and right if there is pipe, only assign left if there is no pipe
        // return the number of command
        int cmdCnt = parseRawCMD(cmd, &left, &right);

        if (left == NULL || *left == '\0')
            continue;

        pid_t cpid = fork();
        if (cpid == 0) {
            // reset the signal handlers
            signal(SIGCHLD, SIG_DFL);
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            if (setpgid(0, 0) != 0) {
                perror("setpgid() error");
            }

            if (andSignIndex == -1) { // if the cmd tend to run in fg
                grantTerminalControl(getpid());
            }

            if (cmdCnt == 1)
                executeOneChild(left, 0, 0, 0, 0);
            else
                executeTwoChild(left, right);
            exit(EXIT_FAILURE);
        } else {
            job *currJob = addJob(stk, cmd, cpid, 0);
            if (andSignIndex == -1) {//when the cmd is without & sign
                int status = 0;
                // wait for the child process to stop / exit
                waitpid(cpid, &status, WUNTRACED);
                // after the child process exit, should take control of stdin stdout stderr again
                grantTerminalControl(getpid());
                int exitSig = WSTOPSIG(status);
                if (exitSig == SIGTSTP) {
                    stop(currJob);
                    continue;
                }
                removeFromStack(currJob);
            }
        }
    }
}