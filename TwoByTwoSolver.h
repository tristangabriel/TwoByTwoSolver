#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <utility>
#include <optional>

class TwoByTwoSolver {
    public:
        struct Cube {
            std::array<uint8_t, 8> perm = {0, 1, 2, 3, 4, 5, 6, 7};
            std::array<uint8_t, 8> orient = {0, 0, 0, 0, 0, 0, 0, 0};
        };
        static constexpr int MAX_STATES = 3674160;
        static const std::string move_ind[];

        static std::map<std::string, int> ind_move;

        TwoByTwoSolver();
        std::vector<std::string> convert_to_sticker(const Cube &c);
        void print_cube(const Cube &c);
        inline int cube_hash(const Cube &c);
        Cube convert_to_cubie(const std::vector<std::string>& stickers);
        void rotate(Cube &c, std::string move_name);
        void fix_rotation(const Cube &sc, Cube &solved);
        std::optional< std::vector< std::string> > bi_bfs(Cube sc);
        std::optional< std::vector< std::string> > bi_bfs(Cube sc, Cube dest);
        void scrambling(Cube& sc, const std::vector<std::string>& move_list);
    
    private:
        void generate_index_moves();
        void tracking_sol(std::array<std::pair<int, uint8_t>, MAX_STATES>& parent, std::vector<std::string>& v, int cur_state, int final_state);
        std::optional< std::vector< std::string> > soln_tracker(std::array<std::pair<int, uint8_t>, MAX_STATES>& parent, int solved_id, int sc_id, int sc_mid, int solved_mid, int mid_move, Cube scrambled);
};