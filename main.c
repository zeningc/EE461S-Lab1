#include <stdio.h>
#include <readline/readline.h>
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

static void sigchldHandler(int signum) {
    pid_t pid;
    while ((pid = waitpid(-1, (int *) NULL, WNOHANG)) > 0) {
        job *j = findByPgid(stk, pid);
        if (j != NULL) {
            done(j);
        }
    }
}


int main(void) {
    signal(SIGCHLD, sigchldHandler);
    signal(SIGTTOU, SIG_IGN);
    stk = createStack();
    while (1) {
        // 0. Register signal handlers
        signal(SIGINT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        // 1. Print the prompt (#)
        char *inStr = readline("# ");
        // 2. Grab and parse the input - NOTE: Make sure to remove the newline
        // character from the input string (otherwise, you'll pass "ls\n" to
        // execvp and there is no executable called "ln\n" just "ls")


        // 3. Check for job control tokens (fg, bg, jobs, &) (for now just
        // ignore those commands)
        if (strcmp(inStr, "jobs") == 0) {
            listAllJobs(stk);
            removeDoneJobs(stk);
            continue;
        }

        listDoneJobs(stk);
        removeDoneJobs(stk);

        if (strcmp(inStr, "fg") == 0) {
            job *j = getFirstJob(stk);
            listJob(stk, j);
            if (j->status == 1) {
                kill(-j->pgid, SIGCONT);
                run(j);
            }
            grantTerminalControl(j->pgid);
            int status = 0;
            waitpid(j->pgid, &status, WUNTRACED);
            /*
             * after the child process exit, should take control of stdin stdout stderr again
             * */
            grantTerminalControl(getpid());
            if (WIFEXITED(status)) {
                done(j);
                removeFromStack(j);
                continue;
            }
            printf("\n");
            int exitSig = WSTOPSIG(status);
            if (exitSig == SIGTSTP)
                stop(j);
            else {
                done(j);
                removeFromStack(j);
            }
            continue;
        }

        if (strcmp(inStr, "bg") == 0) {
            job *j = getFirstStopJob(stk);
            listJob(stk, j);
            if (j == NULL)
                continue;
            run(j);
            kill(-j->pgid, SIGCONT);
            continue;
        }
        char *cmd = (char *) malloc(2000 * sizeof(char));
        strcpy(cmd, inStr);
        int andSignIndex = parseAndSign(inStr);
        if (andSignIndex != -1) {
            *(cmd + andSignIndex) = '\0';
        }


        // 4. Determine the number of children processes to create (number of
        // times to call fork) (call fork once per child) (right now this will
        // just be one)
        char *left;
        char *right;
        int cmdCnt = parseRawCMD(cmd, &left, &right);
        // 5. Execute the commands using execvp or execlp - e.g. execOneChild()
        // or execTwoChildren()
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        pid_t cpid = fork();
        if (cpid == 0) {
            if (setpgid(0, 0) != 0) {
                perror("setpgid() error");
            }
            if (andSignIndex == -1) {
                grantTerminalControl(getpid());
            }

            if (cmdCnt == 1)
                executeOneChild(left, 0, 0, 0, 0);
            else
                executeTwoChild(left, right);
            exit(0);
        } else {
            job *currJob = addJob(stk, inStr, cpid, 0);
            signal(SIGINT, SIG_IGN);
            signal(SIGTSTP, SIG_IGN);
            if (andSignIndex == -1) {
                //when the cmd is without & sign
                int status = 0;
                waitpid(cpid, &status, WUNTRACED);
                /*
                 * after the child process exit, should take control of stdin stdout stderr again
                 * */
                grantTerminalControl(getpid());
                if (WIFEXITED(status)) {
                    done(currJob);
                    removeFromStack(currJob);
                    continue;
                }
                printf("\n");
                int exitSig = WSTOPSIG(status);
                if (exitSig == SIGTSTP)
                    stop(currJob);
                else {
                    done(currJob);
                    removeFromStack(currJob);
                }

            }
        }

        // 6. NOTE: There are other steps for job related stuff but good luck
        // we won't spell it out for you
    }

}