cmake_minimum_required(VERSION 3.14)
project(VehiclePowerDistribution CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(_ISOC11_SOURCE)

set(CMAKE_PREFIX_PATH "C:/msys64/mingw64")
find_package(GTest REQUIRED)

add_library(vehicle_lib
  src/sys_vars.cpp
  src/engine.cpp
  src/power_transmission.cpp
  src/bcm.cpp
)
target_include_directories(vehicle_lib PUBLIC src)

add_executable(run_tests
  tests/test_vehicle.cpp
)
target_link_libraries(run_tests
  vehicle_lib
  GTest::GTest
  GTest::Main
)

include(GoogleTest)
gtest_add_tests(TARGET run_tests)
