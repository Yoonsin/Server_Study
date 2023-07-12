#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

void (*old_fun) (int);
int cnt;

void exit_handler(int signo)
{
	printf("종료 로직");
	cnt++;
	if(cnt==3){
	  signal(SIGINT,old_fun);
	 }
}

int main()
{
	old_fun = signal(SIGINT,exit_handler);
	while(1){
		printf("%d\n",cnt);
		sleep(1);
	}
	return 0;

}
