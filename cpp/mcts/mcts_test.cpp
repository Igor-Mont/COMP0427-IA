#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include <memory>
#include "mcts.cpp"
#include <vector>
#include <set>
#include <random>
#include <map>

template <typename M>
using PlayerFunc = std::function<M(Game<M>& game, GameState state)>;

GameState gen_state(
  char to_move = 'X',
  std::vector<Index2D> x_positions = {},
  std::vector<Index2D> o_positions = {},
  int h = 3,
  int v = 3
) {
  std::set<Index2D> moves;
  for (int x = 1; x < h + 1; x++) {
    for (int y = 1; y < v + 1; y++) {
      moves.insert(std::make_pair(x, y));
    }
  }
  for (const auto& pos : x_positions) moves.erase(pos);
  for (const auto& pos : o_positions) moves.erase(pos);
  std::vector<Index2D> remaining_moves(moves.begin(), moves.end());

  std::map<Index2D, char> board{};
  for (const auto& pos : x_positions) board[pos] = 'X';
  for (const auto& pos : o_positions) board[pos] = 'O';

  return GameState{ to_move, 0, board, remaining_moves };
}

TEST_CASE("Monte Carlo Tree Search") {
  TicTacToe ttt{};
  // SECTION("test simulate") {
  //   auto state = gen_state(
  //     'X',
  //     {{1, 1}, {3,3}},
  //     {{1,2}, {3,2}}
  //   );
  //   auto node = std::make_shared<MCT_Node<GameState, std::pair<int,int>>>(state);
  //   auto child = expand(node, ttt);
  //   simulate(ttt, node->state);
  // }
  SECTION("plays on middle and wins") {
    auto state = gen_state(
      'X',
      { {1, 1}, {3, 3} }, // X positions
      { {1, 2}, {3, 2} }  // O positions
    );
    Index2D expected_move{ 2, 2 };
    auto move = monte_carlo_tree_search(state, ttt);
    REQUIRE(move == expected_move);
  }

  //SECTION("plays on middle if that is the most likely move") {
  //  auto state = gen_state('X',
  //    { {1, 1}, {3, 3}, {3, 1} }, // X positions
  //    { {1, 2}, {3, 2} }  // O positions
  //  );
  //  Index2D expected_move{ 2, 2 };
  //  auto move = monte_carlo_tree_search(state, ttt);
  //  std::cout << move << std::endl;
  //  REQUIRE(move == expected_move);
  //}

  // SECTION("plays to avoid enemy's win") {
  //   auto state = gen_state(
  //     'X',
  //     { {1, 1} }, // X positions
  //     { {1, 2}, {3, 2} }  // O positions
  //   );
  //   Index2D expected_move{ 2, 2 };
  //   auto move = monte_carlo_tree_search(state, ttt);
  //   REQUIRE(move == expected_move);
  // }

  // SECTION("never loses to a random player in ttt game") {
  //   REQUIRE(ttt.play_game({ mcts_player<Index2D>, random_player<Index2D> }) >= 0);
  // }
 }