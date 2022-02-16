//
// Created by Zening Chen on 2/1/22.
//

#ifndef LAB1_JOB_H
#define LAB1_JOB_H
#include <stdlib.h>
typedef struct job {
    char *jStr;
    int pgid;
    int status; // 0 as Running, 1 as Stopped, 2 as Done
    struct job *next;
    int id;
} job;

typedef struct stack {
    job *head;
    int size;
} stack;

stack *createStack();
void addToStack(stack *stk, job *node);
void removeFromStack(job *node);
job* addJob(stack *stk, char *jStr, int pgid, int status);
void listFgJob(job *j);
void listBgJob(stack *stk, job *j);
void listAllJobs(stack *stk);
void listDoneJobs(stack *stk);
void removeDoneJobs(stack *stk);
void stop(job *j);
void done(job *j);
void run(job *j);
job* findByPgid(stack *stk, pid_t pgid);
job* getFirstStopJob(stack *stk);
job* getFirstUndoneJob(stack *stk);
#endif //LAB1_JOB_H
