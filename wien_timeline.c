#include "wien_timeline.h"

#define WIEN_ANALYTIC_CSV "../csv_files/wien_analytic.csv"
#define WIEN_EULER_CSV "../csv_files/wien_euler.csv"
#define WIEN_MIDPOINT_CSV "../csv_files/wien_midpoint.csv"
#define WIEN_RUNGE_KUTTA_CSV "../csv_files/wien_runge_kutta.csv"
#define TIMELINE_HEADERS "iterations,time,r_y,r_z,v_y,v_z,a_y,a_z\n"
#define TIMELINE_ROW "%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n"
#define DID_EXIT "did exit,yes\n"
#define DIDNT_EXIT "did exit,no\n"
#define WRITE_MODE "w"

/******************************************/
/*        FUNCTIONS DECLARATIONS          */
/******************************************/

TimeState *get_wien_starting_conditions (Vec *Dr, Vec *Dv);
void init_wien_time_line (Timeline *timeline, TimeState *starting_conditions);
bool run_wien_alg (Timeline *timeline, NEXT_STEP_METHOD next_step_func,
              int dev_factor, double T, bool *did_exit);
bool check_for_exit(Vec *r, bool *did_exit);
char *get_wien_timeline_path (Method method);
void print_wien_timeline (Timeline *timeline, char *path, bool did_exit);

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

bool export_one_wien_timeline (Method method, Vec *Dr, Vec *Dv, double T)
{
  bool did_exit;
  Timeline *timeline = create_wien_time_line (DIVISION_CONST, T, get_method
  (method), Dr, Dv, &did_exit);
  if (!timeline)
  {
    free_time_line (&timeline);
    return false;
  }
  char *path = get_wien_timeline_path (method);
  print_wien_timeline (timeline, path, did_exit);
  free (path);
  free_time_line (&timeline);
  return true;
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
  bool stop_condition = true;
  while (stop_condition)
  {
    TimeState *next_time_state = next_step_func (timeline->last, Dt);
    if (!next_time_state)
    { return false; }
    timeline->last->next = next_time_state;
    timeline->last = next_time_state;
    timeline->size++;
    stop_condition = check_for_exit(timeline->last->r, did_exit);
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

char *get_wien_timeline_path (Method method)
{
  char *path = "";
  switch (method)
  {
    case ANALYTIC:
      path = WIEN_ANALYTIC_CSV;
      break;

    case EULER:
      path = WIEN_EULER_CSV;
      break;

    case MIDPOINT:
      path = WIEN_MIDPOINT_CSV;
      break;

    case RUNGE_KUTTA:
      path = WIEN_RUNGE_KUTTA_CSV;
      break;
  }
  char *ret = malloc (strlen (path) + 1);
  strcpy (ret, path);
  return ret;
}

void print_wien_timeline (Timeline *timeline, char *path, bool did_exit)
{
  FILE *f = fopen (path, WRITE_MODE);
  TimeState *curr_time_state = timeline->first;

  if (did_exit)
  {
    fprintf (f, DID_EXIT);
    fprintf (stdout, "yes\n");
  }
  else
  {
    fprintf (f, DIDNT_EXIT);
    fprintf (stdout, "no\n");
  }

  fprintf (f, TIMELINE_HEADERS);
  for (int i = 0; i < timeline->size; ++i)
  {
    fprintf (f, TIMELINE_ROW,
             i,
             curr_time_state->time,
             curr_time_state->r->_y,
             curr_time_state->r->_z,
             curr_time_state->v->_y,
             curr_time_state->v->_z,
             curr_time_state->a->_y,
             curr_time_state->a->_z);
    curr_time_state = curr_time_state->next;
  }
  fclose (f);
}

