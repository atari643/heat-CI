/**
 * @file      heat_par.c
 * @copyright Copyright (c) 2018, Inria
 * @author    Inria SED Bordeaux
 * @brief     Heat computation main procedure code for parallele computation
 *
 * @details   This file declares the entry point (main() procedure) of the program.
 *
 */
#include "heat.h"
#include "mat_utils.h"
#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Procedure which put ones on the boundaries of the global solution.
 *
 * @details If the current process touchs the boundary of the 2D cartesian
 * topology we have to puts ones of the corresponding column or row.
 *
 * @param coo integer pair containing the coordinates in 2D cartesian topology
 * @param nc_x number of processes in the x-dimension
 * @param nc_y number of processes in the y-dimension
 * @param size_x rows number of the local part of the solution
 * @param size_y columns number of the local part of the solution
 * @param u local part of the solution
 */
static void
set_bounds (const int *coo, int nc_x, int nc_y, int size_x, int size_y,
            double *u)
{

  int i, j;
  for (i = 0; i < size_x; ++i)
    {
      if (coo[1] == 0)
        u[i * size_y + 0] = 1.;
      if (coo[1] == (nc_y - 1))
        u[i * size_y + size_y - 1] = 1.;
    }

  for (j = 0; j < size_y; ++j)
    {
      if (coo[0] == 0)
        u[0 * size_y + j] = 1.;
      if (coo[0] == (nc_x - 1))
        u[(size_x - 1) * size_y + j] = 1.;
    }

}

/**
 * @brief Procedure for swapping the boundaries (2 columns and 2 rows)
 * with the neighbour processes
 *
 * @param comm the 2D communicator
 * @param col the column type
 * @param neighbours the set of the local neighbours processes
 * @param size_x rows number of the local part of the solution
 * @param size_y columns number of the local part of the solution
 * @param u local part of the solution
 */
static void
ghosts_swap (MPI_Comm comm, MPI_Datatype col,
                  const int *neighbours, int size_x,
                  int size_y, double *u)
{

  int N = 0, S = 1, E = 2, W = 3;
  int s_tag, r_tag;
  MPI_Status status;

  /* N --> S
    N block last significant row goes to S block first ghost row */
  s_tag = 0;
  r_tag = 0;
  MPI_Sendrecv (&u[(size_x - 2) * size_y + 0], size_y, MPI_DOUBLE, neighbours[S],
                s_tag, &u[+0 * size_y + 0], size_y, MPI_DOUBLE, neighbours[N],
                r_tag, comm, &status);
  /*  S --> N
   S block first significant row  goes to N block last ghost row */
  s_tag = 1;
  r_tag = 1;
  MPI_Sendrecv (&u[1 * size_y + 0], size_y, MPI_DOUBLE, neighbours[N], s_tag,
                &u[(size_x - 1) * size_y + 0], size_y, MPI_DOUBLE, neighbours[S],
                r_tag, comm, &status);

  /* W --> E
   W block last significant column goes to E block first ghost column */
  s_tag = 2;
  r_tag = 2;
  MPI_Sendrecv (&u[1 * size_y + size_y - 2], 1, col, neighbours[E], s_tag,
                &u[1 * size_y + 0], 1, col, neighbours[W], r_tag, comm, &status);

  /*  E --> W
    E block first significant column goes to W block last ghost column */
  s_tag = 3;
  r_tag = 3;
  MPI_Sendrecv (&u[1 * size_y + 1], 1, col, neighbours[W], s_tag,
                &u[1 * size_y + size_y - 1], 1, col, neighbours[E], r_tag, comm,
                &status);

}


/**
 * @brief A usage function
 *
 * @details This function prints out the normal usage of the program and exit
 * the program with failure.
 *
 * @param argv the array of arguments passed to the main procedure
 */
static void
usage(char *argv[])
{
  fprintf(stderr, "Usage: mpirun -np (px*py) %s nx ny iter_max px py save\n", argv[0]);
  fprintf(stderr, "\tnx       number of discretisation points in X\n");
  fprintf(stderr, "\tny       number of discretisation points in Y\n");
  fprintf(stderr, "\titer_max maximal number of iterations in temporal loop\n");
  fprintf(stderr, "\tpx       X process number\n");
  fprintf(stderr, "\tpy       Y process number\n");
  fprintf(stderr, "\tsave     boolean flag (1 or 0) for recording states\n");
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

  int nx, ny, i, j, r, size_x, size_y, cell_x, cell_y, nc_x, nc_y;

  double hx, hy, dt, err_loc, err, iter_max, prec;

  double *u_in, *u_out, *vec_temp, *solution;

  int rank_w, size_w;

  int rank_2D;
  MPI_Comm comm2D;
  MPI_Datatype type_col;
  int ndims = 2;
  int save;
  int dims[2], periods[2], coords[2], reorder;

  int N = 0, S = 1, E = 2, W = 3;
  int neighbours[4];


  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank_w);
  MPI_Comm_size (MPI_COMM_WORLD, &size_w);


  if (argc < 7)
    {
      usage(argv);
    }


  nx = atoi (argv[1]);
  ny = atoi (argv[2]);
  iter_max = atoi (argv[3]);
  nc_x = atoi (argv[4]);
  nc_y = atoi (argv[5]);
  save = atoi (argv[6]);

  if (nc_x * nc_y != size_w)
    {
      printf
        (" the total number of processus differs from the product px * py :  %d x %d != %d \n",
         nc_x, nc_y, size_w);
      exit (-1);
    }
  dims[0] = nc_x;
  dims[1] = nc_y;
  periods[0] = 0;
  periods[1] = 0;
  reorder = 1;

  // creation of a 2D cartesian topology (nc_x x nc_y processus)
  MPI_Cart_create (MPI_COMM_WORLD, ndims, dims, periods, reorder, &comm2D);
  // fetch the rank of the topology
  MPI_Comm_rank (comm2D, &rank_2D);
  // fetch the coords of the current process
  MPI_Cart_coords (comm2D, rank_2D, ndims, coords);

  // we identify neighbors in the cartesian topology
  // neighbours[point] contains the rank in the 2-D communicator
  // of the cell corresponding to point
  MPI_Cart_shift (comm2D, 0, 1, &neighbours[N], &neighbours[S]);
  MPI_Cart_shift (comm2D, 1, 1, &neighbours[W], &neighbours[E]);


  cell_x = nx / nc_x;
  cell_y = ny / nc_y;

  size_x = cell_x + 2;
  size_y = cell_y + 2;


  // creation of a MPI column type to transfer a column (non contiguous in C)
  // from one cell to another
  MPI_Type_vector (cell_x, 1, size_y, MPI_DOUBLE, &type_col);
  MPI_Type_commit (&type_col);

  u_in = (double *) calloc (size_x * size_y, sizeof (double));
  if (u_in == NULL)
     {
      printf("not enough memory!\n");
      exit(-1);
     }

  u_out = (double *) calloc (size_x * size_y, sizeof (double));
  if (u_out == NULL)
     {
      printf("not enough memory!\n");
      exit(-1);
     }

  set_bounds (coords, nc_x, nc_y, size_x, size_y, u_in);
  set_bounds (coords, nc_x, nc_y, size_x, size_y, u_out);


  hx = 1. / nx;
  hy = 1. / ny;
  dt = MIN (SQR (hx) / 4., SQR (hy) / 4.);
  prec = 1e-4;
  err = 1e10;
  // temporal loop
  for (i = 0; i < iter_max; ++i)
    {

      err_loc = heat (hx, hy, dt, size_x, size_y, u_in, u_out);

      // retrieve local error to compute the global error
      MPI_Allreduce (&err_loc, &err, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
      err = sqrt (err);
      if (rank_w == 0 && (i % 10 == 0))
        printf ("heat: it = %d, t = %.3e, err = %.3e\n", i, i * dt, err);
      memcpy (u_in, u_out, sizeof (double) * size_x * size_y);

      ghosts_swap (comm2D, type_col, neighbours, size_x, size_y, u_in);

      if (err <= prec)
        break;
    }

  vec_temp = (double *) calloc (nc_x * nc_y * size_x * size_y, sizeof (double));
  // gather the big solution on the process of rank 0
  MPI_Gather (u_in, size_x * size_y, MPI_DOUBLE, vec_temp, size_x * size_y,
              MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (rank_w == 0)
    {
      solution = (double *) calloc (nx * ny, sizeof (double));
      // loop over coordinates to re-organize the big flat vector
      for (r = 0; r < size_w; ++r)
        {
          int coo2[2];
          MPI_Cart_coords (comm2D, r, ndims, coo2);
          //  we copy the flattened submatrix with coords (i,j)
          // in the global solution matrix
          for (i = 1; i < size_x - 1; ++i)
            {
              for (j = 1; j < size_y - 1; ++j)
                {
                  solution[(coo2[0] * (size_x - 2) + (i - 1)) * ny +
                           coo2[1] * (size_y - 2) + j - 1] =
                    vec_temp[(coo2[0] * nc_y + coo2[1]) * size_x * size_y +
                             i * size_y + j];
                }
            }
          if (save)
            save_mat ("sol_para.txt", nx, ny, solution);
        }
      free (solution);
    }

  free (vec_temp);
  MPI_Type_free (&type_col);
  free (u_out);
  free (u_in);
  MPI_Finalize ();
  return 0;
}
