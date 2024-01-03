#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

#include "a_star.cpp"

TEST_CASE("A* can solve valid 8 Puzzle") {
  using namespace std::placeholders;
  Matrix initial = {{
    {7, 2, 4},
    {5, 0, 6},
    {8, 3, 1}
  }};

  Matrix goal = {{
    {0, 1, 2},
    {3, 4, 5},
    {6, 7, 8}
  }};

  EightPuzzle<Matrix, Actions> eightPuzzle(initial, goal);
  auto fp = std::bind(&EightPuzzle<Matrix, Actions>::f, eightPuzzle, _1);

  std::shared_ptr<Node<Matrix, Actions>> test = best_first_search<Matrix, Actions>(eightPuzzle, fp);
  REQUIRE(eightPuzzle.is_goal(test->state));
}

TEST_CASE("A* can't solve invalid 8 Puzzle") {
  Matrix fake = {{
    {1, 2, 3},
    {4, 6, 5},  // Troque a posição de 5 e 6 para tornar a matriz insolucionável
    {7, 8, 0}
  }};
}
