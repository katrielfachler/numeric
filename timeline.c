#include "timeline.h"

#define ANALYTIC_CSV "../csv_files/analytic.csv"
#define EULER_CSV "../csv_files/euler.csv"
#define MIDPOINT_CSV "../csv_files/midpoint.csv"
#define RUNGE_KUTTA_CSV "../csv_files/runge_kutta.csv"
#define TIMELINE_HEADERS "iterations,time,r_y,r_z,v_y,v_z,a_y,a_z\n"
#define TIMELINE_ROW "%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n"
#define WRITE_MODE "w"

/******************************************/
/*        FUNCTIONS DECLARATIONS          */
/******************************************/

TimeState *get_starting_conditions ();
void init_time_line (Timeline *timeline, TimeState *starting_conditions);
bool run_alg (Timeline *timeline, NEXT_STEP_METHOD next_step_func,
              int dev_factor, double T);
char *get_timeline_path (Method method);
void print_timeline (Timeline *timeline, char *path);

/***********************************************/
/*        H FUNCTIONS IMPLEMENTATIONS          */
/***********************************************/
//
Timeline *
create_time_line (int dev_factor, double T, NEXT_STEP_METHOD next_step_method)
{
  TimeState *starting_conditions = get_starting_conditions ();
  Timeline *timeline = alloc_time_line ();
  if (!starting_conditions || !alloc_time_line ())
  { return NULL; }
  init_time_line (timeline, starting_conditions);

  if (!run_alg (timeline, next_step_method, dev_factor, T))
  { return NULL; }
  return timeline;
}

bool export_one_timeline (Method method, double T)
{
  Timeline *timeline = create_time_line (DIVISION_CONST, T, get_method (method));
  if (!timeline)
  {
    free_time_line (&timeline);
    return false;
  }
  char *path = get_timeline_path (method);
  print_timeline (timeline, path);
  free (path);
  free_time_line (&timeline);
  return true;
}

/***************************/
/*        HELPERS          */
/***************************/

TimeState *get_starting_conditions ()
{
  Vec *v_0 = alloc_vec (0, (3 * E) / B);
  if (!v_0)
  { return NULL; }
  Vec *r_0 = alloc_vec (0, 0);
  Vec *a_0 = alloc_vec ((q / m) * (E - B * v_0->_z), (q / m) * (B * v_0->_y));
  double t_0 = 0;
  TimeState *starting_conditions = alloc_time_state (t_0, r_0, v_0, a_0);
  if (!starting_conditions || !r_0 || !a_0)
  {
    free (r_0);
    free (v_0);
    free (a_0);
    free (starting_conditions);
    return NULL;
  }
  return starting_conditions;
}

void init_time_line (Timeline *timeline, TimeState *starting_conditions)
{
  timeline->first = starting_conditions;
  timeline->last = starting_conditions;
  timeline->size = 1;
}

bool run_alg (Timeline *timeline, NEXT_STEP_METHOD next_step_func,
              int dev_factor, double T)
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
  }
  return true;
}

char *get_timeline_path (Method method)
{
  char *path = "";
  switch (method)
  {
    case ANALYTIC:
      path = ANALYTIC_CSV;
      break;

    case EULER:
      path = EULER_CSV;
      break;

    case MIDPOINT:
      path = MIDPOINT_CSV;
      break;

    case RUNGE_KUTTA:
      path = RUNGE_KUTTA_CSV;
      break;
  }
  char *ret = malloc (strlen (path) + 1);
  strcpy (ret, path);
  return ret;
}

void print_timeline (Timeline *timeline, char *path)
{
  FILE *f = fopen (path, WRITE_MODE);
  TimeState *curr_time_state = timeline->first;
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