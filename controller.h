#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "cst.h"
#include "scene.h"

typedef struct{
    WINDOW* container;
    WINDOW* data_display[DATA_NB];
    int initialized;
    sphere_t object;
}line_t;

#endif