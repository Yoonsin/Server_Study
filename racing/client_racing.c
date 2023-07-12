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
#define ENEMY_NUM 7

//8-directions code
#define N 119
#define E 100
#define S 115
#define W 97
#define NE 219
#define SE 215
#define SW 212
#define NW 216

//quit code
#define QUIT 113

void update(); //fuc
void inputCart();
void* recv_msg(void* arg);
void* send_msg(void* arg);

int track[ROW][COL]; //track

int cartX,cartY; //my cart
int dirX,dirY;
int dx[2] = {0,1};
int dy[3] = {0,1,-1};

typedef struct enemy{
	int x,y;
	char name[NORMAL_SIZE];
}enemy;
enemy eme[ENEMY_NUM];
int emeCnt=0;
int emeX = 1,emeY = 1; //enemy cart
char emeName[NORMAL_SIZE] = "NONE";

int itemX = COL/2, itemY = ROW/2; //item

char clnt_ip[NORMAL_SIZE]; //network value
char serv_port[NORMAL_SIZE];
char name[NORMAL_SIZE]="[DEFALT]";
queue recv_queue, send_queue;

//수신 송신 큐에 대한 뮤텍스
pthread_mutex_t a_lock = PTHREAD_MUTEX_INITIALIZER;
//에너미에 대한 뮤텍스
pthread_mutex_t b_lock = PTHREAD_MUTEX_INITIALIZER;

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

    for(int i=0;i<ENEMY_NUM;i++){ 
	sprintf(eme[i].name,"%s","NONE");
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
        
	printf("my(%s) Cart : %d %d\n",name,cartX,cartY);
	printf("eme(%s) Cart : %d %d\n",emeName,emeX,emeY);

	pthread_mutex_lock(&a_lock);
	if(!is_empty(&recv_queue)){
	   packet eme_cart_data = seek(&recv_queue);
	   dequeue(&recv_queue);
            
	   pthread_mutex_lock(&b_lock);
	   for(int i=0;i<emeCnt;i++)
	   {  printf("eme(%d) cartName : %s\n",i,eme[i].name);
	
	      if(strcmp(eme[i].name,eme_cart_data.name)==0){
		      eme[i].x = eme_cart_data.x;
		      eme[i].y = eme_cart_data.y;
		      //strcpy(eme[i].name,eme_cart_data.name);
		      break;
	      }
           }
	   pthread_mutex_unlock(&b_lock);
	   

	}
        pthread_mutex_unlock(&a_lock); 
        update();	


       
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
     char* move = linux_getch();
     int moveCode = move[0] + move[1];
     printf("move[0] : %d move[1] : %d\n",move[0],move[1]);
     switch(moveCode){
     case N:
	     cartY-=1;
	     break;
     case E:
	     cartX+=1;
	     break;
     case S:
	     cartY+=1;
	     break;
     case W:
	     cartX-=1;
	     break;
     case NE:
	     cartX+=1;
	     cartY-=1;
	     break;
     case SE:
	     cartY+=1;
	     break;
     case SW:
	     cartX-=1;
	     cartY+=1;
	     break;
     case NW:
	     cartX-=1;
	     cartY-=1;
	     break;
     case QUIT:
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

void update()
{
		//print cart
		for(int i=0;i<ROW;i++){
			for(int j=0;j<COL;j++){
		
				if(cartX==j && cartY==i)
				{
					printf("■ ");
			        }else if(itemX ==j && itemY==i){
					printf("☆ ");

                                }
				else{
                                   int isTrack = 1;
				   pthread_mutex_lock(&b_lock);
				   for(int k=0;k<emeCnt;k++){
				       if(eme[k].x==j&&eme[k].y==i&&strcmp(eme[k].name,"NONE")!=0){
					      printf("● ");
					      isTrack = 0;
					   }
				   }  
                                   pthread_mutex_unlock(&b_lock);
				   
				   if(isTrack) printf("□ ");
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
	
 pthread_mutex_lock(&b_lock);
	   int newClnt = 1;
	   for(int i=0;i<emeCnt;i++){
	      if(strcmp(eme[i].name,temp.name)==0){
		 newClnt = 0;
		 break;
	      }
	    }
	   if(newClnt){
             sprintf(eme[emeCnt].name,"%s",temp.name);
	     emeCnt++;
	   }
 pthread_mutex_unlock(&b_lock);

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
 
