/**
 * @file      heat.h
 * @copyright Copyright (c) 2018, Inria
 * @author    Inria SED Bordeaux
 * @brief     Heat computation iteration code
 *
 * @details   This file declares the heat() function.
 */
#ifndef __HEAT_H
#define __HEAT_H

/**
 * @brief Computes the square of @e X, e.g., 4 for @e X = 2, 9 for @e X = 3,
 * etc.
 *
 * @param X the value to square
 * @return the square of @e X, that is @e X * @e X
 */
#define SQR(X)    ((X) * (X))

/**
 * @brief Computes the minimum between @e X and @e Y. E.g.,
 * <code>MIN(1,2) = MIN(2,1) = 1</code>
 *
 * @param X the first value to test
 * @param Y the second value to test
 * @return the minimum value between @e X and @e Y
 */
#define MIN(X,Y)  (((X) <= (Y)) ? (X) : (Y))

/**
 * @brief Do a single iteration of the heat equation iterative computation on
 * a 2-D cartesian map.
 *
 * @details For each cell in @e u_in that is in
 * <code>[1..size_x]x[1..size_y]</code>, it put in @e u_out the result of one
 * iteration computation using a cross-stencil.
 * The complete equation can be found in the @e README file of the project.
 *
 * @param hx precision of the derivation over x
 * @param hy precision of the derivation over y
 * @param dt precision of the derivation over time
 * @param size_x the size of the cartesian map in x
 * @param size_y the size of the cartesion map in y
 * @param u_in the input map
 * @param u_out the output map
 * @return the square of the quadratic differences between @e u_in and @e u_out after
 *         the execution of the heat function
 */
double
heat (double hx, double hy, double dt,
         int size_x, int size_y,
         const double *u_in, double *u_out);


#endif
