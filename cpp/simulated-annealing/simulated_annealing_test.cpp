#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "simulated_annealing.cpp"

#include <algorithm>

TEST_CASE("Default exponential schedule") {
  auto s = exp_schedule();

  SECTION("starts at 20 with t = 0") {
    auto T = s(0);
    REQUIRE(T == Approx(20L));
  }

  SECTION("zeroes after its limit 100") {
    size_t beyond_limit = 101;
    auto T = s(beyond_limit);
    REQUIRE(T == Approx(0L));
  }
}

TEST_CASE("Simmulated annealing") {
  std::vector<double> sols;
  SECTION("finds maximum in balanced grid") {
    PeakFindingProblem prob{ 
      {0, 0},
      {{ 0, 5, 10, 20},
       {-3, 7, 11, 5}}
    };
    for (size_t i{}; i < 100; i++) {
      sols.push_back(prob.value(simulated_annealing(prob)));
    }
    auto max = std::max_element(sols.begin(), sols.end());
    REQUIRE(*max == Approx(20));
  }

  SECTION("finds maximum in unbalanced grid") {
    PeakFindingProblem prob{ 
      {0, 0},
      {{ 0, 5, 10, 8},
       {-3, 7, 9,  999},
       { 1, 2, 5,  11}},
      directions8()
    };
    for (size_t i{}; i < 100; i++) {
      sols.push_back(prob.value(simulated_annealing(prob)));
    }
    auto max = std::max_element(sols.begin(), sols.end());
    REQUIRE(*max == Approx(999));
  }
}
