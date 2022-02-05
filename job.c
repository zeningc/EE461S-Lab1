#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef struct job {
    char *jStr;
    int pgid;
    int status; // 0 as Running, 1 as Stopped, 2 as Done
    struct job *next;
    struct job *prev;
    int id;
} job;

typedef struct stack {
    job *head;
    int size;
} stack;

char *statusTbl[3] = {"Running", "Stopped", "Done"};

stack *createStack() {
    job *h = (job *) malloc(sizeof(job));
    h->jStr = NULL;
    h->id = 0;
    h->status = -1;
    h->pgid = 0;
    h->next = h;
    h->prev = h;
    stack *ret = (stack *) malloc(sizeof(stack));
    ret->head = h;
    ret->size = 0;
    return ret;
}

void addToStack(stack *stk, job *node) {
    node->next = stk->head->next;
    node->prev = stk->head;
    stk->head->next = node;
    node->next->prev = node;
    stk->size++;
}

void removeFromStack(job *j)  {
    j->prev->next = j->next;
    j->next->prev = j->prev;
    free(j);
}


job* addJob(stack *stk, char *jStr, int pgid, int status) {
    job *j = (job *) malloc(sizeof(job));
    j->jStr = jStr;
    j->pgid = pgid;
    j->status = status;
    j->id = stk->size = stk->head->next->id + 1;
    j->next = NULL;
    j->prev = NULL;
    addToStack(stk, j);
    return j;
}

void listFgJob(job *j)    {
    if (j == NULL)
        return;
    char *ou = (char *) malloc(3000 * sizeof (char));
    sprintf(ou, "%s\n", j->jStr);
    write(STDOUT_FILENO, ou, strlen(ou));
}

void listBgJob(stack *stk, job *j)    {
    if (j == NULL)
        return;
    char *ou = (char *) malloc(3000 * sizeof (char));
    char ch = '-';
    if (j->prev == stk->head)
        ch = '+';
    sprintf(ou, "[%d]%c\t%s &\n", j->id, ch, j->jStr);
    write(STDOUT_FILENO, ou, strlen(ou));
}

void listAllJobs(stack *stk) {
    job *curr = stk->head->prev;
    char ch = '-';
    while (curr != stk->head)   {
        char *ou = (char *) malloc(3000 * sizeof (char));
        job *next = curr->prev;
        if (next == stk->head)
            ch = '+';
        sprintf(ou, "[%d]%c\t%s\t%s\t%d\n", curr->id, ch, statusTbl[curr->status], curr->jStr, curr->pgid);
        write(STDOUT_FILENO, ou, strlen(ou));
        curr = next;
    }
}

void listDoneJobs(stack *stk)   {
    job *curr = stk->head->prev;
    char ch = '-';
    while (curr != stk->head)   {
        char *ou = (char *) malloc(3000 * sizeof (char));
        job *next = curr->prev;
        if (curr->status == 2)  {
            if (next == stk->head)
                ch = '+';
            sprintf(ou, "[%d]%c\t%s\t%s\t%d\n", curr->id, ch, statusTbl[curr->status], curr->jStr, curr->pgid);
            write(STDOUT_FILENO, ou, strlen(ou));
        }
        curr = next;
    }
}

void removeDoneJobs(stack *stk)  {
    job *curr = stk->head->next;
    while (curr != stk->head)    {
        job *next = curr->next;
        if (curr->status == 2)
            removeFromStack(curr);
        curr = next;
    }
}


void stop(job *j)   {
    j->status = 1;
}

void done(job *j)   {
    j->status = 2;
}

void run(job *j)    {
    j->status = 0;
}

job* findByPgid(stack *stk, pid_t pgid)   {
    job *curr = stk->head->next;
    while (curr != stk->head)    {
        if (curr->pgid == pgid)
            return curr;
        curr = curr->next;
    }
    return NULL;
}


/*
 * return the pgid of first stopped job
 * return -1 if such job doesn't exist
 * */
job* getFirstStopJob(stack *stk)   {
    job* curr = stk->head->next;
    while (curr != stk->head)    {
        if (curr->status == 1)  {
            return curr;
        }
    }
    return NULL;
};

job* getFirstJob(stack *stk)    {
    if (stk->head->next == stk->head)
        return NULL;
    return stk->head->next;
}


