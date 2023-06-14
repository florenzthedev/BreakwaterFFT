# BreakwaterFFT
Distributed implementation of the FFT algorithm using MPI, designed for efficient data transformation in clustered systems.

## Usage

### Building

### Running

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

### Bit-Reversal Permutation Algorithm

### FFT Buffering Algorithm

## FFT Algorithm Implementation

### Copyright Notice
Copyright 2023 Zachary Todd Edwards. MIT License