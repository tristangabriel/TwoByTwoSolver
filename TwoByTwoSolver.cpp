#include "TwoByTwoSolver.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <algorithm>
#include <queue>
#include <utility>
#include <optional>

namespace {

    constexpr int8_t UNVISITED = 0x7F;

    const std::string coloring[8] = {"WOB", "WBR", "WRG", "WGO", "YOG", "YGR", "YRB", "YBO"};

    const uint8_t cubie_order[6][4] = {{0, 1, 2, 3}, {0, 3, 4, 7}, {3, 2, 5, 4}, {2, 1, 6, 5}, {1, 0, 7, 6}, {4, 5, 6, 7}};
    const uint8_t cubie_side[6][4]  = {{0, 0, 0, 0}, {1, 2, 1, 2}, {1, 2, 1, 2}, {1, 2, 1, 2}, {1, 2, 1, 2}, {0, 0, 0, 0}};
    std::vector<std::vector<std::vector<std::string> > > greedy = {
        {{"z2", "y'"}, {"x"}, {"z'"}},
        {{"z2"}, {"z", "y'"}, {"x", "y'"}},
        {{"z2", "y"}, {"x'", "y2"}, {"z", "y2"}},
        {{"x2"}, {"z'", "y"}, {"x'", "y"}},
        {{"y"}, {"x'"}, {"z'", "y2"}},
        {{"y2"}, {"z", "y"}, {"x'", "y'"}},
        {{"y'"}, {"x", "y2"}, {"z"}},
        {{}, {"z'", "y'"}, {"x", "y"}}
    };

    std::map<std::string, std::pair<uint8_t, uint8_t> > cubie_indexing;
    int perm_lookup[1<<21];

    bool is_initialized=0;
    static std::array<int8_t, TwoByTwoSolver::MAX_STATES> dist_table;
    static std::array<std::pair<int, uint8_t>, TwoByTwoSolver::MAX_STATES> parent;
}

// Global Functions
void init_lookup_table() {
    static const int FACTORIAL[] = {1, 1, 2, 6, 24, 120, 720};
    int p[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    do {
        int hash=0;
        for (int i=0;i<6;i++) {
            int cnt=0;
            for (int j = i+1; j<7 ;j++) {
                if (p[j]<p[i]) cnt++;
            }
            hash += cnt * FACTORIAL[6-i];
        }  
        perm_lookup[p[0] | p[1] << 3 | p[2] << 6 | p[3] << 9 | p[4] << 12 | p[5] << 15 | p[6] << 18] = hash;
    } while (std::next_permutation(p, p+8));
}

void generate_cubies() {  // used for cubie indexing (needed for input)
    for (int i=0;i<8;i++) {
        std::string s = coloring[i]+coloring[i];
        for(int permute=0; permute < 3; permute++){
            cubie_indexing[s.substr(permute, 3)] = {i, permute};
        }
    }
}

const std::string TwoByTwoSolver::move_ind[] = {"U", "U2", "U'", "L", "L2", "L'", "F", "F2", "F'", "R", "R2", "R'", "B", "B2", "B'", "D", "D2", "D'", 
                                    "x", "x2", "x'", "y", "y2", "y'", "z", "z2", "z'"};
std::map<std::string, int> TwoByTwoSolver::ind_move;

//TwoByTwoSolver Methods

void TwoByTwoSolver::generate_index_moves() {
    for (int i=0; i<27;i++) {
        ind_move[move_ind[i]] = i;
    }
}


TwoByTwoSolver::TwoByTwoSolver() {
    if (!is_initialized) {
        generate_cubies();
        generate_index_moves();
        init_lookup_table();
        is_initialized = true;
    }
}


std::vector<std::string> TwoByTwoSolver::convert_to_sticker(const TwoByTwoSolver::Cube &c) {
    std::vector<std::string> stickers(6);
    for (int f=0;f<6;f++) {
        auto ordering = cubie_order[f];  // what cubies are needed for face f
        auto side = cubie_side[f];  // select the right side of the cubie to get the color from

        stickers[f]="";
        for (int i=0;i<4;i++) {
            stickers[f] += coloring[ c.perm[ordering[i]] ][(side[i] + c.orient[ordering[i]]) % 3];  // get the color from coloring
        }
    }
    return stickers;
}

void TwoByTwoSolver::print_cube(const TwoByTwoSolver::Cube &c) {
    auto stickers = convert_to_sticker(c);
    std::cout << "   " << stickers[0][0] << stickers[0][1] << "\n";
    std::cout << "   " << stickers[0][3] << stickers[0][2] << "\n\n";
    for (int side=1;side<5;side++) {
        std::cout << stickers[side][0] << stickers[side][1] << ' ';
    }
    std::cout << "\n";
    for (int side=1;side<5;side++) {
        std::cout << stickers[side][3] << stickers[side][2] << ' ';
    }
    std::cout << "\n\n";
    std::cout << "   " << stickers[5][0] << stickers[5][1] << "\n";
    std::cout << "   " << stickers[5][3] << stickers[5][2] << "\n\n";
}

inline int TwoByTwoSolver::cube_hash(const TwoByTwoSolver::Cube &c) {
    int orient_hash = c.orient[0]
                    + c.orient[1] * 3
                    + c.orient[2] * 9
                    + c.orient[3] * 27
                    + c.orient[4] * 81
                    + c.orient[5] * 243;
    int perm_hash = c.perm[0] | c.perm[1] << 3 | c.perm[2] << 6 | c.perm[3] << 9 
                    | c.perm[4] << 12 | c.perm[5] << 15 | c.perm[6] << 18;
    return (perm_lookup[perm_hash] * 729 + orient_hash);
}

TwoByTwoSolver::Cube TwoByTwoSolver::convert_to_cubie(const std::vector<std::string>& stickers) {
    char form_cubie[8][3];
    TwoByTwoSolver::Cube state;
    for (int side=0;side<6;side++) {
        for (int sticker=0;sticker<4;sticker++) {
            form_cubie[cubie_order[side][sticker]][cubie_side[side][sticker]] = stickers[side][sticker];
        }
    }

    for (int i=0;i<8;i++) {
        std::string s(form_cubie[i], 3);
        std::tie(state.perm[i], state.orient[i]) = cubie_indexing[s];
    }
    return state;
}

void TwoByTwoSolver::rotate(TwoByTwoSolver::Cube &c, const std::string move_name) {
    int move_num = ind_move[move_name];
    int face = move_num/3;
    for (int i=0; i<(move_num%3+1); i++) {
        if (face >= 6) {  //if its a rotational move
            if (face==6) {
                rotate(c, "R");
                rotate(c, "L'");
            }
            else if (face==7) {
                rotate(c, "U");
                rotate(c, "D'");
            }
            else if (face==8) {
                rotate(c, "F");
                rotate(c, "B'");
            }
            continue;
        }

        uint8_t temp_p = c.perm[cubie_order[face][3]], temp_o = c.orient[cubie_order[face][3]];
        for (int j=3;j>0;j--) {
            c.perm[cubie_order[face][j]] = c.perm[cubie_order[face][j-1]];
            c.orient[cubie_order[face][j]] = c.orient[cubie_order[face][j-1]];
        }
        c.perm[cubie_order[face][0]] = temp_p;
        c.orient[cubie_order[face][0]] = temp_o;
        if (face!=0 && face != 5) {  // move is neither Up nor Down
            for (int j=0;j<4;j++) {
                c.orient[cubie_order[face][j]] = (c.orient[cubie_order[face][j]] + 1 + j%2)%3;
            }
        }
    }
}

void TwoByTwoSolver::scrambling(Cube& sc, const std::vector<std::string>& move_list) {
    for (const std::string& move_name : move_list) {
        rotate(sc, move_name);
    }
}

void TwoByTwoSolver::fix_rotation(const TwoByTwoSolver::Cube &sc, TwoByTwoSolver::Cube &solved) {
    int dest_cubie = 8;
    for (int j=0;j<8;j++) {
        if (solved.perm[j]==sc.perm[7]) {
            dest_cubie = j;
            break;
        }
    }

    for (const std::string& u : greedy[dest_cubie][(3-sc.orient[7]) % 3]) {
        rotate(solved, u);
    }
}

void TwoByTwoSolver::tracking_sol(std::array<std::pair<int, uint8_t>, TwoByTwoSolver::MAX_STATES>& parent, std::vector<std::string>& v, int cur_state, int final_state) {
    while (cur_state!=final_state) {
        v.push_back(move_ind[parent[cur_state].second]);
        cur_state = parent[cur_state].first;
    }
}

std::optional< std::vector< std::string> > TwoByTwoSolver::soln_tracker(std::array<std::pair<int, uint8_t>, TwoByTwoSolver::MAX_STATES>& parent, 
                int solved_id, int sc_id, int sc_mid, int solved_mid, int mid_move, TwoByTwoSolver::Cube scrambled) {

    std::vector<std::string> soln;
    tracking_sol(parent, soln, sc_mid, sc_id);
    reverse(soln.begin(), soln.end());
    soln.push_back(move_ind[mid_move]);

    tracking_sol(parent, soln, solved_mid, solved_id);
    scrambling(scrambled, soln);
    if (cube_hash(scrambled)!=solved_id) {
        return std::nullopt;
    }
    return soln;
}

std::optional< std::vector< std::string> > TwoByTwoSolver::bi_bfs(TwoByTwoSolver::Cube sc) {
    return bi_bfs(sc, Cube());
}

std::optional< std::vector< std::string> > TwoByTwoSolver::bi_bfs(TwoByTwoSolver::Cube sc, TwoByTwoSolver::Cube dest) {  // Uses bidirectional BFS
    dist_table.fill(UNVISITED);
    parent.fill({0, 0});
    fix_rotation(sc, dest);


    int right_id = cube_hash(dest);
    int left_id = cube_hash(sc);
    dist_table[right_id] = 0, dist_table[left_id] = -1;
    parent[right_id]={-1, 100}; parent[left_id]={-1, 100};

    std::queue<Cube> q({sc, dest});
    
    int mid_state=-1;
    while (!q.empty() && mid_state==-1) {
        const Cube cur = q.front(); q.pop();

        int ind = cube_hash(cur);
        bool sign = dist_table[ind]>=0;
        for (uint8_t side : {9, 0, 6}) {
            if (side <= parent[ind].second && parent[ind].second < side+3) {  // if same move side
                continue;
            }
            Cube next_state = cur;  // the same state but moved
            for (int add=0;add<3;add++) {
                rotate(next_state, move_ind[side]);  //moved

                int next_state_index = cube_hash(next_state);
                if (dist_table[next_state_index]==UNVISITED) {  // if unvisited
                    q.push(next_state);
                    dist_table[next_state_index] = dist_table[ind] + ( (sign) ? 1 : -1) ;
                    parent[next_state_index] = {ind, side + ((sign) ? (2 - add) : (add)) };
                }
                else if ((dist_table[next_state_index]>=0) && !sign) {  // match from scramble side
                    return soln_tracker(parent, right_id, left_id, ind, next_state_index, side+add, sc);
                }
                else if ((dist_table[next_state_index]<0) && sign) {  // match from solved side
                    return soln_tracker(parent, right_id, left_id, next_state_index, ind, side+2-add, sc);
                }
            }           
        }
    }
    return std::nullopt;
}


