#define CATCH_CONFIG_MAIN
#include "../../catch.hpp"
#include "simulated_annealing.cpp"

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
  SECTION("finds maximum in balanced grid") {
  }

  SECTION("finds maximum in unbalanced grid") {
  }
}
