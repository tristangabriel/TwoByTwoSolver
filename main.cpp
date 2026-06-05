#include "TwoByTwoSolver.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    TwoByTwoSolver solver;

    // By face, going by top, left, front, right, back, bottom (net order)
    // Stickers are dictated clockwise, similar to 2BLD counting
    // GWBO = GW
    //        BO
    TwoByTwoSolver::Cube c = solver.convert_to_cubie({"ORRY", "WRBB", "GWBO", "GBOW", "WGRG", "YOYY"});

    solver.print_cube(c);
    auto solute = solver.bi_bfs(c);
    if (solute) {
        for (auto &u : *solute) {
            cout << u << ' ';
        }
    }
    else {
        cout << "Invalid Somehow";
    }


    return 0;
}