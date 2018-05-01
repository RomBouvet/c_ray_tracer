#ifndef _NCURSES_
#define _NCURSES_

#include <ncurses.h>

/**
 * Initialization of ncurses.
 */
void ncurses_initialize();

/**
 * Stop ncurses mode.
 */
void ncurses_stop();

/**
 * Initialization of colors.
 */
void ncurses_colors();

/**
 * Check the sizes of the terminal.
 * @param height the height needed
 * @param width the width needed
 * @return TRUE if the sizes are OK else returns FALSE
 */
int ncurses_checksize(int height, int width);

WINDOW* create_newwin(int,int,int,int,int);
void destroy_win(WINDOW*);

WINDOW *display_newobj(int i,int width);
WINDOW *display_newdata(WINDOW* master,int i,int index);

int souris_getpos(int *x, int *y, int *bouton);

#endif
