#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>


struct termios initial_settings, new_settings;
unsigned char peek_character[2];

void init_keyboard()
{
    tcgetattr(0,&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;
    new_settings.c_cc[VMIN] = 1;
    new_settings.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &new_settings);
}
 
void close_keyboard()
{
    tcsetattr(0, TCSANOW, &initial_settings);
}
 
int linux_kbhit()
{
    unsigned char ch[2];
    int nread;
 
    if (peek_character[0] != '\0') return 1;
    new_settings.c_cc[VMIN]=0; //read시 0개의 문자만 읽어도 블로킹없이 바로 리턴 
    tcsetattr(0, TCSANOW, &new_settings);
    nread = read(0,ch,2);
    new_settings.c_cc[VMIN]=1; //읽은 뒤에는 다시 1개 문자로 바꾸기
    tcsetattr(0, TCSANOW, &new_settings);
    if(nread >= 1)
    {
        //peek_character = ch[0];
	memcpy(peek_character, ch, sizeof(peek_character));
        return 1;
    }
    return 0;
}
 
char* linux_getch()
{
    static unsigned char ch[2];
 
    if(peek_character[0] != '\0')
    {
	memcpy(ch, peek_character, sizeof(peek_character));
        memset(peek_character, '\0', sizeof(unsigned char)*2);
        return ch;
    }
    read(0,ch,2);
    return ch;
}
 
int linux_putch(int c) {
    putchar(c); putchar('\n');
    fflush(stdout);
    return c;
}
