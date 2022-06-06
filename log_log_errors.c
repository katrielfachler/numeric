#include "log_log_errors.h"
#include <math.h>

#define ERRORS_HEADERS "Dt,err_r,err_v\n"
#define EULER_ERRORS_CSV "../csv_files/euler_errors.csv"
#define MIDPOINT_ERRORS_CSV "../csv_files/midpoint_errors.csv"
#define RUNGE_KUTTA_ERRORS_CSV "../csv_files/runge_kutta_errors.csv"
#define WRITE_MODE "w"
#define ERROR_CELL "%lf,"
#define NEW_LINE "\n"

/******************************************/
/*        FUNCTIONS DECLARATIONS          */
/******************************************/

TimeState *get_analytic_T (double T);
bool get_err (TimeState *analytic_T, int dev_factor, Method method, double
T, double *err_r, double *err_v);
void print_err_arr (double *err_arr, char *path, int num_of_errors);
TimeState *get_numeric_T (int dev_factor, double T, Method method);
double get_dist (Vec *first, Vec *sec);
char *get_error_path (Method method);

/***********************************************/
/*        H FUNCTIONS IMPLEMENTATIONS          */
/***********************************************/

bool export_log_log_error (Method method, double T, int max_dev_factor, int
num_of_errors)
{
  double err_arr[num_of_errors * 3];
  TimeState *analytic_T = get_analytic_T (T);
  int dev_factor = max_dev_factor / num_of_errors;
  for (int i = dev_factor; i < max_dev_factor; i += dev_factor)
  {
    double err_r = 0;
    double err_v = 0;
    if (!get_err (analytic_T, i, method, T, &err_r, &err_v))
    {
      free_time_state (&analytic_T);
      return false;
    }
    int j = 3 * (i / dev_factor - 1);
    err_arr[j] = log (T / i);
    err_arr[j + 1] = log (err_r);
    err_arr[j + 2] = log (err_v);
  }
  char *path = get_error_path (method);
  print_err_arr (err_arr, path, num_of_errors);
  free (path);
  free_time_state (&analytic_T);
  return true;
}

/***************************/
/*        HELPERS          */
/***************************/


TimeState *get_analytic_T (double T)
{
  TimeState time_state = {0, NULL, NULL, NULL, NULL};
  TimeState *ret = analytic_method (&time_state, T);
  if (!ret)
  { return NULL; }
  return ret;
}

bool get_err (TimeState *analytic_T, int dev_factor, Method method, double
T, double *err_r, double *err_v)
{
  TimeState *numeric_T = get_numeric_T (dev_factor, T, method);
  if (!numeric_T)
  { return false; }
  *err_r = get_dist (analytic_T->r, numeric_T->r);
  *err_v = get_dist (analytic_T->v, numeric_T->v);
  free_time_state (&numeric_T);
  return true;
}

TimeState *get_numeric_T (int dev_factor, double T, Method method)
{
  Timeline *timeline = create_time_line (dev_factor, T, get_method (method));
  if (!timeline)
  { return NULL; }
  TimeState *numeric_T = clone_time_state (timeline->last);
//  printf ("Numeric, Dt = %lf:\ntime: %lf\nr= %lf, %lf\nv = %lf, %lf\n\n",
//          T/dev_factor,
//          numeric_T->time,
//          numeric_T->r->_y,
//          numeric_T->r->_z,
//          numeric_T->v->_y,
//          numeric_T->r->_z);
  free_time_line (&timeline);
  return numeric_T;
}

double get_dist (Vec *first, Vec *sec)
{
  return sqrt (pow (first->_y - sec->_y, 2) + pow (first->_z - sec->_z, 2));
}

char *get_error_path (Method method)
{
  char *path = "";
  switch (method)
  {
    case EULER:
      path = EULER_ERRORS_CSV;
      break;

    case MIDPOINT:
      path = MIDPOINT_ERRORS_CSV;
      break;

    case RUNGE_KUTTA:
      path = RUNGE_KUTTA_ERRORS_CSV;
      break;
  }
  char *ret = malloc (strlen (path) + 1);
  strcpy (ret, path);
  return ret;
}

void print_err_arr (double *err_arr, char *path, int num_of_errors)
{
  FILE *f = fopen (path, WRITE_MODE);
  fprintf (f, ERRORS_HEADERS);
  for (int i = 0; i < num_of_errors * 3; ++i)
  {
    fprintf (f, ERROR_CELL, err_arr[i]);
    if ((i + 1) % 3 == 0)
    {
      fprintf (f, NEW_LINE);
    }
  }
  fclose (f);
}