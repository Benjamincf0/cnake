#ifndef TC_H
#define TC_H

#define TC_B_NRM "\x1B[0m"
#define TC_B_RED "\x1B[1;31m"

#define TC_BG_NRM "\x1B[40m"
#define TC_BG_RED "\x1B[41m"
#define TC_BG_WHT "\x1B[47m"

#define tc_clear_screen() puts("\x1B[2J")

#define tc_move_cursor(X, Y) printf("\033[%d;%dH]", Y, X)

#define tc_enter_alt_screen() puts("\033[?1049h\033[H")
#define tc_exit_alt_screen() puts("\033[?1049l")

#endif
