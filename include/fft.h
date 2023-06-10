//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#ifndef FFT_H_INCLUDED
#define FFT_H_INCLUDED
#include <complex.h>

/**
 * @brief The Fast-Fourier Transform algorithm, computes the Fourier transform
 * on a set of complex numbers. This is a recursive implementation and
 * overwrites the array it is passed. The input set must be a power of two in
 * size.
 *
 * @param X The input set of complex numbers, must be a power of two in size,
 * and will be overwritten by the results.
 * @param n The size of the input set.
 */
void fft(double complex X[], int n);

/**
 * @brief Reads in a csv file into an array of double complex. File is expected
 * to have one complex number on each line, with the real and imaginary parts
 * separated by a comma, eg. "1.23,4.56" This function will pad the resulting
 * array with zeros so that its size is a power of two. The minimum size is
 * controlled by a constant INIT_BLOCK_SIZE.
 *
 * @param filename The name of the file to attempt to open.
 * @param N The integer to store the size of the complex number array.
 *
 * @return A pointer to a dynamically allocated array of complex numbers.
 */
double complex *csv2cmplx(const char *filename, int *N);

/**
 * @brief Calculates an even partitioning for N values across nodes nodes.
 *
 * @param N Total number of values to be operated on, must be a power of two.
 * @param parts Preallocated array of ints that the resulting partition will be
 * stored in.
 * @param nodes Number of nodes.
 */
void partition_fft(int N, int parts[], int nodes);

/**
 * @brief Performs a bit reversal permutation on the given array of complex
 * numbers to prepare them for the FFT. Will use compiler builtins if available.
 *
 * @param x The array of complex numbers the FFT will be performed on.
 * @param N The size of the array, must be a power of two.
 */
void bit_reversal_permutation(double complex *x, int N);

#endif  // FFT_H_INCLUDED
