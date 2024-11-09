# Thread Synchronization

A C++ program that analyzes genomic files to calculate their CG content ratio using multi-threading synchronization mechanisms.

## Description

This program reads genomic files (.fna) from a specified directory and calculates the CG content ratio for each file. It implements two different synchronization approaches:
- Mutex with condition variables
- Counting semaphores

## Requirements

- C++20 compiler
- POSIX-compliant system (Linux/Unix)
- Make (optional)

## Installation

Clone the repository and compile using one of these methods:

Using make:
```sh
make
```
or manually:

```bash
g++ -std=c++20 -o proyecto.out proyecto.cpp
```

## Usage

Run the program with:

```bash
./proyecto.out [directory] [threshold]

./proyecto.out genomas/invertebrate 0.3
```

Parameters:

- directory: Path to directory containing genomic files (e.g., genomas/bacterias)
- threshold: Float value between 0 and 1 representing the CG content threshold