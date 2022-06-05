#ifndef TIMELINE_H
#define TIMELINE_H

# include "methods.h"

Timeline *
create_time_line (int dev_factor, double T, NEXT_STEP_METHOD next_step_method);
bool export_one_timeline (Method method, double T);

#endif