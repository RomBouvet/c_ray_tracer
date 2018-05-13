#include <ncurses.h>
#include <stdlib.h>

#include "ncurses.h"

/**
* Initialization of ncurses.
*/
void ncurses_initialize() {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	refresh();
	curs_set(FALSE);
	mousemask(ALL_MOUSE_EVENTS, NULL);
}

/**
* Stop ncurses mode.
*/
void ncurses_stop() {
	endwin();
}

/**
* Initialization of colors.
*/
void ncurses_colors() {
	/* Check colors support */
	if(has_colors() == FALSE) {
		ncurses_stop();
		fprintf(stderr, "No color support for this terminal.\n");
		exit(EXIT_FAILURE);
	}

	/* Activate colors */
	start_color();

	init_pair(1,COLOR_WHITE, COLOR_BLACK);
	init_pair(2,COLOR_BLACK, COLOR_WHITE);

	init_pair(3,COLOR_WHITE, COLOR_MAGENTA);
	init_pair(4,COLOR_BLACK, COLOR_YELLOW);

	init_pair(5,COLOR_WHITE, COLOR_GREEN);
	init_pair(6,COLOR_WHITE, COLOR_RED);

	init_pair(7,COLOR_WHITE, COLOR_CYAN);
	init_pair(8,COLOR_WHITE, COLOR_BLUE);
}

/**
* Check the sizes of the terminal.
* @param height the height needed
* @param width the width needed
* @return TRUE if the sizes are OK else returns FALSE
*/
int ncurses_checksize(int height, int width) {
	if((COLS < width) || (LINES < height))
		return FALSE;

	return TRUE;
}

WINDOW *create_newwin(int height, int width, int starty, int startx, int bordered){	
	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	if(bordered)
		box(local_win, 0 , 0);
	wrefresh(local_win);

	return local_win;
}

void destroy_win(WINDOW *local_win){	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	* result of erasing the window. It will leave it's four corners 
	* and so an ugly remnant of window. 
	*/
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are 
	* 1. win: the window on which to operate
	* 2. ls: character to be used for the left side of the window 
	* 3. rs: character to be used for the right side of the window 
	* 4. ts: character to be used for the top side of the window 
	* 5. bs: character to be used for the bottom side of the window 
	* 6. tl: character to be used for the top left corner of the window 
	* 7. tr: character to be used for the top right corner of the window 
	* 8. bl: character to be used for the bottom left corner of the window 
	* 9. br: character to be used for the bottom right corner of the window
	*/
	wrefresh(local_win);
	delwin(local_win);
}

WINDOW *display_newobj(int i,int width){
	return create_newwin(1,width,i+5,2,NO_BORDERS);
}

WINDOW *display_newdata(WINDOW* master,int i,int index){
	WINDOW *ret;
	ret=subwin(master,1,4,i+5,15+(7*index));

	switch(index){
		case 4:
			wbkgd(ret,COLOR_PAIR(((i+1)%5)+3));
			break;
		case 9:
			wbkgd(ret,COLOR_PAIR(6));
			break;
		case 10:
			mvwprintw(ret,0,1,"||");
			wbkgd(ret,COLOR_PAIR(((i+1)%2)+7));
			break;
		default:
			wbkgd(ret,COLOR_PAIR((i%2)+3));
			break;
	}

	wrefresh(ret);
	return ret;
}

void display_obj(int id, scene_t *scene,WINDOW *data[MAX_OBJ][DATA_NB]){
	if(id>=MAX_OBJ)
		return;
	
	werase(data[id][0]);
	wprintw(data[id][0],"%.2f",scene->objs[id].center.x);
	wrefresh(data[id][0]);

	werase(data[id][1]);
	wprintw(data[id][1],"%.2f",scene->objs[id].center.y);
	wrefresh(data[id][1]);

	werase(data[id][2]);
	wprintw(data[id][2],"%.2f",scene->objs[id].center.z);
	wrefresh(data[id][2]);

	werase(data[id][3]);
	wprintw(data[id][3],"%.2f",scene->objs[id].radius);
	wrefresh(data[id][3]);

	wbkgd(data[id][4],COLOR_PAIR(scene->objs[id].color));
	wrefresh(data[id][4]);

	werase(data[id][5]);
	wprintw(data[id][5],"%.2f",scene->directions[id].x);
	wrefresh(data[id][5]);

	werase(data[id][6]);
	wprintw(data[id][6],"%.2f",scene->directions[id].y);
	wrefresh(data[id][6]);

	werase(data[id][7]);
	wprintw(data[id][7],"%.2f",scene->directions[id].z);
	wrefresh(data[id][7]);

	werase(data[id][8]);
	wprintw(data[id][8],"%d",scene->speed[id]);
	wrefresh(data[id][8]);

	werase(data[id][10]);
	if(scene->moving[id]==TRUE){
		mvwprintw(data[id][10],0,1,"||");
	}
	else{
		mvwprintw(data[id][10],0,1,">");
	}
	wrefresh(data[id][10]);
}

int souris_getpos(int *x, int *y, int *bouton) {
	MEVENT event;
	int resultat = getmouse(&event);

	if(resultat == OK) {
		*x = event.x;
		*y = event.y;
		*bouton = event.bstate;
	}
	return resultat;
}
 