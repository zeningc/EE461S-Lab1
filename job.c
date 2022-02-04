#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef struct job {
    char *jStr;
    int pgid;
    int status; // 0 as Running, 1 as Stopped, 2 as Done
    struct job *next;
    int detached;
    int id;
} job;

typedef struct stack {
    job *head;
    int size;
} stack;

char *statusTbl[3] = {"Running", "Stop", "Done"};

stack *createStack() {
    job *h = (job *) malloc(sizeof(job));
    h->jStr = NULL;
    h->id = 0;
    h->status = -1;
    h->pgid = 0;
    stack *ret = (stack *) malloc(sizeof(stack));
    ret->head = h;
    h->next = NULL;
    ret->size = 0;
    return ret;
}

void addToStack(stack *stk, job *node) {
    node->next = stk->head->next;
    stk->head->next = node;
    stk->size++;
}

void removeFromStack(stack *stk, job *prev, job *curr) {
    prev->next = curr->next;
    stk->size--;
}

job* addJob(stack *stk, char *jStr, int pgid, int status, int detached) {
    job *j = (job *) malloc(sizeof(job));
    j->jStr = jStr;
    j->pgid = pgid;
    j->status = status;
    j->id = stk->size == 0 ? 1 : stk->head->next->id + 1;
    j->next = NULL;
    j->detached = detached;
    addToStack(stk, j);
    return j;
}

void detach(job *j) {
    j->detached = 1;
}

void unDetach(job *j) {
    j->detached = 0;
}

void stop(job *j)   {
    j->status = 1;
}

void done(job *j)   {
    j->status = 2;
}

job* findByPgid(stack *stk, pid_t pgid)   {
    job *curr = stk->head->next;
    while (curr != NULL)    {
        if (curr->pgid == pgid)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

void listAllJobs(stack *stk) {
    job *curr = stk->head->next;
    char jobs[stk->size][3000];
    int i = stk->size - 1;
    int flag = 1;
    while (curr != NULL) {
        char ch = '-';
        if (flag == 1) {
            flag = 0;
            ch = '+';
        }
        sprintf(jobs[i--], "[%d]%c\t%s\t%s\t%d\n", curr->id, ch, statusTbl[curr->status], curr->jStr, curr->pgid);
        curr = curr->next;
    }

    for (int i = 0; i < stk->size; i++) {
        write(STDOUT_FILENO, jobs[i], strlen(jobs[i]));
    }
}

void listDetachedDoneJobs(stack *stk)   {
    job *prev = stk->head;
    job *curr = prev->next;
    job *jobs[stk->size];
    int index = 0;
    int top = stk->head->next != NULL && stk->head->next->status == 2 ? 1 : 0;
    while (curr != NULL) {
        if (curr->status == 2)  {
            job* next = curr->next;
            jobs[index++] = curr;
            curr = next;
        }
        else    {
            prev = curr;
            curr = curr->next;
        }
    }

    for (int i = index - 1; i > -1; i--)    {
        char *ou = (char*) malloc(3000 * sizeof (char));
        curr = jobs[i];
        if (curr->detached == 1)    {
            if (top == 1 && i == 0)
                sprintf(ou, "[%d]+\t%s\t%s\n", curr->id , statusTbl[curr->status], curr->jStr);
            else
                sprintf(ou, "[%d]-\t%s\t%s\n", curr->id , statusTbl[curr->status], curr->jStr);
        }
        write(STDOUT_FILENO, ou, strlen(ou));
    }

}

void removeDoneJobs(stack *stk) {
    job *prev = stk->head;
    job *curr = prev->next;
    while (curr != NULL) {
        if (curr->status == 2)  {
            job* next = curr->next;
            removeFromStack(stk, prev, curr);
            free(curr);
            curr = next;
        }
        else    {
            prev = curr;
            curr = curr->next;
        }
    }
}
