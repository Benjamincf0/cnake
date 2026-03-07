#ifndef TC_H
#define TC_H

#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define TC_B_NRM "\x1B[0m"
#define TC_B_RED "\x1B[1;31m"

#define TC_BG_NRM "\x1B[40m"
#define TC_BG_RED "\x1B[41m"
#define TC_BG_WHT "\x1B[47m"

// #define tc_clear_screen() puts("\x1B[2J")
#define tc_clear_screen() write(1, "\033[H\033[2J\033[3J", 11)

#define tc_move_cursor(X, Y) printf("\033[%d;%dH", Y, X)

#define tc_enter_alt_screen() puts("\033[?1049h\033[H")
#define tc_exit_alt_screen() puts("\033[?1049l")

#define tc_hide_cursor() printf("\033[?25l");
#define tc_show_cursor() printf("\033[?25h");

void tc_get_cols_rows(int *cols, int *rows);

void tc_get_cols_rows(int *cols, int *rows) {
  struct winsize size;
  ioctl(1, TIOCGWINSZ, &size);
  *cols = size.ws_col;
  *rows = size.ws_row;
}

void tc_echo_off();
void tc_echo_on();

void tc_echo_off() {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag &= ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void tc_echo_on() {
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag |= ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void tc_getc_block_on();

void tc_getc_block_on() {
  struct termios newSettings;
  tcgetattr(STDIN_FILENO, &newSettings);
  newSettings.c_cc[VMIN] = 1;  // dont wait for chars
  newSettings.c_cc[VTIME] = 0; // return right away
  tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

void tc_getc_block_off();

void tc_getc_block_off() {
  struct termios newSettings;
  tcgetattr(STDIN_FILENO, &newSettings);
  newSettings.c_cc[VMIN] = 0;  // dont wait for chars
  newSettings.c_cc[VTIME] = 0; // return right away
  tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

void tc_canonical_on();

void tc_canonical_on() {
  struct termios newSettings;
  tcgetattr(STDIN_FILENO, &newSettings);
  newSettings.c_lflag |= ICANON;
  tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

void tc_canonical_off();

void tc_canonical_off() { // a.k.a. raw ;) no buffering
  struct termios newSettings;
  tcgetattr(STDIN_FILENO, &newSettings);
  newSettings.c_lflag &= ~ICANON;
  tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

#endif
