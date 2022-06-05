#include "methods.h"

Timeline *
create_wien_time_line (int dev_factor, double T, NEXT_STEP_METHOD
next_step_method, Vec *Dy, Vec *Dv, bool *did_exit);
bool export_one_wien_timeline (Method method, double T);