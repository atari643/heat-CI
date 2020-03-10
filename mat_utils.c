/**
 * @file      mat_utils.c
 * @copyright Copyright (c) 2018, Inria
 * @author    Inria SED Bordeaux
 * @brief     A matrix utility library
 *
 * @details   This file provides several function to manipulate matrices.
 */
#include "mat_utils.h"

void
print_mat (int size_x, int size_y, const double *u)
{

  int i, j;
  printf ("heat: \n[");
  for (i = 0; i < size_x - 1; ++i)
    {
      for (j = 0; j < size_y - 1; ++j)
        {
          printf ("heat: %f,", u[i * size_y + j]);
        }
      printf ("heat: %f\n", u[i * size_y + size_y - 1]);
    }
  for (j = 0; j < size_y - 1; ++j)
    {
      printf ("heat: %f,", u[(size_x - 1) * size_y + j]);
    }
  printf ("heat: %f]\n", u[(size_x - 1) * size_y + size_y - 1]);
}

void
save_mat (const char *filename, int size_x, int size_y,
          const double *u)
{
  int i, j;
  FILE *fid;

  if (filename == NULL)  {
    fprintf(stderr, "Error: input argument <filename> NULL\n");
    return;
  }
  fid = fopen (filename, "w");
  if (fid == NULL) {
    fprintf(stderr, "Error: unable to open <%s>\n", filename);
    return;
  }
  for (i = 0; i < size_x; ++i)
    {
      for (j = 0; j < size_y; ++j)
        {
          fprintf (fid, "%.15e  ", u[i * size_y + j]);
        }
      fprintf (fid, "\n");
    }
  fclose (fid);
}
