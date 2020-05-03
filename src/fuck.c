#include "fuck.h"


void push(queue* q, int x){
	q->_q[q->last++] = x;
}

int pop(queue* q){
	if(q->start==q->last) return -1;
	return q->_q[q->start++];
}

int inq(queue* q, int x){
	for(int i = q->start; i < q->last; i++){
		if(q->_q[i]==x) return 1;
	}
	return 0;
}

int main()
{
	//policy and number of processes
	char policy[15];
	int num;
	scanf("%s%d", policy, &num);
	p processes[num + 5];
	
	for (int i = 0; i < num; i++)
		scanf("%s %d %d", processes[i].name, &(processes[i].ready_time), &(processes[i].exec_time));
	
	char Policies[4][15] = {"FIFO", "RR", "SJF", "PSJF"};
	int myPolicy;
	for (int i = 0; i < 4; i++)
		if (!strcmp(policy, Policies[i])) {
			myPolicy = i;
			break;
		}
	//sort the processes
	qsort(processes, num, sizeof(p), cmp);
	//start scheduling
	scheduling(myPolicy, num, processes);
	return 0;
}

int cmp (const void *A, const void *B) {return ((p *)A)->ready_time - ((p *)B)->ready_time;}
static int RRlast, CurTime, RunIdx, FinishCnt;

int FIFO_next(p *processes, int num) {
	int ret = -1;
	for (int i = 0; i < num; i++) {
		if (processes[i].pid == -1 || processes[i].exec_time == 0)	continue;
		if (ret == -1 || processes[i].ready_time < processes[ret].ready_time)	ret = i;
	}
	return ret;
}

int SJF_next(p *processes, int num) {
	int ret = -1;
	for (int i = 0; i < num; i++) {
		if (processes[i].pid == -1 || processes[i].exec_time == 0)	continue;
		if (ret == -1 || processes[i].exec_time < processes[ret].exec_time)	ret = i;
	}
	return ret;
}

int RR_next(p *processes, int num, queue *q) {
	if (RunIdx == -1)	return pop(q);
	else if ((CurTime - RRlast) % timequantum == 0) {
		int k = pop(q);
		if (RunIdx != k && !inq(q, RunIdx) && processes[RunIdx].exec_time > 0)	push(q, RunIdx);
		return k;

	}
	else	return RunIdx;
}

int next_process(int policy, int num, p *processes, queue *q)
{
	//Index is -1, and not preemptive
	if ((policy == SJF || policy == FIFO) && RunIdx != -1)	return RunIdx;
	switch (policy) {
		case FIFO: return FIFO_next(processes, num);
		case RR: return RR_next(processes, num, q);
		case SJF: return SJF_next(processes, num);
		case PSJF: return SJF_next(processes, num);
		default: return -1;
	}	
}

int scheduling(int sched_policy, int num, p *processes)
{

	//init
	queue q;
	q.start = 0;
	q.last = 0;
	memset(q._q, 0, sizeof(q._q));
	CurTime = 0, RunIdx = -1, FinishCnt = 0;
	//Set to inactive
	for (int i = 0; i < num; i++)	processes[i].pid = -1;
	//prevent preemption and set high priority
	assign_core(getpid(), PARENT_CPU);
	set_priority(getpid(), 1);

	while(true) {
		//check finish or not
		if (processes[RunIdx].exec_time == 0 && RunIdx != -1) {
			waitpid(processes[RunIdx].pid, NULL, 0);
			printf("%s %d\n", processes[RunIdx].name, processes[RunIdx].pid);
			fflush(stdout);
			RunIdx = -1;
			//all finished or not
			if (++FinishCnt == num)	break;
		}
		//Ready or not
		for (int i = 0; i < num; i++)
			if (processes[i].ready_time == CurTime) {
				processes[i].pid = exec_proc(processes[i]);
				push(&q, i);
				set_priority(processes[i].pid, 0);
			}
		
		//Choose next process
		int next_proc = next_process(sched_policy, num, processes, &q);
		if (next_proc != -1)
			// context switch
			if (RunIdx != next_proc) {
				set_priority(processes[next_proc].pid, 1);
				set_priority(processes[RunIdx].pid, 0);
				RunIdx = next_proc;
				RRlast = CurTime;
			}

		runUnit();
		
		if (RunIdx != -1)	processes[RunIdx].exec_time--;
		CurTime++;
	}

	return 0;
}

int assign_core(int pid, int CPU)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(CPU, &mask);
	sched_setaffinity(pid, sizeof(cpu_set_t), &mask);
	return 0;
}

int set_priority(int pid, int p)
{
	struct sched_param param;
	param.sched_priority = 0;
	int ret = (p == 1? sched_setscheduler(pid, SCHED_OTHER, &param) : 
				sched_setscheduler(pid, SCHED_IDLE, &param));
	return ret < 0? -1 : ret;
}

int exec_proc(p proc)
{
	int pd, pid = fork();
	//struct timespec start, end;
	if (pid == 0) {
		pd = getpid();
       	//clock_gettime(CLOCK_REALTIME, &start);
		long start_time = syscall(GET_TIME);
		for(int j = 0; j < proc.exec_time; j++)	runUnit();
		// clock_gettime(CLOCK_REALTIME, &end);
		long end_time = syscall(GET_TIME);

		char my_message[256];
		static const long BASE = 1000000000;
		// sprintf(my_message, "[Project1] %d %ld.%09ld %ld.%09ld\n", pd, start.tv_sec, start.tv_nsec, end.tv_sec, end.tv_nsec);
		sprintf(my_message, "[Project1] %d %ld.%09ld %ld.%09ld\n", pd, start_time / BASE, start_time % BASE, end_time / BASE, end_time % BASE);
		syscall(PRINTK, my_message);            
		//fprintf(stderr, "[Project1] %d %ld.%09ld %ld.%09ld\n", pd, start / BASE, start % BASE, end / BASE, end % BASE);
		exit(0);
	}

	assign_core(pid, CHILD_CPU);
	return pid;
}
