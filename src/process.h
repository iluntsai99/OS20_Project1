//FINAL VERSION?
#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>

#define CHILD_CPU 1
#define PARENT_CPU 0
#define PRINTK 336
#define GET_TIME 335
#define ll long long

typedef struct process{
	char name[32];
	int ready_time;
	int exec_time;
	pid_t pid;
}p;

int assign_core(int pid, int core); //Assign process to specific core
int exec_proc(p proc); //Execute the process and return pid
int set_priority(int pid, int p); //Set priority to a process (int pid), p = 1 means high priority, p = 0 means very low priority 

#endif
