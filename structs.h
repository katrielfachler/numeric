#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/**************************/
/*        CONSTS          */
/**************************/

#define E 1.f
#define B 0.5f
#define q 3.f
#define m 20.f
#define LENGTH 5.f
#define R 0.03f
#define DIVISION_CONST 500
#define V 15.f

/***************************/
/*        STRUCTS          */
/***************************/

typedef struct Vec
{
    double _y, _z;
}Vec;

typedef struct TimeState
{
    double time;
    Vec *r, *v, *a;
    struct TimeState *next;
}TimeState;

typedef struct Timeline
{
    TimeState *first;
    TimeState *last;
    unsigned int size;
}Timeline;

typedef TimeState * (NEXT_STEP_METHOD)(TimeState *, double Dt);

/*****************************/
/*        FUNCTIONS          */
/*****************************/

Timeline *alloc_time_line();
TimeState *alloc_time_state(double t,Vec *r, Vec *v, Vec *a);
Vec *alloc_vec(double y, double z);
void free_time_line(Timeline **p_timeline);
void free_time_state(TimeState **p_time_state);
void mult_vec_scalar(Vec *vec, double n);
void add_to_vec_scalar(Vec *vec, double n);
void mult_vec_vec(Vec *first, Vec *second);
void add_to_vec_vec(Vec *first, Vec *second);
TimeState *clone_time_state(TimeState *time_state);

#endif