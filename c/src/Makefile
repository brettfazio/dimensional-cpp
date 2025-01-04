PLUGIN_NAME = DimensionalAnalysisCheck
LLVM_PATH = /opt/homebrew/Cellar/llvm/19.1.6
LLVM_LIB_PATH = $(LLVM_PATH)/lib
LLVM_INCLUDE_PATH = $(LLVM_PATH)/include

CXX = $(LLVM_PATH)/bin/clang++
CXXFLAGS = -fPIC -shared -std=c++17 -stdlib=libc++

PLUGIN_SRC = dimensional.cpp
PLUGIN_SO = lib$(PLUGIN_NAME).so

all: $(PLUGIN_SO)

$(PLUGIN_SO): $(PLUGIN_SRC)
	$(CXX) $(CXXFLAGS) $(PLUGIN_SRC) -o $(PLUGIN_SO) \
	-I$(LLVM_INCLUDE_PATH) \
	-L$(LLVM_LIB_PATH) \
	-Wl,-rpath,$(LLVM_LIB_PATH) \
	-lclangTidy \
	-lLLVM \
	-lclang-cpp \
	-lclang

clean:
	rm -f $(PLUGIN_SO)

.PHONY: all clean