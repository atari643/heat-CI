/**
 * @file      heat.c
 * @copyright Copyright (c) 2018, Inria
 * @author    Inria SED Bordeaux
 * @brief     Heat computation iteration code
 *
 * @details   This file defines the heat() function.
 */

#include "heat.h"

double
heat (double hx, double hy, double dt,
      int size_x, int size_y,
      const double *u_in, double *u_out)
{

    int i, j;
    double w_x, w_y, err, d;

    w_x = dt / (hx * hx);
    w_y = dt / (hy * hy);
    d = 1. - 2. * w_x - 2. * w_y;

    err = 0.;
    for (j = 1; j < size_y - 1; ++j)
        {
            for (i = 1; i < size_x - 1; ++i)
                {
                    u_out[i * size_y + j] = d * u_in[ i * size_y + j]  + w_x * (u_in[(i - 1) * size_y + j] + u_in[(i + 1) * size_y + j])
                        + w_y * (u_in[i * size_y + j - 1]   + u_in[i * size_y + j + 1]);
                    err += SQR (u_out[i * size_y + j] - u_in[i * size_y + j]);
                }
        }
    return err;
}
