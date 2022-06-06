#ifndef METHODS_H
#define METHODS_H

#include "structs.h"
#include <math.h>

typedef enum Method
{
    NON_METHOD,
    ANALYTIC,
    EULER,
    MIDPOINT,
    RUNGE_KUTTA
}Method;

TimeState *analytic_method(TimeState *curr_time_state, double Dt);
TimeState *euler_method (TimeState *curr_time_state, double Dt);
TimeState *midpoint_method (TimeState *curr_time_state, double Dt);
TimeState *runge_kutta_method (TimeState *curr_time_state, double Dt);
NEXT_STEP_METHOD *get_method (Method method);

#endif