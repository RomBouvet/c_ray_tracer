#ifndef __CST_H__
#define __CST_H__ 

/* You can change these ones */

#define MSG_KEY 5555
#define SHM_KEY 6666

#define CLIENT_NB 4
#define MAX_OBJ 10
#define REFRESH_TIME 1000

/* DO NOT CHANGE */

#define M_PI 3.14159265358979323846

#define MIN_WIDTH 92
#define DATA_NB 11

#define NO_BORDERS 0
#define WT_BORDERS 1 

typedef struct{
    int width,height;
}dimensions_t;

#endif