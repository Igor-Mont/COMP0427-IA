#include <memory>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>
#include <queue>

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

  S state;
  A action;
  double path_cost;
  int depth;
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

  /*
   * For optimization problems (hill climbing, gradient descent, simulated annealing...) each state
   * has an associated value which can be {max,min}imized.
   *
   * This is not set as pure (= 0) because not all problems are optimization problems.
   */
  virtual double value(S state) const {
    throw std::logic_error{ "Function not yet implemented" };
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


