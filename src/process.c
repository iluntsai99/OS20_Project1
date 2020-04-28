//Final version?
#define _GNU_SOURCE
#include "process.h"
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define __NS 1000000000

int assign_core(int pid, int core){
	if(core>sizeof(cpu_set_t)){
		fprintf(stderr, "core error.");
		return -1;
	}

	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);
	if(sched_setaffinity(pid, sizeof(mask), &mask) < 0){
		perror("sched_setaffinity");
		exit(1);
	}
	return 0;
}

int exec_proc(p proc){
	int p;
	int pid = fork();
	ll start_s, start_ns, end_s, end_ns;
	if(pid < 0){
		perror("fork");
		return -1;
	}
	else if(pid==0){

		p = getpid();

		start_s = syscall(GET_TIME);
		start_ns = start_s%__NS;
		start_s/=__NS;

		for(int j = 0; j < proc.exec_time; j++){
			volatile unsigned long i;		
			for (i = 0; i < 1000000UL; i++);
		}
		
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

int set_priority(int pid, int p){
	struct sched_param param;
	param.sched_priority = 0;

	int x;
	if(p==0)
		x = sched_setscheduler(pid, SCHED_IDLE, &param);
	else if(p==1)
		x = sched_setscheduler(pid, SCHED_OTHER, &param);
	

	
	return x;
}
