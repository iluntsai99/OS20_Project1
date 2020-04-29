#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sched.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/types.h>

#define __NS 1000000000
#define CHILD_CPU 1
#define PARENT_CPU 0
#define PRINTK 336
#define GET_TIME 335
#define ll long long
#define FIFO	0
#define RR		1
#define SJF		2
#define PSJF 	3
#define timequantum 500
#define true 1
#define runUnit() {volatile unsigned long i; for (i = 0; i < 1000000UL; i++);}


typedef struct process{
	char name[32];
	int ready_time;
	int exec_time;
	pid_t pid;
} p;

int assign_core(int pid, int core); //Assign process to specific CPU
int exec_proc(p proc); //Execute the process and return pid
int set_priority(int pid, int p); //Set priority to a process (int pid), p = 1 means high priority, p = 0 means low priority 


int cmp (const void *A, const void *B);
int next_process(int policy, int num, p *processes);
int FIFO_next(p *processes, int num);
int SJF_next(p *processes, int num);
int RR_next(p *processes, int num);
int scheduling(int sched_policy, int num, p *processes);