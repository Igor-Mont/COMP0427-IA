#include <random>
#include <vector>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <map>

#include "../search.cpp"

using ScheduleFunction = std::function<double(size_t)>;

// ---------------------------------------------------------------------------------
// Utilities.

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
 * Possible schedule function.
 * Copied from https://github.com/aimacode/aima-python/blob/61d695b37c6895902081da1f37baf645b0d2658a/search.py#L653
 */
ScheduleFunction exp_schedule(int k = 20, double lam = 0.005, int limit = 100) {
  return [=](size_t t) { return t < limit ?  k * std::exp(-lam * t) : 0; };
}


// ---------------------------------------------------------------------------------
// Algorithm

/*
 * TODO: Description & implementation.
 */
template <typename S, typename A>
S simulated_annealing(Problem<S, A> problem, ScheduleFunction schedule = exp_schedule()) {}

// ---------------------------------------------------------------------------------
// Problems and applications

// Predefined actions for PeakFindingProblem.

enum class Direction {
  W, N, E, S,
  NW, NE, SE, SW,
};
using PFAction = std::map<Direction, std::pair<int, int>>;

PFAction directions4() {
  return {
    { Direction::W, { -1,  0 } },
    { Direction::N, {  0,  1 } },
    { Direction::E, {  1,  0 } },
    { Direction::S, {  0, -1 } },
  };
}

PFAction directions8() {
  PFAction m{ directions4() };
  m.insert({
    { Direction::W, { -1,  0 } },
    { Direction::N, {  0,  1 } },
    { Direction::E, {  1,  0 } },
    { Direction::S, {  0, -1 } },
  });
  return m;
}

// TODO: Implement.
struct PeakFingingProblem {};
