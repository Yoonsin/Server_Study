#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
/*
//packet queue;
typedef struct packet{	
	int x,y;
	//추후 데이터 추가예정...
}packet;

typedef struct queue_node{
	packet data;
	struct queue_node *link;	
}node;


typedef struct queue {
	queue_node *front,*rear;
}queue;

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
*/
int main(int argc, char *argv[])
{
	queue q;
	init(&q);
	
	int n;
	while(1){
		printf("1. enqueue 2. dequeue 3. seek\n");
		scanf("%d",&n);
		scanf("%*c");
		if(n==1){
			int x,y;
			printf("input x,y >>");
			scanf("%d %d",&x, &y); 
			packet temp;
			temp.x = x; temp.y =y;
			enqueue(&q,temp);
			
		}else if(n==2){
			dequeue(&q);
			printf("dequeue sucess\n");
			
		}else if(n==3){
			packet temp = seek(&q);
		printf("front x : %d , y : %d\n", temp.x,temp.y);
			
		}else{
			
			printf("quit...\n");
			
		}		
		//scanf("%*c");
	}
}
