//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#ifndef FFT_H_INCLUDED
#define FFT_H_INCLUDED
#include <complex.h>

/**
 * @brief Prints out an array of complex numbers. Numbers are printed in a way
 * that csv2complex will accept.
 *
 * @param x Array of complex numbers.
 * @param N Size of array.
 */
void print_complex(double complex *x, int N);

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
 * @brief Calculates fair power of two partitioning for N values across nodes
 * nodes.
 *
 * @param N Total number of values to be operated on, must be a power of two.
 * @param parts Preallocated array of ints that the resulting partition will be
 * stored in.
 * @param nodes Number of nodes.
 */
void partition_pow2(int N, int parts[], int nodes);

/**
 * @brief From an array of partition sizes this calculates how large the result
 * of each node should be and where it should be sent. Results are passed up to
 * the next node that handles that result size, until the last node is reached
 * where the final result is sent to node 0. Any node with a partition size of 0
 * has their result size set to 0 and no destination set, these nodes are
 * expected to quit once they receive a result size of 0.
 *
 * @param result_size Preallocated array of ints to store result sizes in.
 * @param result_dest Preallocated array of ints to store result destinations
 * in.
 * @param parts Array of partition sizes, in order from smallest to largest.
 * @param nodes The number of nodes, excluding the head node, in the current
 * system.
 */
void result_targets(int result_size[], int result_dest[], int parts[],
                    int nodes);

/**
 * @brief Performs a bit reversal permutation on the given array of complex
 * numbers to prepare them for the FFT. Will use compiler intrinsics if
 * available.
 *
 * @param x The array of complex numbers the FFT will be performed on.
 * @param N The size of the array, must be a power of two.
 */
void bit_reversal_permutation(double complex *x, int N);

/**
 * @brief A single FFT butterfly operation, used internally by fft_pass(). It is
 * unlikely that this will need to be called externally on its own.
 *
 * @param X Dataset to perform butterfly operation on, will be overwritten and
 * must be a power of two in size.
 * @param n
 * @param omega
 */
void fft_butterfly(double complex X[], int n, double complex omega);

/**
 * @brief A single pass of the Fast-Fourier Transform, used internally by fft()
 * and on its own for consolidating two neighboring identically sized chunks of
 * data that have already had the FFT performed on them. All parameters must be
 * a power of two.
 *
 * @param X The input dataset of contiguous already-FFT'd data, will be
 * overwritten and must be a power of two in size.
 * @param N The total size of the input dataset.
 * @param n The size of the butterfly operation to perform on the dataset. If
 * this is being used two consolidate two already-FFT'd chunks of data this will
 * probably be equal to N.
 */
void fft_pass(double complex X[], int N, int n);

/**
 * @brief The Fast-Fourier Transform algorithm, computes the Fourier transform
 * on a set of complex numbers. The input array must be a power of two in size,
 * already be in bit reversal permutation order, and will be overwritten.
 *
 * @param X The input set of complex numbers, must be a power of two in size,
 * and will be overwritten by the results.
 * @param n The size of the input set.
 */
void fft(double complex X[], int n);

#endif  // FFT_H_INCLUDED
