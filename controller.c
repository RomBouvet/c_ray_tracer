#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>  
#include <ncurses.h>
#include <sys/msg.h>  
#include <sys/stat.h>     
#include <sys/shm.h>   
#include <unistd.h> 
#include <errno.h>      

#include "controller_utils.h"

#include "ncurses.h"
#include "scene.h"
#include "cst.h"

scene_t *scene;
    WINDOW *bg,*main_window,*info_window_borders,*info_window,*obj_windows[MAX_OBJ],*data[MAX_OBJ][DATA_NB];

void* obj_thread(void *arg){
    int status,*id=(int*) arg;

    wprintw(info_window,"Object %d created\n",*id);

    while(1){ 
        sleep(1);

        status=pthread_mutex_lock(&scene->mutexs[*id]);
        if (status != 0)
            wprintw(info_window, "Probleme lock mutex %d\n",*id);

        sphere_move(scene,*id,info_window);
        display_obj(*id,scene,data);
        wrefresh(obj_windows[*id]);

        wprintw(info_window,"Object %d is moving (%f,%f,%f)...\n",*id,scene->objs[*id].center.x,scene->objs[*id].center.y,scene->objs[*id].center.z);
        wrefresh(info_window);

        pthread_cond_broadcast(&scene->is_free[*id]);
        status=pthread_mutex_unlock(&scene->mutexs[*id]);
        if (status != 0)
            wprintw(info_window,"Probleme unlock mutex %d\n",*id);
        wrefresh(info_window);
        
         
    }
    return NULL;
}

int main(int argc, char *argv[]){
    int i,j,msgid,shmid,mouse_x,mouse_y,button,ch,buf=6,status,ids[MAX_OBJ];

    char static_data[9]={'x','y','z','r','c','x','y','z','v'};
    void *scene_pointer;

    pthread_t obj_threads[MAX_OBJ];

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);

    /*** message's queue creation ***/
	if((msgid=msgget((key_t)MSG_KEY, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL))==-1){
		if(errno==EEXIST)
			fprintf(stderr,"Queue ( key=%d ) already exist.\n",MSG_KEY);
		else
			perror("Error while creating message's queue\n");
		exit(EXIT_FAILURE);
	}

    if((shmid = shmget((key_t)SHM_KEY, 0, 0)) != -1) {
        /* Suppression du segment de memoire partagee */
        if(shmctl(shmid, IPC_RMID, 0) == -1) {
            perror("Erreur lors de la suppression du segment de memoire partagee ");
            exit(EXIT_FAILURE);
        }
    }

    /* Creation du segment contenant la scene */
    if((shmid = shmget((key_t)SHM_KEY, sizeof(scene_t), S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
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

        data[i][9]=display_newdata(obj_windows[i],i,9);
        data[i][10]=display_newdata(obj_windows[i],i,10);
       
        wrefresh(obj_windows[i]);
    }
    
	wprintw(info_window,"CONTROLLER STARTED\n");
	wrefresh(info_window);

	/*** Sending start message ***/
	if(msgsnd(msgid,&buf,sizeof(buf),0)==-1){
		perror("Error while sending message ");
	} else {
		wprintw(info_window,"MESSAGE SENT TO MESSAGE'S QUEUE\n");
	}	
    
	wrefresh(info_window);
	while((ch = getch()) != KEY_F(2)){	
        switch(ch){
            case KEY_MOUSE:
                if(souris_getpos(&mouse_x, &mouse_y, &button) == OK) {
                    if(mouse_y>4 && mouse_y<=4+MAX_OBJ){
                        status=pthread_mutex_lock(&scene->mutexs[mouse_y-5]);
                        if (status != 0)
                            wprintw(info_window, "Probleme lock mutex %d\n",mouse_y-5);
                        else if(mouse_y-5==0)
                            wprintw(info_window, "Locked mutex %d -> ",mouse_y-5);

                        switch(mouse_x){
                            case 15:
                            case 16:
                            case 17:
                            case 18:
                                if(button & BUTTON1_CLICKED){
                                    incr_data(scene,mouse_y-5,0);
                                }
                                if(button & BUTTON3_CLICKED){
                                    decr_data(scene,mouse_y-5,0);
                                }
                                werase(data[mouse_y-5][0]);
                                wprintw(data[mouse_y-5][0],"%.2f",scene->objs[mouse_y-5].center.x);
                                wrefresh(data[mouse_y-5][0]);
                                break;

                            case 22:
                            case 23:
                            case 24:
                            case 25:
                                if(button & BUTTON1_CLICKED){
                                    incr_data(scene,mouse_y-5,1);
                                }
                                if(button & BUTTON3_CLICKED){
                                    decr_data(scene,mouse_y-5,1);
                                }
                                werase(data[mouse_y-5][1]);
                                wprintw(data[mouse_y-5][1],"%.2f",scene->objs[mouse_y-5].center.y);
                                wrefresh(data[mouse_y-5][1]);
                                break;

                            case 29:
                            case 30:
                            case 31:
                            case 32:
                                if(button & BUTTON1_CLICKED){
                                    incr_data(scene,mouse_y-5,2);
                                }
                                if(button & BUTTON3_CLICKED){
                                    decr_data(scene,mouse_y-5,2);
                                }
                                werase(data[mouse_y-5][2]);
                                wprintw(data[mouse_y-5][2],"%.2f",scene->objs[mouse_y-5].center.z);
                                wrefresh(data[mouse_y-5][2]);
                                break;

                            case 36:
                            case 37:
                            case 38:
                            case 39:
                                if(button & BUTTON1_CLICKED){
                                    incr_data(scene,mouse_y-5,3);
                                }
                                if(button & BUTTON3_CLICKED){
                                    decr_data(scene,mouse_y-5,3);
                                };
                                werase(data[mouse_y-5][3]);
                                wprintw(data[mouse_y-5][3],"%.2f",scene->objs[mouse_y-5].radius);
                                wrefresh(data[mouse_y-5][3]);
                                break;
                            
                            case 43:
                            case 44:
                            case 45:
                            case 46:
                                incr_data(scene,mouse_y-5,4);
                                wbkgd(data[mouse_y-5][4],COLOR_PAIR(scene->objs[mouse_y-5].color));
                                wrefresh(data[mouse_y-5][4]);
                                break;
                            
                            case 50:
                            case 51:
                            case 52:
                            case 53:
                                if(button & BUTTON1_CLICKED){
                                    incr_data(scene,mouse_y-5,5);
                                }
                                if(button & BUTTON3_CLICKED){
                                    decr_data(scene,mouse_y-5,5);
                                };
                                werase(data[mouse_y-5][5]);
                                wprintw(data[mouse_y-5][5],"%.2f",scene->directions[mouse_y-5].x);
                                wrefresh(data[mouse_y-5][5]);
                                break;
                            
                            case 57:
                            case 58:
                            case 59:
                            case 60:
                                if(button & BUTTON1_CLICKED){
                                    incr_data(scene,mouse_y-5,6);
                                }
                                if(button & BUTTON3_CLICKED){
                                    decr_data(scene,mouse_y-5,6);
                                };
                                werase(data[mouse_y-5][6]);
                                wprintw(data[mouse_y-5][6],"%.2f",scene->directions[mouse_y-5].y);
                                wrefresh(data[mouse_y-5][6]);
                                break;

                            case 64:
                            case 65:
                            case 66:
                            case 67:
                                if(button & BUTTON1_CLICKED){
                                    incr_data(scene,mouse_y-5,7);
                                }
                                if(button & BUTTON3_CLICKED){
                                    decr_data(scene,mouse_y-5,7);
                                };
                                werase(data[mouse_y-5][7]);
                                wprintw(data[mouse_y-5][7],"%.2f",scene->directions[mouse_y-5].z);
                                wrefresh(data[mouse_y-5][7]);
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
                                add_remove(scene,mouse_y-5);
                                if(scene->empty[mouse_y-5]==FALSE){
                                    ids[mouse_y-5]=mouse_y-5;
                                    wbkgd(data[mouse_y-5][9],COLOR_PAIR(5));
                                    pthread_create(&obj_threads[mouse_y-5],NULL,obj_thread,&ids[mouse_y-5]);
                                }else{
                                    wbkgd(data[mouse_y-5][9],COLOR_PAIR(6));
                                    pthread_cancel(obj_threads[mouse_y-5]);
                                }
                                /*if(scene->empty[mouse_y-5]==TRUE){
                                    wprintw(info_window,"Removed object %d\n",mouse_y-5);
                                    
                                }else{
                                    wprintw(info_window,"Added object %d\n",mouse_y-5);
                                    wbkgd(data[mouse_y-5][9],COLOR_PAIR(5));
                                }*/
                                wrefresh(data[mouse_y-5][9]);
                                break;

                            case 85:
                            case 86:
                            case 87:
                            case 88:
                            	  wprintw(info_window,"PAUSE/RESUME OBJECT %d\n",mouse_y-5);
                                break;
                        }

                        pthread_cond_broadcast(&scene->is_free[mouse_y-5]);
                        status=pthread_mutex_unlock(&scene->mutexs[mouse_y-5]);
                        if (status != 0)
                            wprintw(info_window, "Probleme lock mutex %d\n",mouse_y-5);
                    }
                }
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

    for(i=0;i<MAX_OBJ;i++){
        if(scene->empty[i]!=TRUE){
            pthread_cancel(obj_threads[i]);
            printf("Thread %d destroyed\n",i);
        }
    }

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

	return EXIT_SUCCESS;
}
