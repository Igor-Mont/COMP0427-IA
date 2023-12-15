#include <optional>
#include <stdexcept>
#include <iostream>
#include <functional>
#include <map>
#include <vector>

#include "../../utils.cpp"

// ---------------------------------------------------------------------------------
// Games.

/*
 * Represents the state of a board game.
 */
struct GameState {
  char to_move;
  double utility;
  std::map<Index2D, char> board;
  std::vector<Index2D> moves;
};

std::ostream& operator<<(std::ostream& s, GameState state) {
  return s;
}

/*
 * Interface similar to a problem, but with utility and a terminal test
 * instead of path cost and goal test for each state.
 */

template <typename A>
struct Game {
  /*
   * List of the allowable moves at this state.
   */
  virtual std::vector<A> actions(GameState state) const = 0;

  /*
   * State that results from making a move from a state.
   */
  virtual GameState result(GameState state, A move) const = 0;

  /*
   * Returns the value of the given final state to player.
   */
  virtual double utility(GameState state, char player_mark) const = 0;

  /*
   * True if the given state is terminal for the game.
   */
  bool is_terminal(GameState state) {
    return actions(state).empty();
  }

  /*
   * Return the player whose move it is in this state.
   */
  char to_move(GameState state) {
    return state.to_move;
  }

  template <typename M>
  using PlayerFunc = std::function<M(Game<M>& game, GameState state)>;

  /*
   * Play an n-person, move-alternating game, such as TicTacToe, Chess, Go, etc.
   */
  double play_game(std::vector<PlayerFunc<A>> players) {
    auto state = initial;
    while (true) {
      for (auto const& player : players) {
        try {
          auto move = player(*this, state);
          state = result(state, move);
        } catch (std::exception& e) {}

        if (is_terminal(state)) {
          std::cout << state;
          return utility(state, to_move(initial));
        }
      }
    }
    return {};
  }

  void display(GameState state) {
    std::cout << state;
  }

  GameState initial;
};

// ---------------------------------------------------------------------------------
// Algorithm.

/*
 * Select a leaf node in the tree.
 */
template <typename A>
MCT_Node<GameState, A> select(MCT_Node<GameState, A> n) {
  if (not n.children.empty()) {
    auto keys = get_keys(n.children);
    auto comparison = [](auto n1, auto n2) { return ucb(n1) < ucb(n2); };
    return select(*std::max_element(keys.begin(), keys.end(), comparison));
  }
  return n;
}

/*
 * Expand the leaf node by adding all its children states.
 */
template <typename A>
MCT_Node<GameState, A> expand(MCT_Node<GameState, A>& n, Game<A>& game) {
  if (n.children.empty() and not game.is_terminal(n.state)) {
    for (const auto& action : game.actions(n.state)) {
      auto state = game.result(n.state, action);
      auto parent = std::make_shared<MCT_Node<GameState, A>>(n);
      n.children[MCT_Node(state, parent)] = action;
    }
  }
  return select(n);
}

/*
 * Simulate the utility of current state by random picking a step.
 */
template <typename A>
double simulate(Game<A>& game, GameState state) {
  auto player_mark = game.to_move(state);
  while (not game.is_terminal(state)) {
    auto action = random_choice(game.actions(state));
    state = game.result(state, action);
  }
  auto v = game.utility(state, player_mark);
  return -v;
}

/*
 * Passes the utility back to all parent nodes.
 */
template <typename A>
void backprop(MCT_Node<GameState, A> n, double utility) {
  if (utility > 0) {
    n.U += utility;
  }
  n.N++;
  if (n.parent != nullptr) {
    backprop(*n.parent, -utility);
  }
}

template <typename A>
A monte_carlo_tree_search(GameState state, Game<A>& game, int N=1000) {
  auto root = MCT_Node<GameState, A>(state);
  for (size_t i{}; i < N; i++) {
    auto leaf = select(root);
    auto child = expand(leaf, game);
    auto result = simulate(game, child.state);
    backprop(child, result);
  }

  auto comparison = [](auto p1, auto p2){ return p1.first.N < p2.first.N; };
  return std::max_element(root.children.begin(), root.children.end(), comparison)->second;
}

// ---------------------------------------------------------------------------------
// Players.

template <typename A>
A random_player(Game<A>& game, GameState state) {
  return random_choice(game.actions(state));
}

template <typename A>
A mcts_player(Game<A>& game, GameState state) {
  return monte_carlo_tree_search(state, game);
}

// ---------------------------------------------------------------------------------
// Game implementations.

struct TicTacToe : Game<Index2D> {
  TicTacToe(int h = 3, int v = 3, int k = 3)
  : Game<Index2D>{},
    h{ h },
    v{ v },
    k{ k } {
    std::vector<Index2D> moves;
    for (size_t x = 1; x < h + 1; x++) {
      for (size_t y = 1; y < v + 1; y++) {
        moves.emplace_back(x, y);
      }
    }
    initial = { 'X', 0, {}, moves };
  }

  std::vector<Index2D> actions(GameState state) const override  {
    return {};
  }
  GameState result(GameState state, Index2D move) const override {
    return {};
  }
  double utility(GameState state, char player_mark) const override {
    return {};
  }

  int h;
  int v;
  int k;
};
