#define CATCH_CONFIG_MAIN
#include "../../catch.hpp"
#include "genetic.cpp"

using namespace std;
using state = vector<int>;

vector<state> population = {
    {1, 2, 3, 4, 5, 6, 7, 8},
    {8, 7, 6, 5, 4, 3, 2, 1},
    {3, 4, 5, 6, 7, 8, 1, 2},
    {4, 5, 6, 7, 8, 1, 2, 3},
    {6, 7, 8, 1, 2, 3, 4, 5},
    {2, 3, 4, 5, 6, 7, 8, 1}
};

TEST_CASE("mutate function") {
  state queen_positions = {3,2,7,4,8,5,5,2};
  state gene_pool = {1,2,3,4,5,6,7,8};
  srand(time(0));
  state result = mutate<vector<int>>(queen_positions, gene_pool); 
  REQUIRE(result != queen_positions);
}

TEST_CASE("reproduce function: ") {
  state p1 = {3,2,7,4,8,5,5,2};
  state p2 = {8,1,7,2,6,3,4,2};

  state child = reproduce<state>(p1, p2);
  REQUIRE(child.size() == p1.size());
  print_array("reproduce function", child);
}

// need implement the fitness functions for NQueenProblem

// TEST_CASE("weighted_by function") {
//  vector<double> weights = weighted_by<state>(population,  [](const state& x) { return 1.2; });
// }

// TEST_CASE("weightsr_random_choices function") {
//   vector<double> weights;
//   pair<state, state> p1, p2 = weights_random_choices<state>(population, weights);
// }
