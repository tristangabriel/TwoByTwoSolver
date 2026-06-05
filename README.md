# 2x2 Rubik's Cube Solver (Pocket Cube Engine)

A high-performance, portfolio-ready 2x2 Rubik's Cube solver written in modern C++. The project transitions a traditional procedural puzzle solver into a clean, object-oriented API that decouples puzzle state data from search algorithm logic. 

## Key Features & Architecture

* **Architecture:** Implements a `Cube` struct within a stateless `TwoByTwoSolver` class. `Cube` is implemented with a permutation array of cubies and its orientation. Order of cubie goes clockwise from top view and then from bottom view. Orientation 0 means White/Yellow color is facing Up/Down, 1 for the color clockwise to White/Yellow, 2 for the color counterclockwise.
* **Optimized Bidirectional BFS:** Uses a two-way graph search to drastically reduce the search space down to the puzzle's maximum diameter (11 moves), ensuring fast execution times.
* **Encapsulation & Compiling Efficiency:** Heavy character indexing maps and permutation sequences are isolated entirely within the `.cpp` source file's **anonymous namespace** to preserve a lightweight, pristine header interface and prevent dependency bloat.

---

## Project Structure

* `TwoByTwoSolver.h` - Public class definition, type specifications, and core solver API.
* `TwoByTwoSolver.cpp` - Bidirectional search loops, rotation logic, translation-unit local helpers, and lookup matrices.
* `main.cpp` - Application entry point demonstrating cube definition, custom scrambling, and solution tracing.

---

### Prerequisites
A modern C++ compiler supporting **C++17** or higher (such as `g++` or `clang++`).

### Compilation
To compile the solver with maximum optimizations (`-O3`), run the following command in your terminal:

```bash
g++ -O3 main.cpp TwoByTwoSolver.cpp -o solver
