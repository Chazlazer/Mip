cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Use C++17 as per your original config
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find Python3 interpreter and development libraries (headers + libs)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Include project directories
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/inc
    ${Python3_INCLUDE_DIRS}
)

# Gather all .cpp files from src
file(GLOB_RECURSE SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# Add your main.cpp plus the sources found
add_executable(${PROJECT_NAME}
    main.cpp
    ${SRC_FILES}
)

# Link robotcontrol and Python libraries
target_link_libraries(${PROJECT_NAME}
    robotcontrol
    ${Python3_LIBRARIES}
)
