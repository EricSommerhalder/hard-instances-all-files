#include <iostream>
#include <iterator>
#include <cstring>
#include "PermutationPDB.h"
#include "LexPermutationPDB.h"
#include "Timer.h"
#include "MNPuzzle.h"
#include "TemplateAStar.h"
using namespace std;
char prefix[1024] = "";
/**
 * Returns a Korf instance
 * @param which : int between 0 and 99 to choose which Korf instance to get
 * @return : The Korf instance as a MNPuzzleState<4,4>
 */
MNPuzzleState<4, 4> GetSTPInstance(int which);


/**
 * Returns the neighbours of a game state.
 * @param state: the game state
 * @return: Array of all neighbours
 */
MNPuzzleState<4,4>* getNeighbours(MNPuzzleState<4,4> state);

/**
 * Helper function which prints a game state to the command line.
 * @param state: the game state
 */
void printGameState(MNPuzzleState<4,4> state);

/**
 * Helper function that finds the neighbour of a game state, where the empty tile is moved to the left.
 * @param state: the game state
 * @return: the left neighbour
 */
MNPuzzleState<4,4> getLeftNeighbour(MNPuzzleState<4,4> state);

/**
 * Helper function that finds the neighbour of a game state, where the empty tile is moved to the right.
 * @param state: the game state
 * @return: the right neighbour
 */
MNPuzzleState<4,4> getRightNeighbour(MNPuzzleState<4,4> state);

/**
 * Helper function that finds the neighbour of a game state, where the empty tile is moved up.
 * @param state: the game state
 * @return: the upper neighbour
 */
MNPuzzleState<4,4> getUpNeighbour(MNPuzzleState<4,4> state);

/**
 * Helper function that finds the neighbour of a game state, where the empty tile is moved down.
 * @param state: the game state
 * @return: the lower neighbour
 */
MNPuzzleState<4,4> getDownNeighbour(MNPuzzleState<4,4> state);

/**
 * Finds the scores for all game states in an array.
 * @param arr: the Array of game states
 * @return: an array of all scores
 */
double * getNeighbourScores(MNPuzzleState<4,4> *arr);

/**
 * Finds the score for a game state.
 * @param state: the game state
 * @return: the score
 */
double getScore(MNPuzzleState<4,4> state);

/**
 * Solves the given state and returns the time needed.
 * @param state: The game state
 * @return: the time needed to solve the game state
 */
double solve(MNPuzzleState<4,4> state);
/**
 * Sets the heuristics for the solver.
 */
void setHeuristics();

MNPuzzle<4, 4> mnp;
Heuristic<MNPuzzleState<4, 4>> h;
PermutationPDB<MNPuzzleState<4, 4>, slideDir, MNPuzzle<4, 4>> *pdb1;
PermutationPDB<MNPuzzleState<4, 4>, slideDir, MNPuzzle<4, 4>> *pdb2;
PermutationPDB<MNPuzzleState<4, 4>, slideDir, MNPuzzle<4, 4>> *pdb3;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "USAGE: ./hardInstanceSearch [instance number (between 0 and 99)]\n";
        return 0;
    }
    setHeuristics();
    MNPuzzleState<4,4> curr;
    int jj = atoi(argv[1]);
    if (jj < 0 || jj > 99) {
        cout << "USAGE: ./hardInstanceSearch [instance number (between 0 and 99)]\n";
        return 0;
    }
        curr = GetSTPInstance(jj);

        double curr_score = getScore(curr);
        while (true) {

            cout << "CURRENT GAME STATE\n\n";
            printGameState(curr);
            cout << "\nCURRENT SCORE: " << curr_score << "\n\n";
            MNPuzzleState<4, 4> *neighbours = getNeighbours(curr);
            double *neighbour_scores = getNeighbourScores(neighbours);
            int index = -1;
            for (int i = 0; i < 4; i++) {
                if (neighbour_scores[i] > curr_score) {
                    curr_score = neighbour_scores[i];
                    index = i;
                }
                cout << "NEIGHBOUR " << i << "\n\n";
                printGameState(neighbours[i]);
                cout << "\nSCORE OF NEIGHBOUR " << i << ": " << neighbour_scores[i] << "\n\n";
            }
            if (index == -1) {
                cout << "----------------------------------------\nFOUND LOCAL MAXIMUM\n";
                printGameState(curr);
                cout << "With Score : " << curr_score << "\n";
                cout << "----------------------------------------\n";
                break;
            }
            cout << "NEIGBOUR " << index << " CHOSEN\n\n";
            for (int i = 0; i < 16; i++) {
                curr.puzzle[i] = neighbours[index].puzzle[i];
            }
            curr.blank = neighbours[index].blank;
            cout << "--------------------------------------------\n";
        }


    return 0;
}
MNPuzzleState<4, 4> GetSTPInstance(int which)
{
    int instances[100][16] =
            {{14, 13, 15, 7, 11, 12, 9, 5, 6, 0, 2, 1, 4, 8, 10, 3},
             {13, 5, 4, 10, 9, 12, 8, 14, 2, 3, 7, 1, 0, 15, 11, 6},
             {14, 7, 8, 2, 13, 11, 10, 4, 9, 12, 5, 0, 3, 6, 1, 15},
             {5, 12, 10, 7, 15, 11, 14, 0, 8, 2, 1, 13, 3, 4, 9, 6},
             {4, 7, 14, 13, 10, 3, 9, 12, 11, 5, 6, 15, 1, 2, 8, 0},
             {14, 7, 1, 9, 12, 3, 6, 15, 8, 11, 2, 5, 10, 0, 4, 13},
             {2, 11, 15, 5, 13, 4, 6, 7, 12, 8, 10, 1, 9, 3, 14, 0},
             {12, 11, 15, 3, 8, 0, 4, 2, 6, 13, 9, 5, 14, 1, 10, 7},
             {3, 14, 9, 11, 5, 4, 8, 2, 13, 12, 6, 7, 10, 1, 15, 0},
             {13, 11, 8, 9, 0, 15, 7, 10, 4, 3, 6, 14, 5, 12, 2, 1},
             {5, 9, 13, 14, 6, 3, 7, 12, 10, 8, 4, 0, 15, 2, 11, 1},
             {14, 1, 9, 6, 4, 8, 12, 5, 7, 2, 3, 0, 10, 11, 13, 15},
             {3, 6, 5, 2, 10, 0, 15, 14, 1, 4, 13, 12, 9, 8, 11, 7},
             {7, 6, 8, 1, 11, 5, 14, 10, 3, 4, 9, 13, 15, 2, 0, 12},
             {13, 11, 4, 12, 1, 8, 9, 15, 6, 5, 14, 2, 7, 3, 10, 0},
             {1, 3, 2, 5, 10, 9, 15, 6, 8, 14, 13, 11, 12, 4, 7, 0},
             {15, 14, 0, 4, 11, 1, 6, 13, 7, 5, 8, 9, 3, 2, 10, 12},
             {6, 0, 14, 12, 1, 15, 9, 10, 11, 4, 7, 2, 8, 3, 5, 13},
             {7, 11, 8, 3, 14, 0, 6, 15, 1, 4, 13, 9, 5, 12, 2, 10},
             {6, 12, 11, 3, 13, 7, 9, 15, 2, 14, 8, 10, 4, 1, 5, 0},
             {12, 8, 14, 6, 11, 4, 7, 0, 5, 1, 10, 15, 3, 13, 9, 2},
             {14, 3, 9, 1, 15, 8, 4, 5, 11, 7, 10, 13, 0, 2, 12, 6},
             {10, 9, 3, 11, 0, 13, 2, 14, 5, 6, 4, 7, 8, 15, 1, 12},
             {7, 3, 14, 13, 4, 1, 10, 8, 5, 12, 9, 11, 2, 15, 6, 0},
             {11, 4, 2, 7, 1, 0, 10, 15, 6, 9, 14, 8, 3, 13, 5, 12},
             {5, 7, 3, 12, 15, 13, 14, 8, 0, 10, 9, 6, 1, 4, 2, 11},
             {14, 1, 8, 15, 2, 6, 0, 3, 9, 12, 10, 13, 4, 7, 5, 11},
             {13, 14, 6, 12, 4, 5, 1, 0, 9, 3, 10, 2, 15, 11, 8, 7},
             {9, 8, 0, 2, 15, 1, 4, 14, 3, 10, 7, 5, 11, 13, 6, 12},
             {12, 15, 2, 6, 1, 14, 4, 8, 5, 3, 7, 0, 10, 13, 9, 11},
             {12, 8, 15, 13, 1, 0, 5, 4, 6, 3, 2, 11, 9, 7, 14, 10},
             {14, 10, 9, 4, 13, 6, 5, 8, 2, 12, 7, 0, 1, 3, 11, 15},
             {14, 3, 5, 15, 11, 6, 13, 9, 0, 10, 2, 12, 4, 1, 7, 8},
             {6, 11, 7, 8, 13, 2, 5, 4, 1, 10, 3, 9, 14, 0, 12, 15},
             {1, 6, 12, 14, 3, 2, 15, 8, 4, 5, 13, 9, 0, 7, 11, 10},
             {12, 6, 0, 4, 7, 3, 15, 1, 13, 9, 8, 11, 2, 14, 5, 10},
             {8, 1, 7, 12, 11, 0, 10, 5, 9, 15, 6, 13, 14, 2, 3, 4},
             {7, 15, 8, 2, 13, 6, 3, 12, 11, 0, 4, 10, 9, 5, 1, 14},
             {9, 0, 4, 10, 1, 14, 15, 3, 12, 6, 5, 7, 11, 13, 8, 2},
             {11, 5, 1, 14, 4, 12, 10, 0, 2, 7, 13, 3, 9, 15, 6, 8},
             {8, 13, 10, 9, 11, 3, 15, 6, 0, 1, 2, 14, 12, 5, 4, 7},
             {4, 5, 7, 2, 9, 14, 12, 13, 0, 3, 6, 11, 8, 1, 15, 10},
             {11, 15, 14, 13, 1, 9, 10, 4, 3, 6, 2, 12, 7, 5, 8, 0},
             {12, 9, 0, 6, 8, 3, 5, 14, 2, 4, 11, 7, 10, 1, 15, 13},
             {3, 14, 9, 7, 12, 15, 0, 4, 1, 8, 5, 6, 11, 10, 2, 13},
             {8, 4, 6, 1, 14, 12, 2, 15, 13, 10, 9, 5, 3, 7, 0, 11},
             {6, 10, 1, 14, 15, 8, 3, 5, 13, 0, 2, 7, 4, 9, 11, 12},
             {8, 11, 4, 6, 7, 3, 10, 9, 2, 12, 15, 13, 0, 1, 5, 14},
             {10, 0, 2, 4, 5, 1, 6, 12, 11, 13, 9, 7, 15, 3, 14, 8},
             {12, 5, 13, 11, 2, 10, 0, 9, 7, 8, 4, 3, 14, 6, 15, 1},
             {10, 2, 8, 4, 15, 0, 1, 14, 11, 13, 3, 6, 9, 7, 5, 12},
             {10, 8, 0, 12, 3, 7, 6, 2, 1, 14, 4, 11, 15, 13, 9, 5},
             {14, 9, 12, 13, 15, 4, 8, 10, 0, 2, 1, 7, 3, 11, 5, 6},
             {12, 11, 0, 8, 10, 2, 13, 15, 5, 4, 7, 3, 6, 9, 14, 1},
             {13, 8, 14, 3, 9, 1, 0, 7, 15, 5, 4, 10, 12, 2, 6, 11},
             {3, 15, 2, 5, 11, 6, 4, 7, 12, 9, 1, 0, 13, 14, 10, 8},
             {5, 11, 6, 9, 4, 13, 12, 0, 8, 2, 15, 10, 1, 7, 3, 14},
             {5, 0, 15, 8, 4, 6, 1, 14, 10, 11, 3, 9, 7, 12, 2, 13},
             {15, 14, 6, 7, 10, 1, 0, 11, 12, 8, 4, 9, 2, 5, 13, 3},
             {11, 14, 13, 1, 2, 3, 12, 4, 15, 7, 9, 5, 10, 6, 8, 0},
             {6, 13, 3, 2, 11, 9, 5, 10, 1, 7, 12, 14, 8, 4, 0, 15},
             {4, 6, 12, 0, 14, 2, 9, 13, 11, 8, 3, 15, 7, 10, 1, 5},
             {8, 10, 9, 11, 14, 1, 7, 15, 13, 4, 0, 12, 6, 2, 5, 3},
             {5, 2, 14, 0, 7, 8, 6, 3, 11, 12, 13, 15, 4, 10, 9, 1},
             {7, 8, 3, 2, 10, 12, 4, 6, 11, 13, 5, 15, 0, 1, 9, 14},
             {11, 6, 14, 12, 3, 5, 1, 15, 8, 0, 10, 13, 9, 7, 4, 2},
             {7, 1, 2, 4, 8, 3, 6, 11, 10, 15, 0, 5, 14, 12, 13, 9},
             {7, 3, 1, 13, 12, 10, 5, 2, 8, 0, 6, 11, 14, 15, 4, 9},
             {6, 0, 5, 15, 1, 14, 4, 9, 2, 13, 8, 10, 11, 12, 7, 3},
             {15, 1, 3, 12, 4, 0, 6, 5, 2, 8, 14, 9, 13, 10, 7, 11},
             {5, 7, 0, 11, 12, 1, 9, 10, 15, 6, 2, 3, 8, 4, 13, 14},
             {12, 15, 11, 10, 4, 5, 14, 0, 13, 7, 1, 2, 9, 8, 3, 6},
             {6, 14, 10, 5, 15, 8, 7, 1, 3, 4, 2, 0, 12, 9, 11, 13},
             {14, 13, 4, 11, 15, 8, 6, 9, 0, 7, 3, 1, 2, 10, 12, 5},
             {14, 4, 0, 10, 6, 5, 1, 3, 9, 2, 13, 15, 12, 7, 8, 11},
             {15, 10, 8, 3, 0, 6, 9, 5, 1, 14, 13, 11, 7, 2, 12, 4},
             {0, 13, 2, 4, 12, 14, 6, 9, 15, 1, 10, 3, 11, 5, 8, 7},
             {3, 14, 13, 6, 4, 15, 8, 9, 5, 12, 10, 0, 2, 7, 1, 11},
             {0, 1, 9, 7, 11, 13, 5, 3, 14, 12, 4, 2, 8, 6, 10, 15},
             {11, 0, 15, 8, 13, 12, 3, 5, 10, 1, 4, 6, 14, 9, 7, 2},
             {13, 0, 9, 12, 11, 6, 3, 5, 15, 8, 1, 10, 4, 14, 2, 7},
             {14, 10, 2, 1, 13, 9, 8, 11, 7, 3, 6, 12, 15, 5, 4, 0},
             {12, 3, 9, 1, 4, 5, 10, 2, 6, 11, 15, 0, 14, 7, 13, 8},
             {15, 8, 10, 7, 0, 12, 14, 1, 5, 9, 6, 3, 13, 11, 4, 2},
             {4, 7, 13, 10, 1, 2, 9, 6, 12, 8, 14, 5, 3, 0, 11, 15},
             {6, 0, 5, 10, 11, 12, 9, 2, 1, 7, 4, 3, 14, 8, 13, 15},
             {9, 5, 11, 10, 13, 0, 2, 1, 8, 6, 14, 12, 4, 7, 3, 15},
             {15, 2, 12, 11, 14, 13, 9, 5, 1, 3, 8, 7, 0, 10, 6, 4},
             {11, 1, 7, 4, 10, 13, 3, 8, 9, 14, 0, 15, 6, 5, 2, 12},
             {5, 4, 7, 1, 11, 12, 14, 15, 10, 13, 8, 6, 2, 0, 9, 3},
             {9, 7, 5, 2, 14, 15, 12, 10, 11, 3, 6, 1, 8, 13, 0, 4},
             {3, 2, 7, 9, 0, 15, 12, 4, 6, 11, 5, 14, 8, 13, 10, 1},
             {13, 9, 14, 6, 12, 8, 1, 2, 3, 4, 0, 7, 5, 10, 11, 15},
             {5, 7, 11, 8, 0, 14, 9, 13, 10, 12, 3, 15, 6, 1, 4, 2},
             {4, 3, 6, 13, 7, 15, 9, 0, 10, 5, 8, 11, 2, 12, 1, 14},
             {1, 7, 15, 14, 2, 6, 4, 9, 12, 11, 13, 3, 0, 8, 5, 10},
             {9, 14, 5, 7, 8, 15, 1, 2, 10, 4, 13, 6, 12, 0, 11, 3},
             {0, 11, 3, 12, 5, 2, 1, 9, 8, 10, 14, 15, 7, 4, 13, 6},
             {7, 15, 4, 0, 10, 9, 2, 5, 12, 11, 13, 6, 1, 3, 14, 8},
             {11, 4, 0, 8, 6, 10, 5, 13, 12, 7, 14, 3, 1, 2, 9, 15}};

    MNPuzzleState<4, 4> s;
    for (int x = 0; x < 16; x++)
    {
        s.puzzle[x] = instances[which][x];
        if (s.puzzle[x] == 0)
            s.blank = x;
    }
    return s;
}
MNPuzzleState<4,4>* getNeighbours(MNPuzzleState<4,4> state) {
    MNPuzzleState<4,4>* r = new MNPuzzleState<4,4>[4];
    r[0] = getLeftNeighbour(state);
    r[1] = getRightNeighbour(state);
    r[2] = getUpNeighbour(state);
    r[3] = getDownNeighbour(state);
    return r ;
}
MNPuzzleState<4,4> getLeftNeighbour(MNPuzzleState<4,4> state) {
    MNPuzzleState<4,4> r;
    if (state.blank % 4 == 0) {
        r.blank = -1;
        return r;
    }
    for (int i = 0; i< 16; i++) {
        r.puzzle[i] = state.puzzle[i];
    }
    r.blank = state.blank;
    r.puzzle[r.blank] = r.puzzle[r.blank -1];
    r.puzzle[r.blank - 1] = 0;
    r.blank--;
    return r;

}
MNPuzzleState<4,4> getRightNeighbour(MNPuzzleState<4,4> state) {
    MNPuzzleState<4,4> r;
    if (state.blank % 4 == 3) {
        r.blank = -1;
        return r;
    }
    for (int i = 0; i< 16; i++) {
        r.puzzle[i] = state.puzzle[i];
    }
    r.blank = state.blank;
    r.puzzle[r.blank] = r.puzzle[r.blank +1];
    r.puzzle[r.blank + 1] = 0;
    r.blank++;
    return r;

}
MNPuzzleState<4,4> getUpNeighbour(MNPuzzleState<4,4> state) {
    MNPuzzleState<4,4> r;
    if (state.blank < 4) {
        r.blank = -1;
        return r;
    }
    for (int i = 0; i< 16; i++) {
        r.puzzle[i] = state.puzzle[i];
    }
    r.blank = state.blank;
    r.puzzle[r.blank] = r.puzzle[r.blank - 4];
    r.puzzle[r.blank - 4] = 0;
    r.blank = r.blank - 4;
    return r;
}
MNPuzzleState<4,4> getDownNeighbour(MNPuzzleState<4,4> state) {
    MNPuzzleState<4,4> r;
    if (state.blank > 11) {
        r.blank = -1;
        return r;
    }
    for (int i = 0; i< 16; i++) {
        r.puzzle[i] = state.puzzle[i];
    }
    r.blank = state.blank;
    r.puzzle[r.blank] = r.puzzle[r.blank + 4];
    r.puzzle[r.blank + 4] = 0;
    r.blank = r.blank + 4;
    return r;
}

double * getNeighbourScores(MNPuzzleState<4,4>* arr) {
    double* r = new double[4];
    for (int i = 0; i <4; i++){
        r[i] = getScore(arr[i]);
    }
    return r;
}
double getScore(MNPuzzleState<4,4> state) {

    if (state.blank == -1){
        return -1;
    }
    return solve(state);
}
void setHeuristics() {
    MNPuzzleState<4, 4> goal;
    std::vector<int> p1 = {0, 1, 2, 3, 4, 5, 6, 7};
    //std::vector<int> p2 = {0, 8, 9, 10, 11, 12, 13, 14, 15};
    std::vector<int> p2 = {0, 8, 9, 12, 13};
    std::vector<int> p3 = {0, 11, 12, 13, 14, 15};

    pdb3 = new LexPermutationPDB<MNPuzzleState<4, 4>, slideDir, MNPuzzle<4, 4>>(&mnp, goal, p3);
    pdb2 = new LexPermutationPDB<MNPuzzleState<4, 4>, slideDir, MNPuzzle<4, 4>>(&mnp, goal, p2);
    pdb1 = new LexPermutationPDB<MNPuzzleState<4, 4>, slideDir, MNPuzzle<4, 4>>(&mnp, goal, p1);
    if (pdb1->Load(prefix) == false)
    {
        cout << "No pdb 1 found\n";
        pdb1->BuildPDB(goal, std::thread::hardware_concurrency());
        pdb1->Save(prefix);
    }
    if (pdb2->Load(prefix) == false)
    {
        cout << "No pdb 2 found\n" ;
        pdb2->BuildPDB(goal, std::thread::hardware_concurrency());
        pdb2->Save(prefix);
    }
    if (pdb3->Load(prefix) == false)
    {
        cout << "No pdb 3 found\n" ;
        pdb3->BuildPDB(goal, std::thread::hardware_concurrency());
        pdb3->Save(prefix);
    }
    h.lookups.resize(0);
    //h.lookups.push_back({kAddNode, 1, 2});
    h.lookups.push_back({kMaxNode, 1, 4});
    h.lookups.push_back({kLeafNode, 0, 0});
    h.lookups.push_back({kLeafNode, 1, 1});
    h.lookups.push_back({kLeafNode, 2, 2});
    h.lookups.push_back({kLeafNode, 3, 3});

    h.heuristics.resize(0);
    h.heuristics.push_back(&mnp);
    h.heuristics.push_back(pdb1);
    h.heuristics.push_back(pdb2);
    h.heuristics.push_back(pdb3);
}

double solve(MNPuzzleState<4,4> state){
    MNPuzzleState<4, 4> goal;
    std::vector<MNPuzzleState<4, 4>> statepath;
    Timer t2;
    TemplateAStar<MNPuzzleState<4, 4>, slideDir, MNPuzzle<4, 4>> astar;
    astar.SetHeuristic(&h);
    t2.StartTimer();
    astar.GetPath(&mnp, state, goal, statepath);
    t2.EndTimer();
    return t2.GetElapsedTime();
}

void printGameState(MNPuzzleState<4,4> state) {
    if (state.blank == -1){
        cout << "\n NO STATE \n";
        return;
    }
    cout << '\n';
    for (int i = 0; i < 4; i++){
        cout << '|';
        for (int j = 0; j <4; j++){
            if (state.puzzle[i*4 + j] < 10) {
                cout << " ";
            }
            cout << state.puzzle[i*4 + j];
            cout << " ";
        }
        cout << "|\n";
    }
}
