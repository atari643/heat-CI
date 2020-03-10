/**
 * @file      mat_utils.h
 * @copyright Copyright (c) 2018, Inria
 * @author    Inria SED Bordeaux
 * @brief     A matrix utility library
 *
 * @details   This file provides several function to manipulate matrices.
 */
#ifndef __MAT_UTILS_H
#define __MAT_UTILS_H

#include <stdio.h>

/**
 * \defgroup GR_mat_utils Matrix utility
 * @{
 */

/**
 * @brief A matrix printing function
 *
 * @details It prints on the standard output the matrix given in the parameter
 * u. For instance if u = [[1., 2.], [3., 4.]], then print_mat(2,2,u)
 * output:
 * @code
 *   [1.,2.
 *    3.,4.]
 * @endcode
 *
 * @param size_x the size in X of the matrix u
 * @param size_y the size in Y of the matrix u
 * @param u the matrix to print
 */
void
print_mat (int size_x, int size_y, const double *u);

/**
 * @brief A matrix saving function
 *
 * @details It saves in a file the matrix given in the parameter @e u.
 * For instance if <code>u = [[1., 2.], [3., 4.]]</code>, then
 * save_mat("toto",2,2,u) will output in the file toto:
 * @code
 * 1.000000000000000e+00  2.000000000000000e+00
 * 3.000000000000000e+00  4.000000000000000e+00
 * @endcode
 *
 * @param filename the file to output the result
 * @param size_x the size in X of the matrix u
 * @param size_y the size in Y of the matrix u
 * @param u the matrix to print
 */
void
save_mat (const char *filename, int size_x, int size_y,
          const double *u);

/**@}*/

#endif
