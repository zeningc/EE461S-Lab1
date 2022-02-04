//
// Created by Zening Chen on 2/1/22.
//


//#include <stdio.h>
//#include <stdlib.h>
//#include <signal.h>
//#include <unistd.h>

//
//typedef struct job {
//    char *jStr;
//    int pgid;
//    int status; // 0 as Running, 1 as Stopped, 2 as Done
//    struct job *next;
//    int id;
//} job;
//
//typedef struct stack {
//    job *head;
//    int size;
//} stack;
//
//char *statusTbl[3] = {"Running", "Stop", "Done"};
//
//stack *createStack() {
//    job *h = (job *) malloc(sizeof(job));
//    h->jStr = NULL;
//    h->id = 0;
//    h->status = -1;
//    h->pgid = 0;
//    stack *ret = (stack *) malloc(sizeof(stack));
//    ret->head = h;
//    h->next = NULL;
//    ret->size = 0;
//    return ret;
//}
//
//void addToStack(stack *stk, job *node) {
//    node->next = stk->head->next;
//    stk->head->next = node;
//    stk->size++;
//}
//
//void removeFromStack(stack *stk, job *prev, job *curr) {
//    prev->next = curr->next;
//    stk->size--;
//}
//
//void addJob(stack *stk, char *jStr, int pgid, int status) {
//    job *j = (job *) malloc(sizeof(job));
//    j->jStr = jStr;
//    j->pgid = pgid;
//    j->status = status;
//    j->id = stk->size == 0 ? 1 : stk->head->next->id + 1;
//    j->next = NULL;
//    addToStack(stk, j);
//}
//
//void listJobs(stack *stk) {
//    job *curr = stk->head->next;
//    char jobs[stk->size][3000];
//    int i = stk->size - 1;
//    int flag = 1;
//    while (curr != NULL) {
//        char ch = '-';
//        if (flag == 1) {
//            flag = 0;
//            ch = '+';
//        }
//        sprintf(jobs[i--], "[%d]%c\t%s\t%s\n", curr->id, ch, statusTbl[curr->status], curr->jStr);
//        curr = curr->next;
//    }
//
//    for (int i = 0; i < stk->size; i++) {
//        write(STDOUT_FILENO, jobs[i], strlen(jobs[i]));
//    }
//}
//
//
//void removeDone(stack *stk) {
//    job *prev = stk->head;
//    job *curr = prev->next;
//    job *jobs[stk->size];
//    int index = 0;
//    int top = stk->head->next != NULL && stk->head->next->status == 2 ? 1 : 0;
//    while (curr != NULL) {
//        if (curr->status == 2)  {
//            job* next = curr->next;
//            jobs[index++] = curr;
//            removeFromStack(stk, prev, curr);
//            curr = next;
//        }
//        else    {
//            prev = curr;
//            curr = curr->next;
//        }
//    }
//
//
//    for (int i = index - 1; i > -1; i--)    {
//        char *ou = (char*) malloc(3000 * sizeof (char));
//        curr = jobs[i];
//        if (top == 1 && i == 0)
//            sprintf(ou, "[%d]+\t%s\t%s\n", curr->id , statusTbl[curr->status], curr->jStr);
//        else
//            sprintf(ou, "[%d]-\t%s\t%s\n", curr->id , statusTbl[curr->status], curr->jStr);
//        write(STDOUT_FILENO, ou, strlen(ou));
//    }
//}
//
//
//int main() {
//    stack *stk = createStack();
//    addJob(stk, "job 1", 1, 0);
//    addJob(stk, "job 2", 2, 1);
//    addJob(stk, "job 3", 3, 0);
//    addJob(stk, "job 4", 4, 0);
//    addJob(stk, "job 5", 5, 2);
//    addJob(stk, "job 6", 6, 2);
//
//    listJobs(stk);
//    printf("\n");
//    removeDone(stk);
//    printf("\n");
//    listJobs(stk);
//}

//void sigchldHandler()   {
//    pid_t pid;
//    while ((pid = waitpid(-1, (int*) NULL, WNOHANG)) > 0)
//    {
//        printf("children id: %d\n", pid);   // Or whatever you need to do with the PID
//    }
//}
//
//int main()  {
//    signal(SIGCHLD, sigchldHandler);
//    pid_t cpid = fork();
//
//    if (cpid == 0)  {
//        sleep(10);
//        exit(0);
//    }
//
//
//    while(1)    {
//        sleep(1);
//        printf("running! %d", getpid());
//    }
//
//    return 0;
//}


#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


int main(void) {
    signal(SIGTTOU, SIG_IGN);
    pid_t cpid = fork();
    if (cpid == 0) {
        if (setpgid(0, 0) != 0) {
            perror("setpgid() error");
        }

        /*
         * Grant the foreground terminal control
         * */
        if (tcsetpgrp(STDIN_FILENO, getpid()) != 0)
            perror("tcsetpgrp() error");

        char *v[3] = {"sleep", "5", (char *) NULL};
        /*
         * after running this line, I pressed ctrl+z and expected the child process ends;
         * */
        execvp(v[0], v);
        exit(0);
    } else {
        waitpid(cpid, (int *)NULL, WUNTRACED);
        if (tcsetpgrp(STDIN_FILENO, getpid()) != 0)
            perror("tcsetpgrp() error");
    }


    printf("parent taking control now\n");
    return 0;
}