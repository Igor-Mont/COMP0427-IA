#include <random>
#include <functional>
#include <algorithm>
#include <vector>

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
