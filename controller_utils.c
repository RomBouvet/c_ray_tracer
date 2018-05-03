#include "controller_utils.h"

/**
 * Create the scene.
 * @param scene the scene
 */
void create_scene(scene_t *scene) {
    area_t area = { -30., 30., -30., 30., -30, 30. }; 
    vector_t camera = { 0., 0., -30 };

    scene_initialize(scene, &area, &camera, 0.018);
}

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

void changeData(scene_t *scene, int obj_id, int data_id){
    if(!exists(scene,obj_id))
        return;

    switch(data_id){
        default:
            break;
    }
}