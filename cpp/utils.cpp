#include <random>
#include <cmath>
#include <map>
#include <functional>
#include <memory>
#include <algorithm>
#include <string>
#include <vector>
#include <utility>
#include <iostream>

/*
 * Cleaner alias for duples.
 */
using Index2D = std::pair<int, int>;

template<typename S>
S generateRandomIndividual(int size) {
  S individual;
  
  for(int i = 0; i < size; i++) {
    int n = rand() % size;
    individual.push_back(n);
  }

  return individual;
}

template<typename S>
std::vector<S> generateRandomPopulation(int populationSize, int individualSize) {
  std::vector<S> population;
  for (int i = 0; i < populationSize; i++) {
    S individual = generateRandomIndividual<S>(individualSize);
    population.push_back(individual);
  }
  return population;
}

std::ostream& operator<<(std::ostream& os, const Index2D& value) {
  os << "(" << value.first << ", " << value.second << ")";
  return os;
}

template <typename S>
using Matrix = std::vector<std::vector<S>>;

template <typename S, typename T>
bool verifyTransform(Matrix<T> matrix, S vector) {
  for(int i = 0; i < vector.size(); i++) {
    if(matrix[vector[i]-1][i] != 1) return false;
  }

  return true;
}

template <typename T>
void print_array(std::string msg, const std::vector<T>& array, int i = 0) {
    std::cout << i << " " << msg << "[ ";
    for (const auto& element : array) {
        std::cout << element << " ";
    }
    std::cout << "]\n";
}
/*
 * Return true with probability p.
 */
bool probability(double p) {
  std::random_device rd{};
  std::uniform_real_distribution<double> distribution{ 0.0, 1.0 };
  return p > distribution(rd);
}

/*
 * Picks a random element from a vector.
 */
template <typename T>
T random_choice(const std::vector<T>& vec) {
  if (vec.empty()) {
    throw std::out_of_range("Empty vector");
  }
  std::random_device rd{};
  std::uniform_int_distribution<size_t> distribution(0, vec.size() - 1);
  return vec[distribution(rd)];
}


/*
 * Returns a + b, element-wise.
 */
template <typename T>
std::vector<T> vector_sum(const std::vector<T>& a, const std::vector<T>& b) {
  std::vector<T> c(a.size());
  std::transform(a.begin(), a.end(), b.begin(), c.begin(), std::plus<T>());
  return c;
}

/*
 * Return a + b, element-wise.
 */
template <typename T>
std::pair<T, T> pair_sum(const std::pair<T, T>& a, const std::pair<T, T>& b) {
  return std::pair<T, T>{ a.first + b.first, a.second + b.second };
}

/*
 * Node in the Monte Carlo search tree, keeps track of the children state.
 */
template <typename S, typename A>
struct MCT_Node {
  MCT_Node(
      S state = {},
      std::shared_ptr<MCT_Node<S, A>> parent = nullptr,
      int U = 0,
      int N = 0
    )
    : parent{ parent },
      state{ state },
      U{ U },
      N{ N },
      children{},
      actions{}
  {}

  bool operator<(const MCT_Node<S, A>& other) const {
    return N < other.N;
  }

  std::shared_ptr<MCT_Node<S, A>> parent;
  S state;
  int U;
  int N;
  std::map<std::shared_ptr<MCT_Node<S, A>>, A> children;
  std::vector<A> actions;
};

/*
 * Returns true if container contains element.
 */
template <typename Container, typename T>
bool is_in(const Container& container, const T& element) {
  return std::find(container.begin(), container.end(), element) != container.end();
}

/*
 * Removes first occurrence of element from the container.
 */
template <typename Container, typename T>
bool remove(Container& container, const T& element) {
  auto it = std::find(container.begin(), container.end(), element);
  if (it != container.end()) {
    container.erase(it);
    return true;
  }
  return false;
}

template <typename S, typename A>
double ucb(MCT_Node<S, A> n, double C = 1.4) {
  if (n.N == 0) {
    return INFINITY;
  }
  return n.U / n.N + C * std::sqrt(std::log(n.parent->N) / n.N);
}

/*
 * Returns a vector of keys of a map.
 */

template <typename T, typename U>
std::vector<T> get_keys(std::map<T, U> m) {
  std::vector<T> keys;
  keys.reserve(m.size());
  std::transform(
    std::begin(m),
    std::end(m),
    std::back_inserter(keys),
    [](const auto& pair) { return pair.first; }
  );
  return keys;
}
