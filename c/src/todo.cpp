// MyCheck.cpp
#include <clang/AST/AST.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Tooling/Tooling.h>
#include "clang-tidy/ClangTidyCheck.h"
#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/ClangTidyModuleRegistry.h"
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tidy;

namespace clang {
namespace tidy {

// Define the check
class MyCheck : public ClangTidyCheck {
public:
  MyCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(MatchFinder *Finder) override {
    // Matcher to find all printf calls in the code
    Finder->addMatcher(
        callExpr(callee(functionDecl(hasName("printf")))).bind("printfCall"),
        this);
  }

  void check(const MatchFinder::MatchResult &Result) override {
    // If a printf function call is found, emit a diagnostic
    const auto *Call = Result.Nodes.getNodeAs<CallExpr>("printfCall");
    if (Call) {
      diag(Call->getBeginLoc(), "Avoid using printf in the code");
    }
  }
};

// Define the module that registers the check
class MyModule : public ClangTidyModule {
public:
    void addCheckFactories(clang::tidy::ClangTidyCheckFactories &Factories) override {
        Factories.registerCheck<MyCheck>("example-check");
    }
};

// Register the module
static ClangTidyModuleRegistry::Add<MyModule> X("example-module", "My custom ClangTidy module");

volatile int awesomePrefixCheckAnchorSource = 0;

} // namespace tidy
} // namespace clang
