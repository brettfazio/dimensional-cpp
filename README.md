# dimensional-cpp

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
