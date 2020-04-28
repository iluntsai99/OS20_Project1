#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sched.h>
#include <string.h>
#include <math.h>
#include "process.h"
#include "scheduler.h"

char policy[10];

int main(){
	int num; //number of processes
	scanf("%s", policy);
	int sched_policy;
	scanf("%d", &num);
	p processes[num+1];
	for(int i = 0; i < num; i++){
		scanf("%s %d %d", processes[i].name, &(processes[i].ready_time), &(processes[i].exec_time));
	}
	char policy_list[4][5] = {"FIFO", "RR", "SJF", "PSJF"};
	int flag = 0;
	for(int i = 0; i<4; i++){
		if(strcmp(policy, policy_list[i]) == 0){
            		sched_policy = i;
			flag = 1;
			break;
		}
	}
	if(flag == 0){
		fprintf(stderr, "%s does not exist", policy);
		exit(0);
	}
	qsort(processes, num, sizeof(p), cmp);
	scheduling(processes, num, sched_policy);
	return 0;
}
