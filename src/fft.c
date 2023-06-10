//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#define INIT_BLOCK_SIZE 8
#define M_TAU 6.28318530717958647692

#include "fft.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitmanip.h"

void fft(double complex X[], int n) {
  if (n == 1) return;

  double complex Ye[n / 2];
  for (int j = 0; j < n; j += 2) Ye[j / 2] = X[j];
  fft(Ye, n / 2);

  double complex Yo[n / 2];
  for (int j = 1; j < n; j += 2) Yo[j / 2] = X[j];
  fft(Yo, n / 2);

  double complex omega = cexp(-((I * M_TAU) / n));
  for (int j = 0; j < n / 2; j++) {
    X[j] = Ye[j] + cpow(omega, j) * Yo[j];
    X[j + n / 2] = Ye[j] - cpow(omega, j) * Yo[j];
  }
}

double complex *csv2cmplx(const char *filename, int *N) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    return NULL;
  }

  double complex *x = malloc(sizeof(double complex) * INIT_BLOCK_SIZE);
  assert(x != NULL);
  double temp_real, temp_imag;
  int allocated = INIT_BLOCK_SIZE;
  (*N) = 0;
  while (fscanf(fp, "%lf,%lf", &temp_real, &temp_imag) > 0) {
    if ((*N) == allocated) {
      x = realloc(x, (allocated *= 2) * sizeof(double complex));
      assert(x != NULL);
    }
    x[(*N)++] = CMPLX(temp_real, temp_imag);
  }
  fclose(fp);

  // pad with zeros to next nearest power of two
  while ((*N) < allocated) x[(*N)++] = 0;

  return x;
}

// Basic implementation of heapsort.
// Given that most of the values should be the same it should not ever take the
// full time, but I was unable to prove the allocation algorithm would never
// produce more than X number of unique values so wanted to avoid counting sort.

// Heap calculations for 0-indexed heap
#define parent(x) ((x - 1) / 2)
#define lchild(x) (2 * x + 1)
#define rchild(x) (2 * x + 2)

int temp;
#define swap_i(x, y) \
  temp = x;          \
  x = y;             \
  y = temp;

void bubble_down(int list[], int index, int N) {
  int hi = index;
  for (;;) {
    if (lchild(index) < N && list[lchild(index)] > list[hi]) hi = lchild(index);
    if (rchild(index) < N && list[rchild(index)] > list[hi]) hi = rchild(index);
    if (hi != index) {
      swap_i(list[hi], list[index]);
      index = hi;
    } else
      break;
  }
}

void heapsort(int list[], int N) {
  for (int i = N / 2; i >= 0; i--) bubble_down(list, i, N);
  while (N > 1) {
    N--;
    swap_i(list[0], list[N]);
    bubble_down(list, 0, N);
  }
}

void partition_fft(int N, int parts[], int nodes) {
  assert((N & (N - 1)) == 0);  // Must be a power of two
  parts[0] = N;

  // treat the allocations like a heap
  // Pretty sure this is an O(nlogn) algorithm
  for (int i = 1; i < nodes; i++) {
    int hi = parent(i);

    // while the parent of our highest known parent is greater
    while (parts[hi] <= parts[parent(hi)] && hi != 0) hi = parent(hi);

    // two is the smallest segment we can work with
    if (parts[hi] < 4) {
      parts[i] = 0;
      continue;
    }

    // Half taken from the highest parent for the current node
    parts[hi] = parts[i] = parts[hi] / 2;
  }

  // Get these block sizes in order!
  heapsort(parts, nodes);
}

void bit_reversal_permutation(double complex *x, int N) {
  assert((N & (N - 1)) == 0);  // Must be a power of two

  // Don't forget bit_length is one indexed!
  int bl = bit_length(N) - 1;

  // We can skip the first and last index, they never need to be moved
  for (int i = 1; i < N - 1; i++) {
    int ri = bit_reverse(i, bl);
    if (i < ri) {
      double complex temp = x[i];
      x[i] = x[ri];
      x[ri] = temp;
    }
  }
}