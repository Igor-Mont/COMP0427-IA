#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <iostream>
#include <functional>
#include <map>
#include <vector>

#include "../utils.cpp"

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
std::shared_ptr<MCT_Node<GameState, A>> select(std::shared_ptr<MCT_Node<GameState, A>> n) {
  if (not n->children.empty()) {
    auto comparison = [](auto n1, auto n2) { 
      auto r1 = ucb(*(n1.first));
      auto r2 = ucb(*(n2.first));
      auto result = r1 < r2;
      std::cout << r1 << " " << r2 << std::endl;
      return result; 

    };
    return select(std::max_element(n->children.begin(), n->children.end(), comparison)->first);
  }
  return n;
}
/*
 * Expand the leaf node by adding all its children states.
 */
template <typename A>
std::shared_ptr<MCT_Node<GameState, A>> expand(std::shared_ptr<MCT_Node<GameState, A>> n, Game<A>& game) {
  if (n->children.empty() and not game.is_terminal(n->state)) {
    for (const auto& action : game.actions(n->state)) {
      auto state = game.result(n->state, action);
      std::shared_ptr<MCT_Node<GameState, A>> node = std::make_shared<MCT_Node<GameState, A>>(state, n);

      n->children[node] = action;
    }

    std::cout << "\nexpand function" << std::endl;
    for (const auto& pair : n->children) {
      const auto key = pair.first;
      const A& value = pair.second;
      std::cout << "Action: " << value << ", Node Address: " << key << std::endl;
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
  std::cout << "simulate utility result: " << -v << std::endl;
  return -v;
}

/*
 * Passes the utility back to all parent nodes.
 */
template <typename A>
void backprop(std::shared_ptr<MCT_Node<GameState, A>> n, double utility) {
  if (utility > 0) {
    n->U += utility;
  }
  n->N++;
  if (n->parent != nullptr) {
    backprop(n->parent, -utility);
  }
}

template <typename A>
A monte_carlo_tree_search(GameState state, Game<A>& game, int N=1000) {
  std::shared_ptr<MCT_Node<GameState, A>> root = std::make_shared<MCT_Node<GameState, A>>(state);
  for (size_t i{}; i < 1000; i++) {
    auto leaf = select(root);

    std:: cout << "\nselect leaf" << std::endl;
    for(auto index2d : leaf->state.board) {
      std::cout << index2d.first << std::endl;
    }

    auto child = expand(leaf, game);

    for(auto index2d : child->state.board) {
      std::cout << index2d.first << std::endl;
    }

    // auto keys = root->children.begin(); return keys->second;
    //
    auto result = simulate(game, child->state);
    backprop(child, result);
  }

  std::cout << "\nroot childrens" << std::endl;
  for (const auto& pair : root->children) {
    const auto key = pair.first;
    const A& value = pair.second;
    std::cout << "N: " << key->N << ", U: " << key->U << ", Action: " << value << ", Node Address: " << key << std::endl;
  }

  auto comparison = [](auto p1, auto p2){ return p1.first->N < p2.first->N; };
  return std::max_element(root->children.begin(), root->children.end(), comparison)->second;
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
    return state.moves;
  }

  GameState result(GameState state, Index2D move) const override {
    if (not is_in(state.moves, move)) {
      return state;
    }
    auto board = state.board;
    board[move] =  state.to_move;
    auto moves = state.moves;
    remove(moves, move);
    return GameState{
      state.to_move == 'X' ? 'O' : 'X',
      compute_utility(board, move, state.to_move),
      board,
      moves
    };
  }

  double utility(GameState state, char player_mark) const override {
    return player_mark == 'X' ? state.utility : -state.utility;
  }

  bool is_terminal(GameState state) {
    return state.utility != 0 or state.moves.size() == 0;
  }

  void display(GameState state) {
    auto board = state.board;
    for (int x = 1; x < h + 1; x++) {
      for (int y = 1; y < h + 1; y++) {
        auto it = board.find({ x, y });
        auto el = it == board.end() ? '.' : it->second;
        std::cout << el << " ";
      }
      std::cout << "\n";
    }
  }

  double compute_utility(std::map<Index2D, char> board, Index2D move, char player_mark) const {
    if (k_in_row(board, move, player_mark, {0, 1}) or
        k_in_row(board, move, player_mark, {1, 0}) or
        k_in_row(board, move, player_mark, {1, -1}) or
        k_in_row(board, move, player_mark, {1, 1})) {
      return player_mark == 'X' ? 1 : -1;
    }
    return 0;
  }

  bool k_in_row(std::map<Index2D, char> board, Index2D move, char player_mark, Index2D delta_x_y) const {
    auto [delta_x, delta_y] = delta_x_y;
    auto [x, y] = move;
    // number of moves in row.
    int n = 0;

    while (board.find({ x, y })->second == player_mark) {
      n += 1;
      x += delta_x;
      y += delta_y;
    }
    x = move.first;
    y = move.second;

    while (board.find({ x, y })->second == player_mark) {
      n += 1;
      x += delta_x;
      y += delta_y;
    }
    n--; // because we counted move itself twice.
    return n >= k;
  }

  int h;
  int v;
  int k;
};
