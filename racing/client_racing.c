#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<time.h>
#include "queue.h"
#include "keyboard.h"

#define BUF_SIZE 1024
#define NORMAL_SIZE 10

#define ROW 6 //track vertical
#define COL 25 //track horizon


void update(); //fuc
void inputCart();
void* recv_msg(void* arg);
void* send_msg(void* arg);

int track[ROW][COL]; //track

int cartX,cartY; //my cart
int dirX,dirY;
int dx[2] = {0,1};
int dy[3] = {0,1,-1};

int emeX = 1,emeY = 1; //enemy cart
char emeName[NORMAL_SIZE];

int itemX = COL/2, itemY = ROW/2; //item

char clnt_ip[NORMAL_SIZE]; //network value
char serv_port[NORMAL_SIZE];
char name[NORMAL_SIZE]="[DEFALT]";
queue recv_queue, send_queue;

pthread_mutex_t a_lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
    
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void* thread_return;

    if (argc!=4)
    {
        printf(" Usage : %s <ip> <port> <name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "%s", argv[3]);
    sprintf(clnt_ip, "%s", argv[1]);
    sprintf(serv_port, "%s", argv[2]);
    sock=socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
    {//error_handling(" conncet() error");
    }    
    
    init_keyboard();
    init(&send_queue);
    init(&recv_queue);

    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);


    while(1){

        inputCart(); //change cartX, cartY

	pthread_mutex_unlock(&a_lock);
	if(!is_empty(&recv_queue)){
	   packet eme_cart_data = seek(&recv_queue);
	   dequeue(&recv_queue);
	   emeX = eme_cart_data.x;
	   emeY = eme_cart_data.y;
	   strcpy(emeName,eme_cart_data.name);

	   printf("my(%s) Cart : %d %d\n",name,cartX,cartY);
	   printf("eme(%s) Cart : %d %d\n",emeName,emeX,emeY);
	}
        pthread_mutex_unlock(&a_lock); 
	//아마 여러개 수신큐에서 read할 때 뮤텍스 때문에 못 읽으니까 계속 기다리느라 끊겨서 출력되는거 아닐까

        update(emeX,emeY);	


       
        usleep(30000); //30ms 1000/60 == 30ms (60frame)
        system("clear");

    }
    
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    
    return 0;
}

void inputCart()
{
   if(linux_kbhit()){ 
     int move = linux_getch();
     switch(move){
     case 'w':
	     cartY-=1;
	     break;
     case 'd':
	     cartX+=1;
	     break;
     case 's':
	     cartY+=1;
	     break;
     case 'a':
	     cartX-=1;
	     break;
     case 'q':
	     printf("quit\n");
	     close_keyboard();
	     exit(1);
	     break;

     }

     if(cartX<0) cartX = COL-1;
     if(cartX>=COL) cartX %= COL;
     if(cartY<0) cartY = 0;
     if(cartY>=ROW) cartY = ROW -1;

     //트랙 벗어나는 거 방지!
     
     //움직임이 있을 때만 패킷 보내기
     packet my_cart_data;
     my_cart_data.x = cartX;
     my_cart_data.y = cartY;
     strcpy(my_cart_data.name,name);

     pthread_mutex_lock(&a_lock);
     enqueue(&send_queue,my_cart_data);
     pthread_mutex_unlock(&a_lock);
   }
}

void update(int emeX, int emeY)
{
		//print cart
		for(int i=0;i<ROW;i++){
			for(int j=0;j<COL;j++){
		
				if(cartX==j && cartY==i)
				{
					printf("■ ");
				}else if(emeX==j && emeY==i)
				{
					printf("● ");
				}else if(itemX ==j && itemY==i){
					printf("☆ ");
				}
				else{				
				   printf("□ ");
				}
				
			}
			printf("\n");
		}
                 

}

void* send_msg(void* arg)
{
    int sock = *((int*)arg);

    while(1){
	pthread_mutex_lock(&a_lock);
     if(!is_empty(&send_queue)){
        packet temp = seek(&send_queue);
	dequeue(&send_queue);

        write(sock, (char*)&temp, sizeof(packet));	
        }


	pthread_mutex_unlock(&a_lock);
    }

    return NULL;
}

void* recv_msg(void* arg)
{
    int sock = *((int*)arg);
    char recvBuf[BUF_SIZE];
    int str_len;

    while(1){
       if((str_len = read(sock, recvBuf, sizeof(recvBuf)))>0)
       {
         recvBuf[str_len] = '\0'; //cut bufferSize (prevent trash value)
         int copySize = sizeof(packet);
	 int remainByte = str_len;
	 int srcPos = 0;

	 while(remainByte > 0){
	   //printf("%d %d\n",str_len, remainByte); 
           char cutMsg[100];
	   memcpy(cutMsg, recvBuf+srcPos, copySize);
	   cutMsg[copySize] = '\0';

           packet temp;
	   packet* recvPacket = (packet*)cutMsg;
	   temp = *recvPacket; 
	
	   

	   pthread_mutex_lock(&a_lock);
           enqueue(&recv_queue,temp);
           pthread_mutex_unlock(&a_lock);
 
 
	   srcPos += copySize;
	   remainByte -= copySize;
             
	 }
      }
    }

    return NULL;
}
 
