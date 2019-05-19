#ifndef MQUEUE
#define MQUEUE
 
#include "Simulation.h"

typedef struct multiqueue_st multiqueue;
typedef multiqueue *p_multiqueue;

typedef struct queue_st queue;
typedef queue *p_queue;

typedef struct node_st node;
typedef node *p_node;


p_multiqueue new_multiqueue(int qA, int qB, int Qc, int preempt);
p_queue new_queue(int quantum);
void add_process(p_process proc, p_multiqueue mqueue, p_cpu thecpu);
void queue_process(p_process process, p_queue queue, int priority);
void print_multiqueue(p_multiqueue mqueue);
void print_queue(p_queue queue);

#endif
