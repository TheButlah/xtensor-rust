#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this
                          // in one cpp file
#include <catch2/catch.hpp>
#include <vector>

#include "cpp/bridge/bridge.hpp"

using rust::Slice, xtensor_rust::bridge::RsTensor;

TEST_CASE("Create rust tensor") {

  auto original_tensor = std::vector<uint8_t>{1, 2, 3, 4, 5, 6, 7, 8, 9};
  auto original_shape = std::vector<size_t>{3, 3};
  auto shape_slice =
      Slice<const size_t>(original_shape.data(), original_shape.size());

  auto rs_tensor = RsTensor<uint8_t>(shape_slice, original_tensor.data());
}
