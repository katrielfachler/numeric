#include "wien_filter.h"

#define MAX_DIVISION 1000

/******************************************/
/*        FUNCTIONS DECLARATIONS          */
/******************************************/

int get_rand (int max);
double get_rand_double (double max_val);

/***********************************************/
/*        H FUNCTIONS IMPLEMENTATIONS          */
/***********************************************/

bool export_wien_filter (Method method, double T)
{
  srand (time (NULL));
  int particles = pow (10, 2);

  for (int i = 0; i < particles; ++i)
  {
    Vec Dr = {get_rand_double (R), 0};
    Vec Dv = {get_rand_double (V), 0};
    bool did_exit;
    Timeline *wien_timeline = create_wien_time_line(DIVISION_CONST, T,
                                                    get_method(method), &Dr,
                                                    &Dv, &did_exit);

  }


  return true;
}

/***************************/
/*        HELPERS          */
/***************************/

int get_rand (int max)
{
  return rand() % max;
}

double get_rand_double (double max_val)
{
  int rand_int = get_rand (MAX_DIVISION);
  if (!rand_int)
  {
    return 0.f;
  }
  return max_val/rand_int;
}