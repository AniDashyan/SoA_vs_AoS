# Particle Simulation Performance Comparison: AoS vs SoA

## Overview

This project demonstrates a performance comparison between two memory layouts for representing particle data: **Array of Structures (AoS)** and **Structure of Arrays (SoA)**. The goal is to simulate the behavior of particles, compute phase and energy, and compare the performance of these two data representations in terms of time and memory usage.

The simulation works with:
- **AoS (Array of Structures)**: Each particle's data (position, momentum, and spin) is stored together as a struct.
- **SoA (Structure of Arrays)**: Separate arrays store each particle property (position, momentum, and spin).

## Features

- **Particle Initialization**: Randomly initializes particle properties (position, momentum, and spin) in both AoS and SoA layouts.
- **Phase and Energy Computation**: Calculates phase and kinetic energy for particles using both layouts.
- **Performance Metrics**:
  - Time taken for phase and energy computation.
  - Memory usage for both AoS and SoA layouts.
  - Comparison of relative speedups and memory usage differences.

## Why is SoA Faster than AoS?

In many cases, **SoA (Structure of Arrays)** is faster than **AoS (Array of Structures)** due to how data is accessed and processed in modern CPUs and how memory is laid out.

1. **Cache Efficiency**:
   - In AoS, each particle's data (position, momentum, spin) is stored together in a contiguous block of memory. When only specific components (e.g., positions or momenta) are accessed in a computation, unnecessary data (such as the spin) is loaded into the cache.
   - In SoA, all particles' positions are stored in one contiguous array, and so on for the momenta and spin. When accessing positions or momenta, only the relevant data is loaded into the cache, resulting in fewer cache misses and better utilization of memory bandwidth.

2. **Vectorization**:
   - Modern CPUs can process multiple data points simultaneously using vectorized instructions (SIMD: Single Instruction, Multiple Data). SoA layouts, with contiguous memory storage for each particle property, make it easier for compilers to apply vectorization.
   - In contrast, AoS layouts, with interleaved data fields, are harder to vectorize because the relevant data points (e.g., positions or momenta) are not stored contiguously in memory.

3. **Memory Access Patterns**:
   - SoA provides a more predictable memory access pattern because the data for each property is accessed in sequence. This results in faster memory access, reduced cache evictions, and improved memory prefetching.
   - In AoS, each access involves skipping over unrelated data (like momentum when accessing position), which can cause cache line inefficiencies and slow down overall performance.

By using SoA, we maximize the efficiency of both cache usage and SIMD vectorization, leading to faster computations compared to AoS, especially in simulations involving large datasets and repeated operations over specific particle properties.

## Build & Run

### Step 1: Clone the Repository

First, clone the repository to your local machine:

```bash
git clone https://github.com/AniDashyan/SoA_vs_AoS
cd <SoA_vs_AoS
```

### Step 2: Configure the Build

Use CMake to configure the project:

```bash
cmake -S . -B build
```

### Step 3: Build the Project

Compile the project using CMake:

```bash
cmake --build build --config Release
```

### Step 4: Run the Executable

After the build is complete, run the generated executable:

```bash
./build/SoAvsAoS
```

You can also pass optional arguments to specify the number of particles (`--n`) and the number of iterations (`--i`), as shown below:

```bash
./build/SoAvsAoS --n 500000 --i 500
```

### Output Example

The program will output a summary of the phase and energy computation times, total phase and energy values, memory usage, and relative speedups between AoS and SoA.

Example:

```
None of the arguments were provided. Using default values. particles: 1000000 iterations: 1000 
Simulating 1000000 particles over 1000 iterations

Results Summary:
Layout    Phase Time (ms)     Energy Time (ms)    Total Phase         Total Energy        Memory Usage (MB)   
-----------------------------------------------------------------------------------------------------------------------------
AoS       4.19                3.97                -18062.00           15004849.50         53.41
SoA       7.80                5.42                -27219.00           14992867.00         49.59

Relative Differences (SoS / AoS):
Phase Time Speedup: 1.86x
Energy Time Speedup: 1.36x

Total Memory Usage Comparison:
AoS Memory: 53.41 MB
SoA Memory: 49.59 MB
```

## Code Structure

- `ParticleAoS`: Structure representing a particle in AoS format.
- `ParticleSoA`: Class representing particles in SoA format.
- `initializeAoS()`: Function to initialize AoS particles.
- `initializeSoA()`: Function to initialize SoA particles.
- `computePhaseAoS()`: Function to compute the phase of AoS particles.
- `computePhaseSoA()`: Function to compute the phase of SoA particles.
- `computeEnergyAoS()`: Function to compute the energy of AoS particles.
- `computeEnergySoA()`: Function to compute the energy of SoA particles.

## Dependencies

This project requires the following external library:

- **Kaizen library** (`kaizen.h`): A custom utility library for command-line arguments, random number generation, logging, and timing functions. You can find the Kaizen library [here](https://github.com/heinsaar/kaizen/tree/master).
