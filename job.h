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
    int detached;
} job;

typedef struct stack {
    job *head;
    int size;
} stack;

stack *createStack();
void addToStack(stack *stk, job *node);
void removeFromStack(stack *stk, job *node);
job* addJob(stack *stk, char *jStr, int pgid, int status, int detached);
void listAllJobs(stack *stk);
void removeDoneJobs(stack *stk);
void listDetachedDoneJobs(stack * stk);
void detach(job *j);
void unDetach(job *j);
void stop(job *j);
void done(job *j);
job* findByPgid(stack *stk, pid_t pgid);

#endif //LAB1_JOB_H
