#include "wien_timeline.h"

/******************************************/
/*        FUNCTIONS DECLARATIONS          */
/******************************************/

TimeState *get_wien_starting_conditions (Vec *Dr, Vec *Dv);
void init_wien_time_line (Timeline *timeline, TimeState *starting_conditions);
bool run_wien_alg (Timeline *timeline, NEXT_STEP_METHOD next_step_func,
              int dev_factor, double T, bool *did_exit);
bool check_for_exit(Vec *r, bool *did_exit);

/***********************************************/
/*        H FUNCTIONS IMPLEMENTATIONS          */
/***********************************************/

Timeline *
create_wien_time_line (int dev_factor, double T, NEXT_STEP_METHOD
next_step_method, Vec *Dr, Vec *Dv, bool *did_exit)
{
  TimeState *starting_conditions = get_wien_starting_conditions (Dr, Dv);
  Timeline *timeline = alloc_time_line ();
  if (!starting_conditions || !alloc_time_line ())
  { return NULL; }
  init_wien_time_line (timeline, starting_conditions);

  if (!run_wien_alg (timeline, next_step_method, dev_factor, T, did_exit))
  { return NULL; }
  return timeline;
}

/***************************/
/*        HELPERS          */
/***************************/

TimeState *get_wien_starting_conditions (Vec *Dr, Vec *Dv)
{
  Vec *v_0 = alloc_vec (0, E / B);
  Vec *r_0 = alloc_vec (0, 0);
  if (!v_0 || !r_0)
  {
    free (v_0);
    free (r_0);
    return NULL;
  }
  add_to_vec_vec (v_0, Dv);
  add_to_vec_vec (r_0, Dr);
  Vec *a_0 = alloc_vec ((q / m) * (E - B * v_0->_z), (q / m) * (B * v_0->_y));
  double t_0 = 0;
  TimeState *starting_conditions = alloc_time_state (t_0, r_0, v_0, a_0);
  if (!starting_conditions || !a_0)
  {
    free (r_0);
    free (v_0);
    free (a_0);
    free (starting_conditions);
    return NULL;
  }
  return starting_conditions;
}

void init_wien_time_line (Timeline *timeline, TimeState *starting_conditions)
{
  timeline->first = starting_conditions;
  timeline->last = starting_conditions;
  timeline->size = 1;
}

bool run_wien_alg (Timeline *timeline, NEXT_STEP_METHOD next_step_func,
                   int dev_factor, double T, bool *did_exit)
{
  double Dt = T/dev_factor;
  for (int i = 0; i < dev_factor; i++)
  {
    TimeState *next_time_state = next_step_func (timeline->last, Dt);
    if (!next_time_state)
    { return false; }
    timeline->last->next = next_time_state;
    timeline->last = next_time_state;
    timeline->size++;
    if (check_for_exit(timeline->last->r, did_exit))
    {
      break;
    }
  }
  return true;
}

bool check_for_exit(Vec *r, bool *did_exit)
{
  if (r->_z > LENGTH)
  {
    *did_exit = true;
    return true;
  }
  if (r->_y > R)
  {
    *did_exit = false;
    return true;
  }
  return false;
}

