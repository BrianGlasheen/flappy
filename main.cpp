#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include <termios.h>
#include <stdlib.h>

struct termios orig_termios;

void raw_mode_off() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void raw_mode_on() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void draw_screen(char* screen, int w, int h) {
    for (int i = 0; i < w + 2; i++){
        printf("=");
    }
    printf("\n");

    for (int i = 0; i < h; i++) {
        printf("|");
        printf("%.*s", w, screen + i * w);
        printf("|");
        printf("\n");
    }

    for (int i = 0; i < w + 2; i++){
        printf("=");
    }
    printf("\n");
}

void clean_screen(char* screen, int w, int h) {
    memset(screen, '-', w * h);
}

int main() {
    struct winsize t;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &t);
    std::cout << "Terminal size: " << t.ws_row << " rows, " << t.ws_col << " columns" << std::endl;

    int w = t.ws_col - 2, h = t.ws_row - 3;
    char screen[w * h];

    raw_mode_on();
    atexit(raw_mode_off);

    int p_y = h / 2;
    int v_y = 0;
    bool playing = true;
    while (playing) {
        char c = '\0';
        read(STDIN_FILENO, &c, 1);

        if (c == ' ') {
            v_y = h / 8;
        }
        p_y += v_y;
        if (p_y > h - 1) {
            p_y = h - 1;
        }

        if (p_y < 0) { // cleaner finish frame
            p_y = 0;
        }
        clean_screen(screen, w, h);

        screen[(h - p_y - 1) * w + 5] = '@';

        draw_screen(screen, w, h);
        // printf("frame %d\n", ++n_frame);
        // usleep(100 * 1000);
        if (p_y <= 0) playing = false;
        v_y -= 1;
        if (v_y <= -2) {
            v_y = -2;
        }
    } 

    return 0;
}
