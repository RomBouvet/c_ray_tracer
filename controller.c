#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>  
#include <ncurses.h>
#include <sys/msg.h>  
#include <sys/stat.h>     
#include <sys/shm.h>   
#include <unistd.h> 
#include <errno.h>      


#include "ncurses.h"
#include "scene.h"
#include "controller.h"
#include "cst.h"

/**
 * Create the scene.
 * @param scene the scene
 */
void create_scene(scene_t *scene) {
    area_t area = { -30., 30., -30., 30., -30, 30. }; 
    vector_t camera = { 0., 0., -30 };

    scene_initialize(scene, &area, &camera, 0.018);
}

int main(int argc, char *argv[]){
    int i,j,msgid,shmid,mouse_x,mouse_y,button,ch,buf=6;
    WINDOW *bg,*main_window,*info_window_borders,*info_window,*obj_windows[MAX_OBJ],*data[MAX_OBJ][DATA_NB];
    char static_data[9]={'x','y','z','r','c','x','y','z','v'};
    void *scene_pointer;
    scene_t *scene;

    /*** message's queue creation ***/
	if((msgid=msgget((key_t)MSG_KEY, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL))==-1){
		if(errno==EEXIST)
			fprintf(stderr,"Queue ( key=%d ) already exist.\n",MSG_KEY);
		else
			perror("Error while creating message's queue\n");
		exit(EXIT_FAILURE);
	}

    /* Creation du segment contenant la scene */
    if((shmid = shmget((key_t)SHM_KEY, sizeof(scene_t), S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST)
        fprintf(stderr, "Le segment de memoire partagee (cle=%d) existe deja\n", SHM_KEY);
        else
            perror("Erreur lors de la creation du segment de memoire ");
        exit(EXIT_FAILURE);
    }

    /* Attachement du segment de memoire partagee */
    if((scene_pointer = shmat(shmid, NULL, 0)) == (void*)-1) {
        perror("Erreur lors de l'attachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    scene=scene_pointer;

    create_scene(scene);

    ncurses_initialize();
    ncurses_colors();

    if(!ncurses_checksize(MAX_OBJ+20,MIN_WIDTH)){
        ncurses_stop();
        exit(EXIT_FAILURE);
    }

    bg = newwin(MAX_OBJ+21,MIN_WIDTH,0,0);
    wbkgd(bg,COLOR_PAIR(2));
    mvwprintw(bg,1,(MIN_WIDTH/2)-8,"# CONTROLLER #");
    wrefresh(bg);
        
        main_window = subwin(bg,MAX_OBJ+6, MIN_WIDTH-2, 2, 1);
        box(main_window,0,0);
        wbkgd(main_window,COLOR_PAIR(1));
        mvwprintw(main_window,1,((MIN_WIDTH-2)/2)-7,"~ OBJECTS ~");
        wrefresh(main_window);

        info_window_borders = subwin(bg,12, MIN_WIDTH-2, MAX_OBJ+8, 1);
        box(info_window_borders,0,0);
        wbkgd(info_window_borders,COLOR_PAIR(1));
        mvwprintw(info_window_borders,1,((MIN_WIDTH-2)/2)-10,"~ INFORMATIONS ~");
        wrefresh(info_window_borders);
        
            info_window = subwin(info_window_borders,8, 86, MAX_OBJ+11, 3);
            scrollok(info_window,1);
            wrefresh(info_window);

    for(i=0;i<MAX_OBJ;i++){
        obj_windows[i]=display_newobj(i,MIN_WIDTH-5);
        wbkgd(obj_windows[i],COLOR_PAIR((i%2)+1));
        wprintw(obj_windows[i],"object %d : ",i);
        for(j=0;j<9;j++){
            mvwprintw(obj_windows[i],0,11+(7*j),"%c",static_data[j]);
            
            data[i][j]=display_newdata(obj_windows[i],i,j);
        }

        data[i][j]=display_newdata(obj_windows[i],i,9);
        data[i][j]=display_newdata(obj_windows[i],i,10);
       
        wrefresh(obj_windows[i]);
    }
    
	wprintw(info_window,"CONTROLLER STARTED\n");
	wrefresh(info_window);

	/*** Sending start message ***/
	if(msgsnd(msgid,&buf,sizeof(buf),0)==-1){
		perror("Error while sending message ");
	} else {
		wprintw(info_window,"STARTING - MESSAGE SENT TO MESSAGE'S QUEUE\n");
	}	
    
	wrefresh(info_window);
	while((ch = getch()) != KEY_F(2)){	
        switch(ch){
            case KEY_MOUSE:
                if(souris_getpos(&mouse_x, &mouse_y, &button) == OK) {
                    if(mouse_y>4 && mouse_y<=4+MAX_OBJ){
                        switch(mouse_x){
                            case 15:
                            case 16:
                            case 17:
                            case 18:
                                wprintw(info_window,"X INPUT (%d)\n",mouse_y-5);
                                break;

                            case 22:
                            case 23:
                            case 24:
                            case 25:
                                wprintw(info_window,"Y INPUT\n");
                                break;

                            case 29:
                            case 30:
                            case 31:
                            case 32:
                                wprintw(info_window,"Z INPUT\n");
                                break;

                            case 36:
                            case 37:
                            case 38:
                            case 39:
                                wprintw(info_window,"R INPUT\n");
                                break;
                            
                            case 43:
                            case 44:
                            case 45:
                            case 46:
                                wprintw(info_window,"C INPUT\n");
                                break;
                            
                            case 50:
                            case 51:
                            case 52:
                            case 53:
                                wprintw(info_window,"X INPUT\n");
                                break;
                            
                            case 57:
                            case 58:
                            case 59:
                            case 60:
                                wprintw(info_window,"Y INPUT\n");
                                break;

                            case 64:
                            case 65:
                            case 66:
                            case 67:
                                wprintw(info_window,"Z INPUT\n");
                                break;

                            case 71:
                            case 72:
                            case 73:
                            case 74:
                                wprintw(info_window,"V INPUT\n");
                                break;

                            case 78:
                            case 79:
                            case 80:
                            case 81:
                                wprintw(info_window,"ADD/DELETE\n");
                                break;

                            case 85:
                            case 86:
                            case 87:
                            case 88:
                            	  wprintw(info_window,"PAUSE/RESUME OBJECT %d\n",mouse_y-5);
                                break;
                        }
                    }
                        
                    /*if(button & BUTTON1_CLICKED){
                        wprintw(info_window,"Clic bouton 1 a la position (%d, %d)\n", mouse_x, mouse_y);
                    }
                    if(button & BUTTON3_CLICKED){
                        wprintw(info_window,"Clic bouton 3 a la position (%d, %d)\n", mouse_x, mouse_y);
                    }*/
                }
                wrefresh(info_window);
                break;
            case KEY_UP:
                wprintw(info_window,"UP Pressed\n");
                wrefresh(info_window);
                break;
        }
	}

	destroy_win(bg);
    destroy_win(main_window);
    destroy_win(info_window_borders);
    destroy_win(info_window);

    for(i=0;i<MAX_OBJ;i++){
        for(j=0;j<9;j++){
            destroy_win(data[i][j]);
        }
        destroy_win(obj_windows[i]);
    }

    ncurses_stop();

    /* Detachement du segment de memoire partagee */
    if(shmdt(scene_pointer) == -1) {
        perror("Erreur lors du detachement ");
        exit(EXIT_FAILURE);
    }

    /*** queue delete ***/
    if(msgctl(msgid, IPC_RMID, 0) == -1) {
        perror("Error while deleting message's queue ");
        exit(EXIT_FAILURE);
    }

	return 0;
}
