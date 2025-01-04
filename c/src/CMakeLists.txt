cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set project name
project(DimensionalAnalysisCheck)

# Set LLVM paths
set(LLVM_PATH /opt/homebrew/Cellar/llvm/19.1.6)
set(LLVM_LIB_PATH ${LLVM_PATH}/lib)
set(LLVM_INCLUDE_PATH ${LLVM_PATH}/include)

# Define plugin and source files
set(PLUGIN_NAME DimensionalAnalysisCheck)
set(PLUGIN_SRC todo.cpp)
set(PLUGIN_SO lib${PLUGIN_NAME}.so)

# Define Clang Tidy and Clang++ binary paths
set(CLANG_TIDY clang-tidy)
set(CXX ${LLVM_PATH}/bin/clang++)

# Set C++ standard and flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fPIC -stdlib=libc++")

# Include LLVM and Clang directories
include_directories(${LLVM_INCLUDE_PATH})
link_directories(${LLVM_LIB_PATH})

# Add the plugin target
add_library(${PLUGIN_NAME} SHARED ${PLUGIN_SRC})
target_link_libraries(${PLUGIN_NAME} clangTidy LLVM clang-cpp clang)

# Test configuration
set(TEST_SRC test.cpp)
set(TEST_OUT test.out)

# Add a custom command to run Clang-Tidy with the plugin
add_custom_target(test
    COMMAND ${CLANG_TIDY} ${TEST_SRC} --load=${CMAKE_BINARY_DIR}/${PLUGIN_SO} --checks=-*,custom-dim-analys-check
    COMMAND cat ${TEST_OUT}
    DEPENDS ${PLUGIN_NAME} ${TEST_SRC}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Make sure the clean target is run after the build
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_BINARY_DIR}/${PLUGIN_SO})
