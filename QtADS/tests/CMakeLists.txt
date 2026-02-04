
# ------------------------------------------------------------------------------
# Testing Configuration
# ------------------------------------------------------------------------------

# Enable CTest integration
enable_testing()

# Output test binaries to a separate directory so they don't end up
# alongside the application binary or get picked up by CPack
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")

# 1. GoogleTest Suite
# -------------------
add_executable(UnitTests_GTest tst_gtest_main.cpp)
target_link_libraries(UnitTests_GTest PRIVATE
    GTest::gtest
    Qt6::Core
)

# Discover tests specifically for this target
include(GoogleTest)
gtest_discover_tests(UnitTests_GTest)

# 2. Qt Test Suite
# ----------------
add_executable(UnitTests_QtTest tst_qtest_main.cpp)
target_link_libraries(UnitTests_QtTest PRIVATE
    Qt6::Core
    Qt6::Test
)

# Register with CTest
add_test(NAME QtTestCommand COMMAND UnitTests_QtTest)
