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
      depth{ parent == nullptr ? 0 : parent.depth + 1 } {}

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
  auto s = node.s;
  std::vector<Node<S, A>> result;
  for (const auto& action : problem.actions(s)) {
    auto new_s = problem.result(s, action);
    auto cost = node.path_cost;
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

using ToDouble = std::function<double(double)>;
using KeyPair = std::pair<double, double>;

struct PriorityQueue {
  PriorityQueue(std::vector<double> items, ToDouble f)
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
  ToDouble f;
};

// ---------------------------------------------------------------------------------
// Search algorithms: Best-First

#include <map>

template <typename S, typename A>
Node<S, A> best_first_search(Problem<S, A> problem, ToDouble f) {
  auto node = Node<S, A>(problem.initial);
  auto frontier = PriorityQueue{ { node }, f };
  auto reached = std::map<S, Node<S, A>>{ 
    { problem.initial, node }
  };
  while (frontier) {
    node = frontier.pop();
    if (problem.is_goal(node.state))
      return node;
    for (auto child : expand(problem, node)) {
      auto s = child.state;
      if (reached.contains(s) or child.path_cost < reached[s].path_cost) {
        reached[s] = child;
        frontier.push(child);
      }
    }
  }
}

int main() {
  return 0;
}
