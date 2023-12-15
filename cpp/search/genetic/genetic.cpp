#include "../search.cpp"
#include "../../utils.cpp"
#include <algorithm>
#include <cstdlib>
#include <functional>
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
vector<double> weighted_by(vector<S> population, function<double(const S&)> fitness) {
  vector<double>population2;
  transform(population.begin(), population.end(), back_inserter(population), [fitness](S x) {
    return fitness(x); 
  });

  return population2;
}

template<typename S>
pair<S, S> weights_random_choices(vector<S> population, vector<double> weights) {
  int total, accumulated, counter = 0;
  for(auto weight : weights){
    total += weight;
  }
  vector<S> result;
  int rnd = rand() % total;

  for(int i = 0; i < population.size(); i++) {
    accumulated += weights[i];

    if(counter < 2 && rnd < accumulated) {
      result.push_back(population[i]);
    }
  }

  if(counter < 2) {
    if(population[0] != result[0]) {
      result.push_back(population[0]);
    } else {
      result.push_back(population[1]);
    }
  }

  return {result[0], result[1]};
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

template<typename S>
auto genectic_algoritm(vector<S> population, function<double(S)> fitness, double mutation_chance) {
  do {
    auto weights = weighted_by(population, fitness);
    vector<S> population2; 
    for(int i = 1; i < population.size(); i++) {
      pair<S, S> parent1, parent2 = weights_random_choices(population, weights, 2);
      S child = reproduce(parent1, parent2);
      if(rand() / static_cast<double>(RAND_MAX) < mutation_chance) child = mutate(child);
      population2.push_back(child);
    }
  }
  while(1);
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
