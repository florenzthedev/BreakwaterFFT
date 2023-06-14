//  Copyright (c) 2023 Zachary Todd Edwards
//  MIT License

#define INIT_BLOCK_SIZE 4
#define MAX_LINE_SIZE 1024
#define M_TAU 6.28318530717958647692

#include "fft.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitmanip.h"

void print_complex(double complex *x, int N) {
  for (int i = 0; i < N; i++) printf("%f,%f\n", creal(x[i]), cimag(x[i]));
}

void fft_butterfly(double complex X[], int n, double complex omega) {
  for (int j = 0; j < n / 2; j++) {
    double complex product = cpow(omega, j) * X[j + n / 2];
    X[j + n / 2] = X[j] - product;
    X[j] = X[j] + product;
  }
}

void fft_pass(double complex X[], int N, int n) {
  double complex omega = cexp(-((I * M_TAU) / n));
  for (int j = 0; j < N; j += n) fft_butterfly(&X[j], n, omega);
}

void fft(double complex X[], int N) {
  for (int j = 2; j <= N; j *= 2) fft_pass(X, N, j);
}

double complex *csv2cmplx(const char *filename, bool header, int *N) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    return NULL;
  }
  if (header) fscanf(fp, "%*[^\n]\n"); //Ignore first line
  double complex *x = malloc(sizeof(double complex) * INIT_BLOCK_SIZE);
  assert(x != NULL);
  double temp_real, temp_imag;
  int allocated = INIT_BLOCK_SIZE;
  (*N) = 0;

  // using fgets means we get the whole line, this is useful if excel wants to
  // add extra commas to our file
  char line[MAX_LINE_SIZE];
  while (fgets(line, MAX_LINE_SIZE, fp)) {
    sscanf(line, "%lf,%lf", &temp_real, &temp_imag);
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
// Given that most of the values will be the same it should not ever take the
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

void partition_pow2(int N, int parts[], int nodes) {
  assert((N & (N - 1)) == 0);                  // Must be a power of two
  memset(parts, 0, nodes * sizeof(int));       // zero out array
  int balance = 1 << (bit_length(nodes) - 1);  // pow(2,floor(log2(nodes)))
  if (balance >= (N / 2)) {                    // Everyone gets 2 or 0
    for (int i = 0; i < N / 2; i++) parts[nodes - 1 - i] = 2;
    return;
  }
  int portion_a = N / balance;       // Only ever need two values
  int portion_b = portion_a / 2;     // a & b
  int difference = nodes - balance;  // num above a power of two
  // int count_a = balance - difference;  // num of nodes with higher value
  int count_b = 2 * difference;  // num of nodes with lower value
  for (int i = 0; i < count_b; i++) parts[i] = portion_b;
  for (int i = count_b; i < nodes; i++) parts[i] = portion_a;
}

void result_targets(int result_size[], int result_dest[], int parts[],
                    int nodes) {
  int start = 0;
  while (parts[start] == 0) start++;  // not enough work to go around

  memcpy(result_size, parts, sizeof(int) * nodes);

  for (int i = result_size[start]; i <= result_size[nodes - 1]; i *= 2) {
    for (int j = start; j < nodes - 1; j++) {
      for (int k = j + 1; k < nodes; k++) {
        if (result_size[j] == i && result_size[k] == i) {
          result_size[k] += result_size[j];
          result_dest[j] = k + 1;  // zero to one indexed
          break;
        }
      }
    }
  }
  result_dest[nodes - 1] = 0;  // return final response to head-node
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

struct fft_buffer_s {
  double complex *x;
  int n;
  struct fft_buffer_s *next;
};

fft_buffer fft_buffer_init() {
  fft_buffer buf = malloc(sizeof(struct fft_buffer_s));
  buf->next = NULL;
  buf->x = NULL;
  buf->n = 0;
  return buf;
}

void fft_buffer_add(fft_buffer buf, double complex *x, int n) {
  while (buf->next != NULL) buf = buf->next;  // locate last link
  buf->next = malloc(sizeof(struct fft_buffer_s));
  buf = buf->next;  // step into new link
  buf->next = NULL;
  buf->n = n;
  buf->x = malloc(sizeof(double complex) * n);
  memcpy(buf->x, x, sizeof(double complex) * n);
}

double complex *fft_buffer_search(fft_buffer buf, int n) {
  while (buf->next != NULL) {
    buf = buf->next;
    if (buf->n == n) return buf->x;
  }
  return NULL;
}

void fft_buffer_free(fft_buffer *root) {
  fft_buffer buf = (*root)->next;
  while (buf != NULL) {
    fft_buffer temp = buf->next;
    free(buf->x);
    free(buf);
    buf = temp;
  }
  free((*root));
  root = NULL;
}
