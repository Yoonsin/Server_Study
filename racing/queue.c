#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void init(queue *q){
	
	q->front = q->rear = 0;
}

int is_empty(queue *q){
	
	return (q->front == NULL);
}

int is_full(queue *q){
	return 0;
}

void enqueue(queue *q, packet data ){
	
	queue_node *temp = (queue_node *)malloc(sizeof(queue_node));
	temp->data = data;
	temp->link = NULL;
	
	if(is_empty(q)){
		q->front = temp;
		q->rear = temp;
	}else {
		q->rear->link = temp;
		q->rear = temp;
		
	}
}

void dequeue(queue *q){
	queue_node *temp = q->front;
	if(is_empty(q)){
		fprintf(stderr,"스택이 비어있음");
		exit(1);
	}else{
		q->front = q->front->link;
		if(q->front == NULL)
		     q->rear = NULL;
		  
		free(temp);
	}
	
}

packet seek(queue *q){
	queue_node *temp = q->front;
	packet data;
	if(is_empty(q)){
		fprintf(stderr,"스택이 비어있음");
		exit(1);
	}else{
		data = temp->data;
		return data;		 
	}
	
}
