# BreakwaterFFT
Distributed implementation of the FFT algorithm using MPI, designed for efficient data transformation in clustered systems. Named after the pier structures that break up waves.

## Usage

### Building
Depends on GCC >= 8 or Clang >= 6 as well as a version of MPI. Most development was done with MPICH but no implementation specific features were used. The makefile uses GCC by default, to use Clang add -cc=clang to CC and remove -fcx-limited-range from CFLAGS.

To build just run `make`, the executable will be named `breakwater` and placed in the root project folder.

### Running

To run use `mpirun [MPI Options] breakwater [Options] [File]`. 

The options for breakwater are:\
-h      Display help message and exit\
-l #    Set loglevel to #, between 0 (none) and 6 (all), default is 4\
-d      Ignore the first line or header of [FILE]\
-i      Calculate the inverse FFT\
-f      Calculate the forward FFT (default)

The file is expected to have one complex number on each line, with the real and imaginary parts separated by a comma (eg. "1.23,4.56") and in the first two columns respectively. The input will be padded with 0s to reach a power of two in size.

Results will be written to standard output in the same format. Logs are written to standard error.

# Description of Algorithms
## Preparatory Algorithms for the FFT

### Partition Algorithm
Let $n$ be a natural power of two representing the number of elements in our input set $x$.\
Let $m$ be a natural number representing the number of nodes in our distributed computing system, excluding the head node.\
Let $k = 2^{\lfloor log_{2}(m)\rfloor}$ which is the highest power of two less than or equal to $m$. \
Let $d = m - k$.\
Then, $n$ can be partitioned into $m$ subsets with $P$ of size $p$ and $Q$ of size $q$, where $p$, $q$ are natural powers of two or zero and $p > q$.\
If $k \geq n \div 2$ then $P = n \div 2$, $p = 2$, $Q = m - P$, and $q = 0$.\
Otherwise $P = k - d = m - Q$, $p = n \div k$, $Q = 2d$, and $q = p \div 2$.

### Communication Tree Algorithm

Let $R$ be the set of expected result sizes from all $m$ nodes, initialized to the sizes of the initial subset assigned to each node.\
Let $D$ be the set of destination nodes for each node and $D_m = 0$.\
Let $q$ be the size of the smallest subset greater than zero assigned to any node.\
1: For each element $r_a$ of value $q$ in $R$ find the next element in $R$, $r_b$, that also equals $q$ and set $r_b = r_b + r_a$, and $D_a = b$.\
2: Set $q = 2q$\
Repeat 1 & 2 until $q > R_m$

### Bit-Reversal Permutation Algorithm
Let $n$ be a natural power of two representing the number of elements in our input set $x$.\
Let $B(b)$ be the $log_{2}{n}$ bit-reversal (palindrome) of $b$.\
For each element $x_a$ in $x$ if $B(a) > a$ then swap $x_a$ and $x_{B(a)}$

The first and last element of $x$ can be skipped as they never need to be swapped.

## FFT Algorithm Implementation

### Fast Fourier Transform
Let $n$ be the size of our input set $x$, where $x$ is expected to be in bit-reversal permutation order.

For each power of two $m$ from 2 to $n$:
- For each subset of $x$, $y$, of size $m$:
  - For the first $m \over 2$ elements in $y$:
    - $y_a = y_a + e^{-ai\tau\over m} \cdot y_{a+{m \over 2}}$
    - $y_{a+{m \over 2}} = y_a - e^{-ai\tau\over m} \cdot y_{a+{m \over 2}}$

The only notable quality of how it is implemented in this program is that the butterfly operation, the innermost loop, is a standalone function that is called to consolidate result sets from multiple nodes.

### FFT Buffering Algorithm
Let $n$ be the size of our current result set $X$.\
Let $r$ be the expected size of our result set.\
While $n < r$: 
- Wait to receive a result set, add set to the end of a list $L$.
- While there is a set $S$ of size $n$ in list $L$:
    - Pre-append $S$ to $X$.
    - Set $n = 2n$.
    - Do a FFT butterfly operation on $X$ of size $n$.

Send the full result set $X$ to the destination node.

### Copyright Notice
Copyright 2023 Zachary Todd Edwards. MIT License