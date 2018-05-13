#ifndef __CST_H__
#define __CST_H__ 

/* You can change these ones */

#define WIDTH 60
#define HEIGHT 60
#define LENGTH 60

#define MSG_KEY 5555
#define SHM_KEY 6666

#define MAX_OBJ 5
#define DEFAULT_SPEED 1000000

/* DO NOT CHANGE */

#define M_PI 3.14159265358979323846

#define MIN_WIDTH 92
#define DATA_NB 11

#define NO_BORDERS 0
#define WT_BORDERS 1 

#define CLIENT_NB 4

typedef struct{
    int width,height;
}dimensions_t;

#endif