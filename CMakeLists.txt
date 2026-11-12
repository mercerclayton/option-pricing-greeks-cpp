cmake_minimum_required(VERSION 3.14)
project(OptionPricingGreeks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Boost is used header-only (Boost.Math for the normal distribution)
find_package(Boost 1.70 REQUIRED)

add_executable(option_pricer
    src/main.cpp
    src/Option.cpp
    src/EuropeanOption.cpp
    src/PerpetualAmericanOption.cpp
    src/Mesh.cpp
)

target_include_directories(option_pricer PRIVATE include)
target_link_libraries(option_pricer PRIVATE Boost::boost)

if(MSVC)
    target_compile_options(option_pricer PRIVATE /W4)
else()
    target_compile_options(option_pricer PRIVATE -Wall -Wextra -Wpedantic)
endif()
