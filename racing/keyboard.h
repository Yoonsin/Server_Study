#ifndef KEYBOARD_H
#define KEYBOARD_H

extern struct termios initial_settings, new_settings;
extern unsigned char peek_character[2];

void init_keyboard(); //기존 키보드 설정 백업
void close_keyboard(); //기존 키보드 설정 적용 

int linux_kbhit(); //키보드 입력 판별

char* linux_getch(); //블로킹없이 바로 입력값 읽음

int linux_putch(int c);

#endif
