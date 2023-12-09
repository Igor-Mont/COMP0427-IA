#include <random>
#include <vector>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <map>
#include <limits>
#include <assert.h>

#include "../search.cpp"
#include "../../utils.cpp"

// ---------------------------------------------------------------------------------
// Utilities.

using ScheduleFunction = std::function<double(size_t)>;

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
 * Initializes a random solution using a variable T (for temperature) which
 * is updated randomly. In case the update has better cost, it is selected,
 * otherwise it's selected with a probability p.
 * 
 * Moving to worst solutions can help to get closer to global optimal solutions.
 */
template <typename S, typename A>
S simulated_annealing(Problem<S, A> problem, ScheduleFunction schedule = exp_schedule()) {
  Node<S, A> current{ problem.initial };
  for (size_t t{}; t < std::numeric_limits<size_t>::max(); t++) {
    auto T = schedule(t);
    if (T == 0) {
      return current.state;
    }
    auto neighbors = expand(problem, current);
    if (neighbors.empty()) {
      return current.state;
    }
    auto next_choice = random_choice(neighbors);
    auto delta_e = problem.value(next_choice.state) - problem.value(current.state);
    if (delta_e > 0 || probability(std::exp(delta_e / T))) {
      current = next_choice;
    }
  }
}

// ---------------------------------------------------------------------------------
// Problems and applications

// Predefined actions for PeakFindingProblem.

enum class Direction {
  W, N, E, S,
  NW, NE, SE, SW,
};

using Grid = std::vector<std::vector<int>>;
using Index2D = std::pair<int, int>;
using ActionTable = std::map<Direction, Index2D>;

ActionTable directions4() {
  return {
    { Direction::W, { -1,  0 } },
    { Direction::N, {  0,  1 } },
    { Direction::E, {  1,  0 } },
    { Direction::S, {  0, -1 } },
  };
}

ActionTable directions8() {
  ActionTable m{ directions4() };
  m.insert({
    { Direction::NW, { -1,  1 } },
    { Direction::NE, {  1,  1 } },
    { Direction::SE, {  1, -1 } },
    { Direction::SW, { -1, -1 } },
  });
  return m;
}

/*
 * Problem of finding the highest peak in a limited grid.
 *
 * The grid is a 2-dimensional array/list whose state is specified by an index.
 */
struct PeakFindingProblem : Problem<Index2D, Direction> {
  PeakFindingProblem(
      Index2D initial,
      Grid grid,
      ActionTable defined_actions = directions4()
  ) : Problem<Index2D, Direction>{ initial, initial },
      grid{ grid },
      defined_actions{ defined_actions },
      n{ grid.size() },
      m{ grid[0].size() } {}

  /*
   * Returns the vector of actions which are allowed to be taken from the given
   * state.
   */
  std::vector<Direction> actions(const Index2D state) const override {
    std::vector<Direction> allowed_actions;
    for (const auto& [action, movement] : defined_actions) {
      auto next_state = pair_sum(state, movement);
      if (valid_state(next_state)) {
        allowed_actions.push_back(action);
      }
    }
    return allowed_actions;
  }

  /*
   * Moves in the direction specified by action.
   */
  Index2D result(const Index2D state, const Direction action) {
    // Since find returns a RB-Tree iterator, we need to do some dereferencing.
    auto movement{ *defined_actions.find(action) };
    return pair_sum(state, movement.second);
  }

  /*
   * Value of a state is the value it is the index to.
   */
  double value(Index2D state) const override {
    auto [x, y] = state;
    assert(x >= 0 && x < n);
    assert(y >= 0 && y < m);
    return grid[x][y];
  }

  bool valid_state(const Index2D state) const {
    return (state.first  >= 0 && state.first  < n) &&
           (state.second >= 0 && state.second < m);
  }

  size_t n, m;
  ActionTable defined_actions;
  Grid grid;
};

