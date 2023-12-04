#include <iostream>
#include <array>
#include <algorithm>
#include <ostream>
#include <queue>
#include <functional>
#include <utility>
#include <memory>
#include <vector>
#include <map>
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
    action{},
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

/*
 * Expand a node, generating the children node.
 */
template <typename S, typename A>
std::vector<Node<S, A>> expand(Problem<S, A>& problem, std::shared_ptr<Node<S, A>> node) {
  auto s = node->state;
  std::vector<Node<S, A>> result;

  for (const auto& action : problem.actions(s)) {
    auto new_s = problem.result(s, action);
    auto cost = node->path_cost + problem.action_cost(node->state, action, new_s);
    auto parent = std::shared_ptr<Node<S, A>>(node);
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

template <typename S, typename A>
using KeyPair = std::pair<double, std::shared_ptr<Node<S, A>>>;

template <typename S, typename A>
using ToDouble = std::function<double(const Node<S, A>&)>;

template <typename S, typename A>
struct PriorityQueue {
  PriorityQueue(std::vector<std::shared_ptr<Node<S, A>>> items, ToDouble<S, A> f)
    : f{ f }, pq{} {
    for (std::shared_ptr<Node<S, A>> item : items) {
      push(item);
    }
  }

  void push(std::shared_ptr<Node<S, A>> item) { 
    KeyPair<S, A> new_elem{ f(*item), item };
    pq.push(new_elem);
  }

  std::shared_ptr<Node<S, A>> pop() {
    auto item = pq.top().second;
    pq.pop();
    return item;
  }

  std::shared_ptr<Node<S, A>> top() {
    return pq.top().second;
  }

  operator bool() const {
    return !pq.empty();
  }

  size_t len() const {
    return pq.size();
  }

private:
  std::priority_queue<KeyPair<S, A>> pq;
  ToDouble<S, A> f;
};

// ---------------------------------------------------------------------------------
// Search algorithms: Best-First

template <typename S, typename A>
std::shared_ptr<Node<S, A>> best_first_search(Problem<S, A>& problem, ToDouble<S, A> f) {
  std::shared_ptr<Node<S, A>> initial_node = std::make_shared<Node<S, A>>(problem.initial);
  auto frontier = PriorityQueue<S, A>{ { initial_node }, f };
  auto reached = std::map<S, std::shared_ptr<Node<S, A>>>{
    { problem.initial, initial_node }
  };

  while (frontier) {  // overloaded bool conversion
    std::shared_ptr<Node<S, A>> current_node = frontier.pop();
    if (problem.is_goal(current_node->state)) {
      return current_node;
    }

    for (const Node<S, A>& child : expand(problem, current_node)) {
      S s = child.state;
      auto found_state = reached.find(s);
      bool contains = found_state != reached.end();

      if (!contains || child.path_cost < found_state->second->path_cost) {
        reached[s] = std::make_shared<Node<S, A>>(child);
        frontier.push(reached[s]);
      }
    }
  }

  return nullptr;  // Indica que não foi encontrado um caminho
}


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
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        if (state[i][j] != this->goal[i][j]) {
          return false;
        }
      }
    }
    return true;
  }

  S result(const S state, const A action) const override {
    Index blank_index = find_blank_square(state);
    int i = blank_index.row;
    int j = blank_index.col;
    S new_state = state;

    if (action == UP) {
      new_state[i][j] = state[i-1][j];
      new_state[i-1][j] = 0;
    }
    if (action == DOWN) {
      new_state[i][j] = state[i+1][j];
      new_state[i+1][j] = 0;
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
      possible_actions.erase(std::remove(possible_actions.begin(), possible_actions.end(), UP), possible_actions.end());
    }
    if (indexes.row == 2) {
      possible_actions.erase(std::remove(possible_actions.begin(), possible_actions.end(), DOWN), possible_actions.end());
    }
    if (indexes.col == 0) {
      possible_actions.erase(std::remove(possible_actions.begin(), possible_actions.end(), LEFT), possible_actions.end());
    }
    if (indexes.col == 2) {
      possible_actions.erase(std::remove(possible_actions.begin(), possible_actions.end(), RIGHT), possible_actions.end());
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
        auto current = node.state[i][j];
        auto it = indexes.find(current);
        if(it != indexes.end()) {
          index = it->second;
          heuristic_value += abs(index.row - i) + abs(index.col - j);
        }
      }
    }
    return -heuristic_value;
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

