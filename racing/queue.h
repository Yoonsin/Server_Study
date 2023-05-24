#ifndef QUEUE
#define QUEUE //한번만 컴파일 시키는 전처리기

//packet;
typedef struct packet{	
	int x,y;
	//추후 데이터 추가예정...
}packet;


//packet_queue
typedef struct queue_node{
	packet data;
	struct queue_node *link;	
}queue_node;


typedef struct queue {
	queue_node *front,*rear;
}queue;

void init(queue *q);

int is_empty(queue *q);

int is_full(queue *q);

void enqueue(queue *q, packet data );

void dequeue(queue *q);

packet seek(queue *q);

#endif
