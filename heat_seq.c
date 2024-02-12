/**
 * @file      heat_seq.c
 * @copyright Copyright (c) 2018, Inria
 * @author    Inria SED Bordeaux
 * @brief     Heat computation main procedure code
 *
 * @details   This file declares the entry point (main() procedure) of the program.
 *
 */
#include "heat.h"
#include "mat_utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @brief Allocates code with error handling
 *
 * @details Allocates a pointer @e var to @e s object of type @e type. This
 * macros exit with a proper message on failure of the allocation.
 * For example:
 * @code
 *   int *buf;
 *   xalloc(buf, 10, int);
 * @endcode
 * allocates an array of 10 integer in buf.
 *
 * @param var the pointer to allocate
 * @param s the number of elements to allocate
 * @param type the type of elements to allocate
 */
#define xalloc(var, s, type) do {			\
    var = (type *) calloc(s, sizeof(type)); \
    if(var == NULL) {                                       \
      perror("calloc");                                     \
      exit(EXIT_FAILURE);                                   \
    }                                                               \
  } while(0)


/**
 * @brief Set the boundaries of a 2-D map to 1.0
 *
 * @details This function will set to 1.0 all the cells on the boundaries of a
 * 2-D double maps @e u of size @e size_x in X and @e size_y in Y. @e u should
 * be an array of size (@e size_x + 2) * (@e size_y + 2).
 *
 * For example, if @e u = [0, 0, 0, 0, 0, 0, 0, 0, 0] then after a call to
 * set_bounds(1, 1, u), @e u will contains [1, 1, 1, 1, 0, 1, 1, 1, 1]. In
 * the same way, if @e u = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
 * then after a call to set_bounds(2, 2, u),  @e u will contains
 * [1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1]. That is in 2-D:
 * @code
 *   1 1 1 1
 *   1 0 0 1
 *   1 0 0 1
 *   1 1 1 1
 * @endcode
 *
 * @param size_x The size in X of the @e u map
 * @param size_y The size in Y of the @e u map
 * @param u The map to set boundaries to 1.0
 */
void
set_bounds(int size_x, int size_y, double *u)
{
  int i, j;

  for (i = 0; i < size_x; ++i)
    {
      u[i * size_y + 0] = 1.;
      u[i * size_y + size_y -1] = 1.;
    }

  for (j = 0; j < size_y; ++j)
    {
      u[0 * size_y + j] = 1.;
      u[(size_x - 1) * size_y + j] = 1.;
    }
}

/**
 * @brief Compute the heat propagation equation using iterative approach
 * until convergence.
 *
 * @details This function will compute the heat propagation equation on the map
 * @e u_in using the iterative approach outputing the result in @e u_out.
 * @e dt, @e hx and @e hy give the derivation approximation steps over time,
 * X and Y. @e iter_max limits the maximum number of iteration if the computation
 * does not converge.
 *
 * @param hx the derivation approximation step in X
 * @param hy the derivation approximation step in Y
 * @param dt the derivation approximation step in time
 * @param iter_max the maximum number of iteration to perform.
 * @param save a boolean indicating if the results is to be saved in a file after
 *        each step
 * @param size_x The size in X of the @e u_in and @e u_out maps
 * @param size_y The size in Y of the @e u_in and @e u_out maps
 * @param u_in the input map, it will be modified and invalidated by the
 *        call of this function
 * @param u_out the output map, it will contains the result of the computation
 */
static void
compute_heat_propagation(double hx, double hy, double dt,
                         int iter_max, int save, int print,
                         int size_x, int size_y,
                         double *u_in, double *u_out)
{
  int i;
  char name_fic[120];
  double err, prec;

  prec = 1e-7;
  err = 1e10;

  for (i = 0; i < iter_max; ++i)
    {
      err = heat (hx, hy, dt, size_x, size_y, u_in, u_out);
      err = sqrt (err);
      if (i % 10 == 0)
        {
         printf ("heat: it = %d, t = %.3e, err = %.3e\n", i, i * dt, err);
        }
      if (save)
        {
         sprintf (name_fic,"sol_%05d", i);
         save_mat (name_fic, size_x, size_y, u_in);
        }
      if (print)
        {
         print_mat (size_x, size_y, u_in);
        }
      memcpy (u_in, u_out, sizeof (double) * size_x * size_y);
      if (err <= prec)
        break;
    }
}

/**
 * @brief A usage function
 *
 * @details This function prints out the normal usage of the program and exit
 * the program with failure.
 *
 * @param argv the array of arguments passed to the main procedure
 *
 */
static void
usage(char *argv[])
{
  fprintf(stderr, "Usage: %s nx ny iter_max save\n", argv[0]);
  fprintf(stderr, "\tnx       number of discretisation points in X\n");
  fprintf(stderr, "\tny       number of discretisation points in Y\n");
  fprintf(stderr, "\titer_max maximal number of iterations in temporal loop\n");
  fprintf(stderr, "\tsave     boolean flag (1 or 0) for recording states\n");
  fprintf(stderr, "\tprint    boolean flag (1 or 0) for printing the states matrix to the standard output\n");
  exit(EXIT_FAILURE);
}

/**
 * @brief Main procedure
 *
 * @details Of course, this is the entry point :P
 *
 * @param argc the number of program arguments
 * @param argv the list of program arguments
 * @return the error code of the program
 */
int
main (int argc, char *argv[])
{
  int nx=0, ny=0, size_x, size_y, save=0, print=0, iter_max=0;
  double hx, hy, dt, utmp;
  double *u_in, *u_out, *u_tmp;
  clock_t start, end;
  double cpu_time_used;

  if( argc < 6 ) {
      usage(argv);
  } else{
    nx = atoi (argv[1]);
    ny = atoi (argv[2]);
    iter_max = atoi (argv[3]);
    save = atoi(argv[4]);
    print = atoi(argv[5]);
  }

  hx = 1. / nx;
  hy = 1. / ny;
  dt = MIN (SQR (hx) / 4., SQR (hy) / 4.);

  size_x = nx + 2;
  size_y = ny + 2;

  xalloc (u_in, size_x * size_y, double);
  xalloc (u_out, size_x * size_y, double);
  xalloc (u_tmp, size_x * size_y, double);
  u_tmp[0] = utmp;

  set_bounds (size_x, size_y, u_in);
  set_bounds (size_x, size_y, u_out);

  start = clock();
  compute_heat_propagation (hx, hy, dt, iter_max, save, print, size_x, size_y,
                            u_in, u_out);
  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

  free (u_out);
  free (u_in);

  return EXIT_SUCCESS;
}
