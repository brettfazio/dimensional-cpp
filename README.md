# dimensional-cpp

Example check working with

brettfazio@Bretts-Air src % clang-tidy -p=./compile_commands.json --load=./libDimensionalAnalysisCheck.dylib --config-file=./.clang-tidy test.cpp
1 warning generated.
/Users/brettfazio/Documents/GitHub/dimensional-cpp/c/src/test.cpp:10:5: error: Avoid using printf in the code [example-check,-warnings-as-errors]
   10 |     printf("bar");
      |     ^
1 warning treated as error