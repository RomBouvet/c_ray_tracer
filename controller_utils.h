#ifndef __CONTROLLER_UTILS_H__
#define __CONTROLLER_UTILS_H__ 1

#include "scene.h"

int exists(scene_t *scene,unsigned int id);

void add_remove(scene_t *scene,int id);

void changeData(scene_t *scene, int obj_id, int data_id);

void incr_data(scene_t *scene, int obj_id, int data_id);

void decr_data(scene_t *scene, int obj_id, int data_id);

#endif