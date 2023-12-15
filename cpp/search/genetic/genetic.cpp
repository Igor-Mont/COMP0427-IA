#include "../search.cpp"
#include "../../utils.cpp"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

enum Direction {
  W, N, E, S,
  NW, NE, SE, SW,
};

using ActionTable = map<Direction, Index2D>;

ActionTable actions = {
  { Direction::N, { -1,  0 } },
  { Direction::S, { 1, 0 } },
  { Direction::W, { 0, -1 } },
  { Direction::E, {  0,  1 } },
  { Direction::NW, { -1, -1 } },
  { Direction::NE, { -1,  1 } },
  { Direction::SE, {  1,  1 } },
  { Direction::SW, {  1, -1 } },
};

template<typename S>
vector<int> weighted_by(vector<S> population, function<double(const S&)> fitness_fn) {
  vector<int>population2;
  transform(population.begin(), population.end(), back_inserter(population2), [fitness_fn](S x) {
    return fitness_fn(x); 
  });

  return population2;
}

template<typename S>
vector<S> weights_random_choices(vector<S> population, vector<int> weights, int parents) {
  int total = 0, accumulated = 0, counter = 0;
  for(int weight : weights){
    total += weight;
  }

  vector<S> result;
  int rnd = rand() % total;
  
  for(int i = 0; i < weights.size(); i++) {
    accumulated += weights[i];

    if(counter < parents && rnd < accumulated) {
      result.push_back(population[i]);
      counter++;
    }
  }

  // cout << "counter: " << counter << " total: " << total << endl;
  // print_array("p1: ", result[0]);

  if(counter < parents) {
    if(population[0] != result[0]) {
      result.push_back(population[0]);
    } else {
      result.push_back(population[1]);
    }
  }

  return result;
}

template<typename S>
S reproduce(S parent1, S parent2) {
  int n = rand() % parent1.size();
  S child {};

  for(int i = 0; i < n; i++){
    child.push_back(parent1[i]);
  }

  for(int i = n; i < parent1.size(); i++) {
    child.push_back(parent2[i]);
  }
  
  return child;
}

template<typename S>
S mutate(S child, S gene_pool) {
  int n = rand() % child.size();
  int gene = rand() % gene_pool.size();

  while(child[n] == gene_pool[gene]) {
    n = rand() % child.size();
    gene = rand() % gene_pool.size();
  };

  child[n] = gene_pool[gene];

  return child;
}

int fitness_treshold(int size) {
  return (int)(size * ((size - 1) / 2.0));
}

template<typename S>
int fitness_fn(S child) {
  int collisions = 0;
  int size = child.size();
  for(int i = 0; i < size - 1; i++) {
    for(int j = 1; j < size; j++) {
      int target = child[i + j];
      bool at_bounds_upwards = child[i] + j <= size;
      bool at_bound_downwards = child[i] - j > 0;
      bool fowardRowCollision = child[i] == target;
      bool fowardAscendCollision = at_bounds_upwards && child[i] + j == target; 
      bool fowardDescendCollision = at_bounds_upwards && child[i] - j == target;
      if(fowardRowCollision || fowardAscendCollision || fowardDescendCollision) {
        collisions++;
      }
    }
  }

  return fitness_treshold(size) - collisions;
}

template<typename S>
vector<S> genectic_algoritm(vector<S> population, function<int(S)> fitness_fn, double mutation_chance, S gene_pool, int parents) {
  int attemptNumber = 0;
  do {
    vector<int> weights = weighted_by<S>(population, fitness_fn);
    vector<S> new_population; 
    for(int i = 0; i < population.size(); i++) {
      vector<S> result = weights_random_choices<S>(population, weights, parents);
      S child = reproduce<S>(result[0], result[1]);
      if(rand() / static_cast<double>(RAND_MAX) < mutation_chance) child = mutate<S>(child, gene_pool);
      new_population.push_back(child);
    }
    population = new_population;
  } while(!hasMemberFit(population) && attemptNumber++ < 1000000);

  return population;
}

template<typename S>
S matrixToVector(Matrix<int> matrix, int size) {
  S nQueens;

  for(int i = 0; i < size; i++) {
    for(int j = 0; j < size; j++) {
      if(matrix[j][i] == 1) {
        nQueens.push_back(j+1);
      }
    }
  }
  return nQueens;
}
