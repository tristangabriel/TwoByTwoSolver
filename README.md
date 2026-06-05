# 2x2 Rubik's Cube Solver (Pocket Cube Engine)

A high-performance, portfolio-ready 2x2 Rubik's Cube solver written in modern C++. The project transitions a traditional procedural puzzle solver into a clean, object-oriented API that decouples puzzle state data from search algorithm logic. 

## Key Features & Architecture

* **Decoupled Stateless Architecture:** Implements a distinct `Cube` data struct nested within a stateless `TwoByTwoSolver` computing engine. This allows multiple cube states to be instantiated, tracked, and solved concurrently.
* **Optimized Bidirectional BFS:** Uses a two-way graph search to drastically reduce the search space down to the puzzle's maximum diameter (11 moves), ensuring fast execution times.
* **Memory-Conscious Allocation:** High-capacity data tables ($3.67\text{ MB}$ tracking distances and $29.39\text{ MB}$ tracking parents) are explicitly allocated dynamically via `std::unique_ptr` to ensure zero stack-overflow overhead.
* **Encapsulation & Compiling Efficiency:** Heavy character indexing maps and permutation sequences are isolated entirely within the `.cpp` source file's **anonymous namespace** to preserve a lightweight, pristine header interface and prevent dependency bloat.
* **Compile-Time Lookups:** Uses `constexpr std::string_view` arrays to ensure fixed layout parameters are evaluated at compile time, reducing runtime heap memory overhead to zero.

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
