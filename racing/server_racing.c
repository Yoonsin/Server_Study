#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
#include<time.h>
#include<string.h>
#include"queue.h"

#define BUF_SIZE 1024
#define MAX_CLNT 100
#define MAX_IP 30
#define ROW 6 //track vertical
#define COL 25 //track horizon

int itemX = COL/2;
int itemY = ROW/2;
int itemFlag;

void * handle_clnt(void *arg);
void send_msg(int clnt_sock,char *msg, int len);
void error_handling(char *msg);
char* serverState(int count);
void menu(char port[]);
 
 
/****************************/
 
int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;
 
    /** time log **/
    struct tm *t;
    time_t timer = time(NULL);
    t=localtime(&timer);
 
    if (argc != 2)
    {
        printf(" Usage : %s <port>\n", argv[0]);
        exit(1);
    }
 
    menu(argv[1]);
 
    pthread_mutex_init(&mutx, NULL);
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));
 
    if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
        error_handling("bind() error");
    if (listen(serv_sock, 5)==-1)
        error_handling("listen() error");
 
    while(1)
    {
        t=localtime(&timer);
        clnt_adr_sz=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
 
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++]=clnt_sock;
        pthread_mutex_unlock(&mutx);
 
        pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);
        pthread_detach(t_id);
        printf(" Connceted client IP : %s ", inet_ntoa(clnt_adr.sin_addr));
        printf("(%d-%d-%d %d:%d)\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday,
        t->tm_hour, t->tm_min);
        printf(" chatter (%d/100)\n", clnt_cnt);
    }
    close(serv_sock);
    return 0;
}
 
void *handle_clnt(void *arg)
{
    int clnt_sock=*((int*)arg);
    int str_len=0, i;
    char msg[BUF_SIZE];
 
    while((str_len=read(clnt_sock, msg, sizeof(msg)))>0){
        
	 msg[str_len] = '\0';
	 //printf("%d\n",str_len);

	 /*
	 if(!itemFlag){
	   packet* recvPacket = (packet*)msg;
	   packet temp = *recvPacket;
           if(temp.x==itemX&&temp.y==itemY){
            printf("[%s] get Item!\n",temp.name);
	    itemFlag = 1;
	   }
	 }*/

         send_msg(clnt_sock,msg, str_len);

    }
    
    // remove disconnected client
    pthread_mutex_lock(&mutx);
    for (i=0; i<clnt_cnt; i++)
    {
        if (clnt_sock==clnt_socks[i])
        {
            while(i++<clnt_cnt-1)
                clnt_socks[i]=clnt_socks[i+1];
            break;
        }
    }
    clnt_cnt--;
    pthread_mutex_unlock(&mutx);
    close(clnt_sock);
    return NULL;
}
 
void send_msg(int clnt_sock,char* msg, int len)
{
    int i;
    int copySize = sizeof(packet);
    packet* recvPacket = (packet*)msg;
    packet temp = *recvPacket;
    
    pthread_mutex_lock(&mutx);
    for (i=0; i<clnt_cnt; i++){
       int remainByte = len;     
       int srcPos = 0;
       if(clnt_socks[i] != clnt_sock){ 
          //자신을 제외한 모두에게 브로드 캐스트한다(자신은 이미 로컬에서 자리 옮김) 
          while(remainByte > 0){
	     printf("%d %d\n", len,remainByte);
             char cutMsg[100];
	     memcpy(cutMsg, msg+srcPos, copySize);
	     cutMsg[copySize] = '\0';

	     write(clnt_socks[i], cutMsg, copySize);
	     
	     srcPos += copySize;
	     remainByte -= copySize;
          }
       }
    }
    pthread_mutex_unlock(&mutx);

     if(strncmp(temp.name,"link",4)==0){
	    //젤다쪽이 느려짐
	     usleep(300000); //뮤텍스 때문에 젤다쪽은 쓰지도 못하는듯...그럼 뮤텍스 바깥으로 옮겨보자
     }
}
 
void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
 
char* serverState(int count)
{
    char* stateMsg = malloc(sizeof(char) * 20);
    strcpy(stateMsg ,"None");
    
    if (count < 5)
        strcpy(stateMsg, "Good");
    else
        strcpy(stateMsg, "Bad");
    
    return stateMsg;
}        
 
void menu(char port[])
{
    system("clear");
    printf(" **** moon/sun chat server ****\n");
    printf(" server port    : %s\n", port);
    printf(" server state   : %s\n", serverState(clnt_cnt));
    printf(" max connection : %d\n", MAX_CLNT);
    printf(" ****          Log         ****\n\n");
}

