#include "../search.cpp"
#include "../../utils.cpp"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <ostream>
#include <vector>
#include <limits>

using namespace std;

int maxValue = std::numeric_limits<int>::max();

std::random_device rd;
std::mt19937 gen(rd());  // Mersenne Twister
std::uniform_real_distribution<> dis(0, 1);  // Gera valores entre 1 e maxValue
std::uniform_int_distribution<> dis_int(1, maxValue);  // Gera valores entre 1 e maxValue
//
template<typename S>
vector<int> weighted_by(vector<S> population, function<int(S)> fitness_fn) {
  vector<int>population2;
  transform(population.begin(), population.end(), back_inserter(population2), [fitness_fn](S x) {
    return fitness_fn(x); 
  });

  return population2;
}

template<typename S>
vector<S> weights_random_choices(vector<S> population, vector<int> weights, int parents) {

  int total = 0, accumulated = 0, counter = 0;
  vector<S> result;

  for(int weight : weights){
    total += weight;
  }

  int rnd = dis_int(gen) % total;

  for(int i = 0; i < weights.size(); i++) {
    accumulated += weights[i];

    if(counter < parents && rnd < accumulated) {
      result.push_back(population[i]);
      counter++;
    }
  }

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
  int n = dis_int(gen) % child.size();
  int gene = dis_int(gen) % gene_pool.size();

  while(child[n] == gene_pool[gene]) {
    n = dis_int(gen) % child.size();
    gene = dis_int(gen) % gene_pool.size();
  };

  child[n] = gene_pool[gene];

  return child;
}

int fitness_treshold(int size) {
  return (int)(size * ((size - 1) / 2.0));
}

template<typename S>
int fitness_fn(S individual) {
  int collisions = 0;
  int size = individual.size();

  for(int i = 0; i < size - 1; i++) {
    for(int j = 1; j < size; j++) {
      if(i+j < size) {
        int target = individual[i + j];
        bool at_bounds_upwards = individual[i] + j <= size;
        bool at_bounds_downwards = individual[i] - j > 0;
        bool fowardRowCollision = individual[i] == target;
        bool fowardAscendCollision = at_bounds_upwards && individual[i] + j == target; 
        bool fowardDescendCollision = at_bounds_downwards && individual[i] - j == target;
        if(fowardRowCollision || fowardAscendCollision || fowardDescendCollision) {
          collisions++;
        }
      }
    }
  }

  return fitness_treshold(size) - collisions;
}

int find_fittest_individual(vector<int> weights) {
  int n = weights[0];
  int i_result = 0;
  for(int i = 1; i < weights.size(); i++) {
    if(n < weights[i]) {
      i_result = i;
    }
  }

  return i_result;
}

template<typename S>
S genectic_algoritm(vector<S> population, function<int(S)> fitness_fn, double mutation_chance, S gene_pool, int parents) {

  double randomValue = dis(gen);

  int attemptNumber = 0;
  int treshold = fitness_treshold(population[0].size());
  int i = 0;
  vector<int> weights = weighted_by<S>(population, fitness_fn); 
  do {
    vector<S> new_population; 
    for(int i = 0; i < population.size(); i++) {
      vector<S> result = weights_random_choices<S>(population, weights, parents);
      S child = reproduce<S>(result[0], result[1]);
      if(dis(gen) < mutation_chance) child = mutate<S>(child, gene_pool);
      new_population.push_back(child);
    }
    population = new_population;
    weights = weighted_by<S>(population, fitness_fn);
    int i = find_fittest_individual(weights);
  } while(weights[i] < treshold && attemptNumber++ < 50000);
  cout << attemptNumber << endl; 
  return population[i]; 
}

