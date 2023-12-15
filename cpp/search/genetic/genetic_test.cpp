#include <algorithm>
#define CATCH_CONFIG_MAIN
#include "../../catch.hpp"
#include "genetic.cpp"

using namespace std;
using state = vector<int>;

TEST_CASE("mutate function") {
  state queen_positions = {3,2,7,4,8,5,5,2};
  state gene_pool = {1,2,3,4,5,6,7,8};
  srand(time(0));
  state result = mutate<state>(queen_positions, gene_pool); 
  REQUIRE(result != queen_positions);
}

TEST_CASE("reproduce function") {
  state p1 = {3,2,7,4,8,5,5,2};
  state p2 = {8,1,7,2,6,3,4,2};

  state child = reproduce<state>(p1, p2);
  REQUIRE(child.size() == p1.size());
  // print_array("reproduce function: ", child);
}

TEST_CASE("matrixToVector should return vector that represent the matrix") {
  Matrix<int> NQueenBoard = {
    {
      { 1, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 1, 0, 0, 0, 0, 1, 0 },
      { 0, 0, 0, 1, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 1, 0, 0 },
      { 0, 0, 1, 0, 0, 0, 0, 1 },
      { 0, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 1, 0, 0, 0 }
    }
  };

  state NQueenVector = matrixToVector<state>(NQueenBoard, 8);
  // print_array("n queen in vector form: ", NQueenVector);
  
  REQUIRE(verifyTransform<state, int>(NQueenBoard, NQueenVector));
}

TEST_CASE("fitness function") {
  state solution = {8,2,5,3,1,7,4,6};
  state collision_2 = {8,3,5,3,1,7,4,6};
  state collision_6 = {8,3,4,3,1,7,4,6};
  state collision_28 = {1,2,3,4,5,6,7,8};
  state collision =  {4, 7, 3, 6, 2, 5, 8, 1}  ;

  REQUIRE(fitness_fn<state>(solution) == 28);
  REQUIRE(fitness_fn<state>(collision_2) == 26);
  REQUIRE(fitness_fn<state>(collision_6) == 22);
  REQUIRE(fitness_fn<state>(collision_28) == 0);
  REQUIRE(fitness_fn<state>(collision) == 27);
}
// need implement the fitness functions for NQueenProblem
state solution = {8,2,5,3,1,7,4,6};
state collision_2 = {8,3,5,3,1,7,4,6};
state collision_6 = {8,3,4,3,1,7,4,6};
state collision_28 = {1,2,3,4,5,6,7,8};
vector<state> population;

TEST_CASE("weighted_by function") {
  population.push_back(solution);
  population.push_back(collision_2);
  population.push_back(collision_6);
  population.push_back(collision_28);

  vector<int> weights = weighted_by<state>(population, fitness_fn<state>);
  vector<int> response = {28, 26, 22, 0};

  REQUIRE(weights == response);
}

TEST_CASE("weightsr_random_choices function") {
  vector<int> weights = {28, 26, 22, 0};

  auto result = weights_random_choices<state>(population, weights, 2);
  // weights_random_choices<state>(population, weights, 2);

  print_array<int>("first parent", result[0]);
  print_array<int>("secont parent", result[1]);
}

state generateRandomIndividual(int size) {
  state individual;
  
  for(int i = 0; i < size; i++) {
    int n = rand() % size;
    individual.push_back(n);
  }

  return individual;
}

vector<state> generateRandomPopulation(int populationSize, int individualSize) {
  vector<state> population;
  for (int i = 0; i < populationSize; i++) {
    state individual = generateRandomIndividual(individualSize);
    population.push_back(individual);
  }
  return population;
}

TEST_CASE("integrate all function") {
  vector<state> population = generateRandomPopulation(20, 8);
  for(int i = 0; i < population.size(); i++) print_array("individual ", population[i], i);
  double probability = 0.1;
  state gene_pool = {1,2,3,4,5,6,7,8};
  state result = genectic_algoritm<state>(population, fitness_fn<state>, probability, gene_pool, 2, 28);
  REQUIRE(fitness_fn<state>(result) == 28);
  print_array("dentro teste: ", result);
}
