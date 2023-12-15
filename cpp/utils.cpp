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
void print_array(std::string msg, const std::vector<T>& array) {
    std::cout << msg << "[ ";
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
      std::shared_ptr<MCT_Node<S, A>> parent = nullptr,
      S state = {},
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

  double ucb(MCT_Node n, double C = 1.4) {
    if (n.N == 0) {
      return INFINITY;
    }
    return n.U / n.N + C * std::sqrt(std::log(n.parent->N) / n.N);
  }

  std::shared_ptr<MCT_Node<S, A>> parent;
  S state;
  int U;
  int N;
  std::map<MCT_Node<S, A>> children;
  std::vector<A> actions;
};
