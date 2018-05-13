#include <stdlib.h>

#include "controller_utils.h"

int exists(scene_t *scene,unsigned int id){
    if(scene->empty[id]==TRUE)
        return FALSE;
    return TRUE;
}

void add_remove(scene_t *scene,int id){
    if(!exists(scene,id)){
        scene_add_new(scene,id);
    } else{
        scene_remove(scene,id);
    }
}

void incr_data(scene_t *scene, int obj_id, int data_id){
    switch(data_id){
        case 0:
            scene->objs[obj_id].center.x++;
            break;
        case 1:
            scene->objs[obj_id].center.y++;
            break;
        case 2:
            scene->objs[obj_id].center.z++;
            break;
        case 3:
            scene->objs[obj_id].radius++;
            break;
        case 4:
            if(scene->objs[obj_id].color>=6 || scene->objs[obj_id].color<3)
                scene->objs[obj_id].color=3;
            else
                scene->objs[obj_id].color++;
            break;
        case 5:
            scene->directions[obj_id].x++;
            break;
        case 6:
            scene->directions[obj_id].y++;
            break;
        case 7:
            scene->directions[obj_id].z++;
            break;
        case 8:
            scene->speed[obj_id]++;
            break;
    }
}

void decr_data(scene_t *scene, int obj_id, int data_id){
    switch(data_id){
        case 0:
            scene->objs[obj_id].center.x--;
            break;
        case 1:
            scene->objs[obj_id].center.y--;
            break;
        case 2:
            scene->objs[obj_id].center.z--;
            break;
        case 3:
            if(scene->objs[obj_id].radius>1)
                scene->objs[obj_id].radius--;
            break;
        case 5:
            scene->directions[obj_id].x--;
            break;
        case 6:
            scene->directions[obj_id].y--;
            break;
        case 7:
            scene->directions[obj_id].z--;
            break;
        case 8:
            if(scene->speed[obj_id]>1)
                scene->speed[obj_id]--;
            break;
    }
}