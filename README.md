# dimensional-cpp

Clang tidy plugin that provides dimensional analysis for C++. Annotate a variable with a unit like `[[clang::annotate("unit:miles")]]` and the plugin will validate the units during operations.

## make

```bash
cd c/src
cmake .
make
```

## run

```bash
cd c/src
clang-tidy -p=./compile_commands.json --load=./libDimensionalAnalysisCheck.dylib --config-file=./.clang-tidy test.cpp
```

## tests
```bash
make run_tests
```
