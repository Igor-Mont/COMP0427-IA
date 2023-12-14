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
  /* List of the allowable moves at this state. */
  virtual std::vector<A> actions(GameState state) const = 0;
  /* State that results from making a move from a state. */
  virtual GameState result(GameState state, A move) const = 0;
  virtual double utility(GameState state, char player_mark) const = 0;

  /*
   * True if the given state is terminal for the game.
   */
  bool is_terminal(GameState state) {
    return not actions(state);
  }

  /*
   * Return the player whose move it is in this state.
   */
  char to_move(GameState state) {
    return state.to_move;
  }

  template <typename M>
  using Player = std::function<M(Game<M> game, GameState state)>;

  /*
   * Play an n-person, move-alternating game, such as TicTacToe, Chess, Go, etc.
   */
  void play_game(std::vector<Player<A>> players) {
    auto state = initial;
    while (true) {
      for (auto const& player : players) {
        try {
          auto move = player(*this, state);
          state = result(state, move);
        } catch (std::exception& e) { }

        if (is_terminal(state)) {
          std::cout << state;
          return utility(state, to_move(initial));
        }
      }
    }
  }

  void display(GameState state) {
    std::cout << state;
  }

  GameState initial;
};

// ---------------------------------------------------------------------------------
// Players.

template <typename A>
A random_player(Game<A> game, GameState state) {
  return random_choice(game.actions(state));
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
