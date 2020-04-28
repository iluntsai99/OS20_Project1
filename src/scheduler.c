#define _GNU_SOURCE
#include "process.h"
#include "scheduler.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>

int cmp (const void *A, const void *B) {
	int a, b;
	a = ((p *)A)->ready_time;
	b = ((p *)B)->ready_time;
	return a - b;
}

static int RRlast, CurTime, RunIdx, Nfinish;

/*Decide the next in FIFO*/
int FIFO_next(p *processes, int num) {
	int temp = -1;
	for (int i = 0; i < num; i++) {
		if (processes[i].pid == -1 || processes[i].exec_time == 0)
			continue;
		if (temp == -1 || processes[i].ready_time < processes[temp].ready_time)
			temp = i;
	}
	return temp;
}

int SJF_next(p *processes, int num) {
	int temp = -1;
	for (int i = 0; i < num; i++) {
		if (processes[i].pid == -1 || processes[i].exec_time == 0)
			continue;
		if (temp == -1 || processes[i].exec_time < processes[temp].exec_time)
			temp = i;
	}
	return temp;
}

int RR_next(p *processes, int num) {
	int temp = -1;
	if (RunIdx == -1) {
		for (int i = 0; i < num; i++) {
			if (processes[i].pid != -1 && processes[i].exec_time > 0){
				temp = i;
				break;
			}
		}
	}
	else if ((CurTime - RRlast) % 500 == 0)  {
		temp = (RunIdx + 1) % num;
		while (processes[temp].pid == -1 || processes[temp].exec_time == 0)
			temp = (temp + 1) % num;
	}
	else
		temp = RunIdx;
	return temp;
}

/* Return index of next process  */
int next_process(p *processes, int num, int policy)
{
	if (RunIdx != -1 && (policy == SJF || policy == FIFO))	//Index is -1, and not preemptive
		return RunIdx;

	int ret = -1;

	switch (policy) {
		case FIFO: return FIFO_next(processes, num);
		case PSJF: return SJF_next(processes, num);
		case SJF: return SJF_next(processes, num);
		case RR: return RR_next(processes, num);
		default: return ret;
	}
	return ret;
}

int scheduling(p *processes, int num, int sched_policy)
{

	/* Initial pid = -1 imply not ready */
	for (int i = 0; i < num; i++)
		processes[i].pid = -1;

	/* Set single core prevent from preemption */
	assign_core(getpid(), PARENT_CPU);
	
	/* Set high priority to scheduler */
	set_priority(getpid(), 1);
	
	CurTime = 0, RunIdx = -1, Nfinish = 0; //init
	
	while(1) {
		/* Check if RunIdx process finish */
		if (RunIdx != -1 && processes[RunIdx].exec_time == 0) {
			waitpid(processes[RunIdx].pid, NULL, 0);
			printf("%s %d\n", processes[RunIdx].name, processes[RunIdx].pid);
			RunIdx = -1;
			Nfinish++;

			if (Nfinish == num) //all processes are finished
				break;
		}

		/* Check if a process is ready and then execute */
		for (int i = 0; i < num; i++) {
			if (processes[i].ready_time == CurTime) {
				processes[i].pid = exec_proc(processes[i]);
				set_priority(processes[i].pid, 0);
			}
		}

		/* Select next RunIdx  process */
		int next_proc = next_process(processes, num, sched_policy);
		if (next_proc != -1) {
			/* Context switch */
			if (RunIdx != next_proc) {
				set_priority(processes[next_proc].pid, 1);
				set_priority(processes[RunIdx].pid, 0);
				RunIdx = next_proc;
				RRlast = CurTime;
			}
		}

		/* Run an unit of time */
		volatile unsigned long i;		
		for (i = 0; i < 1000000UL; i++);
		if (RunIdx != -1)
			processes[RunIdx].exec_time--;
		CurTime++;
	}

	return 0;
}
