#include <ostream>
#include <vector>
#include <memory>
#include <string>

// ---------------------------------------------------------------------------------
// Problems and nodes

/*
 * A node in the search tree.
 *
 * It has a S state and an A action, and a solution can be built upon from it if
 * it corresponds to a goal state.
 */
template <typename S, typename A>
struct Node {
  Node(S state, double path_cost = 0.0)
  : state{ state },
    path_cost{ path_cost },
    parent{},
    depth{} {}

  Node(
    S state,
    A action,
    std::shared_ptr<Node<S, A>> parent = nullptr,
    double path_cost = 0.0
  ) : state{ state },
      parent{ parent },
      action{ action },
      path_cost{ path_cost },
      depth{ parent == nullptr ? 0 : parent->depth + 1 } {}

  bool is_root() const {
    return !parent;
  }

  bool operator<(Node<S, A>& other) {
    return path_cost < other.path_cost;
  }

  Node<S, A>& get_parent() const {
    return *parent;
  }
  
  const S state;
  const A action;
  const double path_cost;
  const int depth;
private:
  std::shared_ptr<Node<S, A>> parent;
};

/*
 * A problem, with all properties:
 * - Initial and goal states;
 * - Actions;
 * - Result and cost functions;
 * - h, for informed algorithms.
 */
template<typename S, typename A>
struct Problem {
  Problem(S initial, S goal)
  : initial{ initial }, goal{ goal } {}

  /*
   * Checks if state is a goal state.
   */
  virtual bool is_goal(const S state) const {
    // Assumes S has an appropriate operator==.
    return state == goal;
  }

  /*
   * Return the actions available for the given state.
   */
  virtual std::vector<A> actions(const S state) const = 0 ;

  /*
   * Return the state resultant from applying action to the given state.
   */
  virtual S result(const S state, const A action) const = 0;

  /*
   * The cost of applying an action in state_1 to reach state_2.
   */
  virtual double action_cost(const S state_1, const A action, const S state_2) const {
    return 1;
  }

  /*
   * Heuristic function.
   */
  virtual double h(Node<S, A> node) const {
    return 0;
  }

  S initial;
  S goal;
private:
};

// Indicates an algorithm couldn't find a solution.
Node<std::string, std::string> failure{ "failure" };

/*
 * Expand a node, generating the children node.
 */
template <typename S, typename A>
std::vector<Node<S, A>> expand(Problem<S, A> problem, Node<S, A> node) {
  auto s = node.state;
  std::vector<Node<S, A>> result;
  for (const auto& action : problem.actions(s)) {
    auto new_s = problem.result(s, action);
    auto cost = node.path_cost + problem.action_cost(node.state, action, new_s);
    auto parent = std::make_shared<Node<S, A>>(node);
    result.emplace_back(new_s, action, parent, cost);
  }
  return result;
}

/*
 * The sequence of actions to get to this node.
 */
template <typename S, typename A>
std::vector<A> path_actions(Node<S, A> node) {
  std::vector<A> actions;
  while (!node.is_root()) {
    actions.push_back(node.action);
    node = node.get_parent();
  }
  actions.push_back(node.action);
  return actions;
}

/*
 * The sequence of states to get to this node.
 */
template <typename S, typename A>
std::vector<S> path_states(Node<S, A> node) {
  std::vector<S> states;
  while (!node.is_root()) {
    states.push_back(node.state);
    node = node.get_parent();
  }
  states.push_back(node.state);
  return states;
}

// ---------------------------------------------------------------------------------
// Priority queue

#include <queue>
#include <functional>
#include <utility>

using KeyPair = std::pair<double, double>;

template <typename S, typename A>
using ToDouble = std::function<double(Node<S, A>)>;


template <typename S, typename A>
struct PriorityQueue {
  PriorityQueue(std::vector<Node<S, A>> items, ToDouble<S, A> f)
    : f{ f }, pq{} {
    for (auto item : items) {
      push(item);
    }
  }

  void push(double item) {
    KeyPair new_elem{ f(item), item };
    pq.push(new_elem);
  }

  double pop() {
     auto item = pq.top().second;
     pq.pop();
     return item;
  }

  double top() {
    return pq.top().second;
  }

  operator bool() const {
    return !pq.empty();
  }

  size_t len() const {
    return pq.size();
  }
private:
  std::priority_queue<KeyPair> pq;
  ToDouble<S, A> f;
};

// ---------------------------------------------------------------------------------
// Search algorithms: Best-First

#include <map>

template <typename S, typename A>
Node<S, A> best_first_search(Problem<S, A> problem, ToDouble<S, A> f) {
  auto node = Node<S, A>(problem.initial);
  auto frontier = PriorityQueue<S, A>{ { node }, f };
  auto reached = std::map<S, Node<S, A>>{ 
    { problem.initial, node }
  };
  while (frontier) {
    node = frontier.pop();
    if (problem.is_goal(node.state))
      return node;
    for (auto child : expand(problem, node)) {
      auto s = child.state;
      auto finded_state = reached.find(s);
      bool contains = finded_state != reached.end();

      if (!contains or child.path_cost < reached[s].path_cost) {
        reached[s] = child;
        frontier.push(child);
      }
    }
  }
}

#include <array>
#include <iostream>
#include <algorithm>

using Matrix = std::array<std::array<int, 3>, 3>;

struct Index {
  int row;
  int col;
};

enum Actions {
  UP,
  DOWN,
  LEFT,
  RIGHT
};

template <typename S, typename A>
struct EightPuzzle : public Problem<S, A> {
  EightPuzzle(S initial, S goal)
    : Problem<S, A>(initial, goal) {
    setIndexes();
  }

  void setIndexes() {
    Index index;
    int i = 0;

    for (int j = 0 ; j < 9; j++) {
      index.row = updateRow(i, j-1) - 1;
      index.col = j % 3;
      int currentValue = this->goal[index.row][index.col];
      if (currentValue != 0) {
        indexes[currentValue] = index;
      }
    } 
  }

  bool is_goal(const S state) const override {
    return std::equal(state.begin(), state.end(), this->goal.begin());
  }

  S result(const S state, const A action) const override {
    Index blank_index = find_blank_square(state);
    int i = blank_index.row;
    int j = blank_index.col;
    S new_state = state;

    if (action == UP) {
      new_state[i][j] = state[i+1][j]; 
      new_state[i+1][j] = 0;
    }
    if (action == DOWN) {
      new_state[i][j] = state[i-1][j]; 
      new_state[i-1][j] = 0;
    }
    if (action == LEFT) {
      new_state[i][j] = state[i][j-1]; 
      new_state[i][j-1] = 0;
    }
    if (action == RIGHT) {
      new_state[i][j] = state[i][j+1]; 
      new_state[i][j+1] = 0;
    }

    return new_state;
  }

  std::vector<A> actions(const S state) const override {
    std::vector<A> possible_actions = { UP, DOWN, LEFT, RIGHT };
    Index indexes = find_blank_square(state);

    if (indexes.row == 0) {
      possible_actions.erase(possible_actions.begin());
    }
    if (indexes.row == 2) {
      possible_actions.erase(possible_actions.begin() + 1);
    }
    if (indexes.col == 0) {
      possible_actions.erase(possible_actions.begin() + 2);
    }
    if (indexes.col == 2) {
      possible_actions.erase(possible_actions.begin() + 3);
    }

    return possible_actions;
  }

  int updateRow(int& i, int j) const {
    if ((j+1) % 3 == 0) {
      return ++i;
    };

    return i;
  };

  bool check_solvability(S state) {
    int i = 0;
    int inversions = 0;
    for (int j = 0 ; j < 8; j++) {
      auto current = state[i][j%3];
      auto next = state[updateRow(i, j)][(j+1)%3];

      if (current > next and current != 0 and next != 0) {
        inversions++;
      }
    }

    return inversions % 2 == 0;
  }

  // First Heuristic function
  /* 
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        if(node.state[i][j] == this->goal[i][j]) heuristic_value++;
      }
    }
  */

  double action_cost(const S state_1, const A action, const S state_2) const override {
    return 1;
  }

  double f(Node<S, A> node) const {
    return h(node) + node.path_cost;
  }

  double h(Node<S, A> node) const override {
    int heuristic_value = 0;
    Index index;

    for (int i = 0 ; i < 3 ; i++) {
      for (int j = 0 ; j < 3 ; j++) {
        auto it = indexes.find(node.state[i][j]); 
        if(it != indexes.end()) {
          index = it->second;
          heuristic_value += abs(index.row - i) + abs(index.col - j);
        }
      }
    }

    return heuristic_value;
  }

  Index find_blank_square(S state) const {
    Index zeroIndex;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        if (state[i][j] == 0) {
          zeroIndex.row = i;
          zeroIndex.col = j;
          return zeroIndex;
        }
      }
    }

    zeroIndex.row = -1;
    zeroIndex.col = -1;
    return zeroIndex;
  }
  
  std::map<int, Index> indexes;
};

using namespace std::placeholders;

int main() {
  Matrix initial = {{
    {7, 2, 4},
    {5, 0, 6},
    {8, 3, 1}
  }};

  Matrix fake = {{
    {1, 2, 3},
    {4, 6, 5},  // Troque a posição de 5 e 6 para tornar a matriz insolucionável
    {7, 8, 0}
  }};

  Matrix goal = {{
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8}
}};

  EightPuzzle<Matrix, Actions> eightPuzzle(initial, goal);
  auto fp = std::bind(&EightPuzzle<Matrix, Actions>::f, eightPuzzle, _1);
  Node<Matrix, Actions> test = best_first_search<Matrix, Actions>(eightPuzzle, fp); 
  
  return 0;
}
