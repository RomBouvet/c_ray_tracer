#ifndef __CONTROLLER_UTILS_H__
#define __CONTROLLER_UTILS_H__ 1

#include "scene.h"

void create_scene(scene_t *scene);

int exists(scene_t *scene,unsigned int id);

void add_remove(scene_t *scene,int id);

void changeData(scene_t *scene, int obj_id, int data_id);

#endif