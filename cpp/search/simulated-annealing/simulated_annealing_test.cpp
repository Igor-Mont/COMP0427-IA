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

TEST_CASE("random_choice") {
  SECTION("throws out of range exception when given empty vector") {
    std::vector<int> vec{};
    REQUIRE_THROWS_AS(random_choice(vec), std::out_of_range);
  }

  SECTION("picks a valid element") {
    std::vector<int> vec{ 1, 2, 3, 4 };
    auto picked = random_choice(vec);
    REQUIRE((picked >= 1 && picked <= 4));
  }
}
