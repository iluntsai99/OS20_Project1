#ifndef _SCHEDULING_H_
#define _SCHEDULING_H_

#include "process.h"
#define FIFO	0
#define RR	1
#define SJF	2
#define PSJF 3
#define timequantum 500 //5E2

int cmp (const void *A, const void *B);
int IDX_Next_Process(p *processes, int num, int sched_policy);
int FIFO_Next(p *processes, int num);
int SJF_Next(p *processes, int num);
int RR_Next(p *processes, int num);
int scheduling(p *processes, int num, int sched_policy);

#endif
