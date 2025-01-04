PLUGIN_NAME = DimensionalAnalysisCheck

LLVM_CONFIG = llvm-config
CLANG_TIDY = clang-tidy

LLVM_LIB_PATH = /opt/homebrew/Cellar/llvm/19.1.6/lib

CXX = $(shell $(LLVM_CONFIG) --bindir)/clang++
LLVM_CXXFLAGS = $(shell $(LLVM_CONFIG) --cxxflags)
LLVM_LDFLAGS = $(shell $(LLVM_CONFIG) --ldflags)
LLVM_LIBS = $(shell $(LLVM_CONFIG) --libs)

PLUGIN_SRC = dimensional.cpp
PLUGIN_SO = lib$(PLUGIN_NAME).so

# Test files
TEST_SRC = test.cpp
TEST_OUT = test.out

all: $(PLUGIN_SO)

# Build the plugin as a shared library
$(PLUGIN_SO): $(PLUGIN_SRC)
	$(CXX) -fPIC -shared $(PLUGIN_SRC) -o $(PLUGIN_SO) $(LLVM_CXXFLAGS) $(LLVM_LDFLAGS) $(LLVM_LIBS) -L$(LLVM_LIB_PATH) -lclangTidy -lclangASTMatchers -lclangAST -lclangFrontend -lclangBasic -lclangTooling -lclangSerialization -lclangLex

# Run Clang-Tidy with the plugin
test: $(PLUGIN_SO) $(TEST_SRC)
	$(CLANG_TIDY) $(TEST_SRC) --load=$(PLUGIN_SO) --checks=-*,custom-dim-analys-check > $(TEST_OUT)
	cat $(TEST_OUT)

clean:
	rm -f $(PLUGIN_SO) $(TEST_OUT)
