#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include "../include/input.h"

static struct termios orig_termios;
static bool held[256];

void input_init(void) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    memset(held, 0, sizeof(held));
}

void input_cleanup(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    fputs("\033[?25h", stdout);
}

int input_poll(void) {
    unsigned char buf[4] = {0};
    int n = (int)read(STDIN_FILENO, buf, sizeof(buf));
    if (n <= 0) return KEY_NONE;

    int key = KEY_NONE;

    if (n == 1) {
        key = buf[0];
    } else if (n >= 3 && buf[0]==27 && buf[1]=='[') {
        switch(buf[2]){
            case 'A': key=KEY_UP;    break;
            case 'B': key=KEY_DOWN;  break;
            case 'C': key=KEY_LEFT; break;
            case 'D': key=KEY_RIGHT;  break;
        }
    } else if (n >= 3 && buf[0]==27 && buf[1]=='O') {
        switch(buf[2]){
            case 'P': key=KEY_F1; break;
            case 'Q': key=KEY_F2; break;
            case 'R': key=KEY_F3; break;
        }
    }

    if (key > 0 && key < 256) held[key] = true;
    return key;
}

bool input_held(int key) {
    if (key < 0 || key >= 256) return false;
    return held[key];
}
