#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "Simulation.h"
#include "multiqueue.h"

struct multiqueue_st{
	int preempt;
	p_queue queue_a;
	p_queue queue_b;
	p_queue queue_c;
};

struct queue_st{
	int quantum;
	p_node head;
};

struct node_st{
	p_process proc;
	p_node next;
};

p_multiqueue new_multiqueue(int qA, int qB, int qC, int preempt){
	p_multiqueue newmq = malloc(sizeof(multiqueue));
	newmq->preempt = preempt;
	newmq->queue_a = new_queue(qA);
	newmq->queue_b = new_queue(qB);
	newmq->queue_c = new_queue(qC);

	return newmq;
}

p_queue new_queue(int quantum){
	p_queue newq = malloc(sizeof(queue));
	newq->quantum = quantum;
	newq->head = NULL;

	return newq;
}

void next_process(p_multiqueue mqueue, p_cpu thecpu){
	if(mqueue->queue_a->head != NULL){
		thecpu->decoded = 0;
		thecpu->time_left = mqueue->queue_a->quantum;
		thecpu->proc = mqueue->queue_a->head->proc;
		mqueue->queue_a->head = mqueue->queue_a->head->next;
	}else
	if(mqueue->queue_b->head != NULL){
		thecpu->decoded = 0;
		thecpu->time_left = mqueue->queue_b->quantum;
		thecpu->proc = mqueue->queue_b->head->proc;
		mqueue->queue_b->head = mqueue->queue_b->head->next;
	}else
	if(mqueue->queue_c->head != NULL){
		thecpu->decoded = 0;
		thecpu->time_left = mqueue->queue_c->quantum;
		thecpu->proc = mqueue->queue_c->head->proc;
		mqueue->queue_c->head = mqueue->queue_c->head->next;
	}
}

void add_process(p_process process, p_multiqueue mqueue, p_cpu thecpu){
	if(process->promote >= 3){
		queue_process(process, mqueue->queue_a, 0);
	}else
	if(process->demote >= 3){
		queue_process(process, mqueue->queue_c,0);
	}else{
		if(thecpu->proc != NULL && process->priority > thecpu->proc->priority && mqueue->preempt){
			p_process kicked = thecpu->proc;
			thecpu->proc = process;
			thecpu->decoded = 0;
			thecpu->time_left = mqueue->queue_b->quantum;
			add_process(kicked, mqueue, thecpu);
		}else queue_process(process, mqueue->queue_b, 1);
	}
}

void queue_process(p_process process, p_queue queue, int priority){
	p_node *curr_node = &(queue->head);
	p_node new_node = malloc(sizeof(node));
	new_node->proc = process;
	new_node->next = NULL;
	
	if(!priority){

		while(*curr_node != NULL){
			curr_node = &((*curr_node)->next);
		}
		
		*curr_node = new_node;
	
	}else{

		while(*curr_node != NULL && (*curr_node)->proc->priority > process->priority){
			curr_node = &((*curr_node)->next);
		}
		
		new_node->next = *curr_node;
		*curr_node = new_node;
	}
}

void print_multiqueue(p_multiqueue mqueue){
	printf("A: ");
	print_queue(mqueue->queue_a);
	printf("B: ");
	print_queue(mqueue->queue_b);
	printf("C: ");
	print_queue(mqueue->queue_c);
}

void print_queue(p_queue queue){
	p_node curr_node = queue->head;

	printf("(HEAD) ");
	while(curr_node != NULL){
		printf("P%d (%d), ", curr_node->proc->pid, curr_node->proc->priority);
		curr_node = curr_node->next;
	}
	printf("(TAIL)\n");


}


