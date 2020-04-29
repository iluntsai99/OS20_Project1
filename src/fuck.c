#include "fuck.h"

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

int RR_next(p *processes, int num) {
	int ret = -1;
	if (RunIdx == -1) {
		for (int i = 0; i < num; i++) {
			if (processes[i].pid != -1 && processes[i].exec_time > 0){
				ret = i;
				break;
			}
		}
	}
	else if ((CurTime - RRlast) % 500 == 0)  {
		ret = (RunIdx + 1) % num;
		while (processes[ret].pid == -1 || processes[ret].exec_time == 0)	ret = (ret + 1) % num;
	}
	else	ret = RunIdx;
	return ret;
}

int next_process(int policy, int num, p *processes)
{
	//Index is -1, and not preemptive
	if ((policy == SJF || policy == FIFO) && RunIdx != -1)	return RunIdx;
	switch (policy) {
		case FIFO: return FIFO_next(processes, num);
		case RR: return RR_next(processes, num);
		case SJF: return SJF_next(processes, num);
		case PSJF: return SJF_next(processes, num);
		default: return -1;
	}	
}

int scheduling(int sched_policy, int num, p *processes)
{

	//init
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
			RunIdx = -1;
			FinishCnt++;
			//all finished or not
			if (FinishCnt == num)	break;
		}
		//Ready or not
		for (int i = 0; i < num; i++)
			if (processes[i].ready_time == CurTime) {
				processes[i].pid = exec_proc(processes[i]);
				set_priority(processes[i].pid, 0);
			}
		//Choose next process
		int next_proc = next_process(sched_policy, num, processes);
		if (next_proc != -1)
			// contet switch
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
	return p == 1? sched_setscheduler(pid, SCHED_OTHER, &param) : 
					sched_setscheduler(pid, SCHED_IDLE, &param);
}

int exec_proc(p proc)
{
	unsigned ll start_s, start_ns, end_s, end_ns;
	int p, pid = fork();
	if (pid < 0){
		perror("fork");
		return -1;
	}
	if (pid == 0){
		p = getpid();

		start_s = syscall(GET_TIME);
		start_ns = start_s % __NS;
		start_s /= __NS;
		for(int j = 0; j < proc.exec_time; j++)	runUnit();
		end_s = syscall(GET_TIME);
		end_ns = end_s%__NS;
		end_s/=__NS;

		FILE *fp = fopen("/dev/kmsg", "a");
		fprintf(fp, "[Project1] %d %lld.%lld %lld.%lld\n", p, start_s, start_ns, end_s, end_ns);
		fclose(fp);
		//char buffer[100];
		//sprintf(buffer, "[Project1] %d %lld.%lld %lld.%lld", getpid(), start_s, start_ns, end_s, end_ns);
		//printf("%s", buffer);
		//syscall(PRINTK, "[Project1] %d %ld.%.09ld %ld.%.09ld\n", p, start_s, start_ns, end_s, end_ns);
		exit(0);
	}

	assign_core(pid, CHILD_CPU);
	return pid;
}
