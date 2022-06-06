#include "log_log_errors.h"
#include "wien_filter.h"

typedef enum Action
{
    FAILED,
    TIMELINE,
    ERRORS,
    WIEN_TIMELINE,
    WIEN_FILTER
}Action;

/**********************************/
/*        STRING DEFINES          */
/**********************************/

#define ALLOC_ERR "Error: failed to allocate memory."
#define ARGS_ERR "Usage: <timeline|errors|wien_timeline|wien_filter> "\
"<analytic|euler|midpoint|runge_kutta>.\n"
#define ANALYTIC_STR "analytic"
#define EULER_STR "euler"
#define MIDPOINT_STR "midpoint"
#define RUNGE_KUTTA_STR "runge_kutta"
#define TIMELINE_STR "timeline"
#define WIEN_TIMELINE_STR "wien_timeline"
#define WIEN_FILTER_STR "wien_filter"
#define ERRORS_STR "errors"

/******************************************/
/*        FUNCTIONS DECLARATIONS          */
/******************************************/

double get_T ();
int exit_err (char *msg);
Action process_args(int argc, char **argv, Method* method);
bool check_argc(int argc);
bool check_for_timeline (char **argv, Method *method);
bool check_for_errors (char **argv, Method *method);
bool check_for_wien_timeline (char **argv, Method *method);
bool check_for_wien_filter (char **argv, Method *method);
Method convert_str_method(char *str_method);

/************************/
/*        MAIN          */
/************************/

int main (int argc, char **argv)
{
  double T = get_T ();
  Method method = 0;
  Action action = process_args(argc, argv, &method);
  switch (action)
  {
    case FAILED:
      return exit_err (ARGS_ERR);
      break;
    case TIMELINE:
      if (!export_one_timeline (method, T))
      {
        return exit_err (ALLOC_ERR);
      }
      break;
    case ERRORS:
      if (!export_log_log_error (method, T, 1000, 100))
      {
        return exit_err (ALLOC_ERR);
      }
      break;
    case WIEN_TIMELINE:
    {
      Vec Dr = {0, 0};
      Vec Dv = {0, (3 * E) / B};
      if (!export_one_wien_timeline (method, &Dr, &Dv, T))
      {
        return exit_err (ALLOC_ERR);
      }
      break;
    }
      case WIEN_FILTER:
        if (!export_wien_filter (method, T))
        {
          return exit_err (ALLOC_ERR);
        }
      break;
  }
  return EXIT_SUCCESS;
}

/********************************/
/*        MAIN HELPERS          */
/********************************/

int exit_err (char *msg)
{
  fprintf (stderr, "%s", msg);
  return EXIT_FAILURE;
}

double get_T ()
{
  double w = (q * B) / m;
  return (2 * M_PI) / w;
}

Action process_args(int argc, char **argv, Method* method)
{

  if (!check_argc (argc))
  {
    return FAILED;
  }

  if (check_for_timeline (argv, method))
  {
    return TIMELINE;
  }

  if (check_for_errors (argv, method))
  {
    return ERRORS;
  }

  if (check_for_wien_timeline (argv, method))
  {
    return WIEN_TIMELINE;
  }

  if (check_for_wien_filter (argv, method))
  {
      return WIEN_FILTER;
  }

  return FAILED;
}

/****************************************/
/*        PROCESS ARGS HELPERS          */
/****************************************/

bool check_argc(int argc)
{
  return argc == 3 || argc == 5;
}

bool check_for_timeline (char **argv, Method *method)
{
  if (strcmp (argv[1], TIMELINE_STR) != 0)
  {
    return false;
  }

  *method = convert_str_method(argv[2]);
  if (*method == NON_METHOD)
  {
    return false;
  }

  return true;
}

bool check_for_errors (char **argv, Method *method)
{
  if (strcmp (argv[1], ERRORS_STR) != 0)
  {
    return false;
  }

  *method = convert_str_method (argv[2]);
  if (*method == NON_METHOD)
  {
    return false;
  }
  return true;
}

bool check_for_wien_timeline (char **argv, Method *method)
{
  if (strcmp (argv[1], WIEN_TIMELINE_STR) != 0)
  {
    return false;
  }

  *method = convert_str_method (argv[2]);
  if (*method == NON_METHOD)
  {
    return false;
  }
  return true;
}

bool check_for_wien_filter (char **argv, Method *method)
{
  if (strcmp (argv[1], WIEN_FILTER_STR) != 0)
  {
    return false;
  }

  *method = convert_str_method (argv[2]);
  if (*method == NON_METHOD)
  {
    return false;
  }
  return true;
}

Method convert_str_method(char *str_method)
{
  if (!strcmp (str_method, ANALYTIC_STR))
  {
    return ANALYTIC;
  }

  if (!strcmp (str_method, EULER_STR))
  {
    return EULER;
  }

  if (!strcmp (str_method, MIDPOINT_STR))
  {
    return MIDPOINT;
  }

  if (!strcmp (str_method, RUNGE_KUTTA_STR))
  {
    return RUNGE_KUTTA;
  }
  return NON_METHOD;
}
