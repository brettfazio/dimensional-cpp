#include "clang-tidy/ClangTidyCheck.h"
#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/ClangTidyModuleRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendDiagnostic.h"
#include "clang/Tooling/Tooling.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

using namespace clang;
using namespace clang::ast_matchers;

// Represents a unit and its exponents (e.g., {km: 1, s: -1} for km/s).
class Unit {
  std::map<std::string, int> exponents;

public:
  void addUnit(const std::string &unit, int power) {
    exponents[unit] += power;
    if (exponents[unit] == 0) {
      exponents.erase(unit);
    }
  }

  Unit operator*(const Unit &other) const {
    Unit result = *this;
    for (const auto &entry : other.exponents) {
      result.addUnit(entry.first, entry.second);
    }
    return result;
  }

  Unit operator/(const Unit &other) const {
    Unit result = *this;
    for (const auto &entry : other.exponents) {
      result.addUnit(entry.first, -entry.second);
    }
    return result;
  }

  bool operator==(const Unit &other) const {
    return normalize() == other.normalize();
  }

  std::string normalize() const {
    std::ostringstream oss;
    for (const auto &entry : exponents) {
      if (entry.second > 0) {
        oss << entry.first << "^" << entry.second << "*";
      } else if (entry.second < 0) {
        oss << entry.first << "^(" << entry.second << ")*";
      }
    }
    std::string result = oss.str();
    if (!result.empty() && result.back() == '*') {
      result.pop_back();
    }
    return result.empty() ? "unitless" : result;
  }

  std::string toString() const { return normalize(); }
};

// Updated token definitions.
enum UnitTokenType { UNIT, MUL, DIV, LPAREN, RPAREN, EXP, END };

struct UnitToken {
  UnitTokenType type;
  std::string value;
};

// Tokenize the unit string.
std::vector<UnitToken> tokenize(const std::string &input) {
  std::vector<UnitToken> tokens;
  size_t i = 0;
  while (i < input.size()) {
    if (std::isspace(input[i])) {
      i++;
      continue;
    }
    if (std::isalpha(input[i])) {
      size_t start = i;
      while (i < input.size() && (std::isalnum(input[i]) || input[i] == '_')) {
        i++;
      }
      tokens.push_back({UNIT, input.substr(start, i - start)});
    } else if (input[i] == '*') {
      tokens.push_back({MUL, "*"});
      i++;
    } else if (input[i] == '/') {
      tokens.push_back({DIV, "/"});
      i++;
    } else if (input[i] == '(') {
      tokens.push_back({LPAREN, "("});
      i++;
    } else if (input[i] == ')') {
      tokens.push_back({RPAREN, ")"});
      i++;
    } else if (input[i] == '^') {
      tokens.push_back({EXP, "^"});
      i++;
    } else {
      throw std::runtime_error("Invalid character in unit string");
    }
  }
  tokens.push_back({END, ""});
  return tokens;
}

// Define the AST node for the unit expression.
struct UnitNode {
  std::string value;
  std::unique_ptr<UnitNode> left, right;

  UnitNode(const std::string &val)
      : value(val), left(nullptr), right(nullptr) {}
};

// Parsing functions for the unit expression.
std::unique_ptr<UnitNode> parseExpression(std::vector<UnitToken>::iterator &it);

std::unique_ptr<UnitNode> parsePrimary(std::vector<UnitToken>::iterator &it) {
  if (it->type == UNIT) {
    auto node = std::make_unique<UnitNode>(it->value);
    ++it;
    return node;
  } else if (it->type == LPAREN) {
    ++it;
    auto node = parseExpression(it);
    if (it->type != RPAREN) {
      throw std::runtime_error("Mismatched parentheses in unit string");
    }
    ++it;
    return node;
  } else {
    throw std::runtime_error("Invalid unit expression");
  }
}

std::unique_ptr<UnitNode> parseFactor(std::vector<UnitToken>::iterator &it) {
  auto node = parsePrimary(it);
  while (it->type == EXP) {
    ++it;
    auto exponentNode = parsePrimary(it);
    auto newNode = std::make_unique<UnitNode>("^");
    newNode->left = std::move(node);
    newNode->right = std::move(exponentNode);
    node = std::move(newNode);
  }
  return node;
}

std::unique_ptr<UnitNode> parseTerm(std::vector<UnitToken>::iterator &it) {
  auto node = parseFactor(it);
  while (it->type == MUL || it->type == DIV) {
    UnitTokenType op = it->type;
    ++it;
    auto rightNode = parseFactor(it);
    auto newNode = std::make_unique<UnitNode>(op == MUL ? "*" : "/");
    newNode->left = std::move(node);
    newNode->right = std::move(rightNode);
    node = std::move(newNode);
  }
  return node;
}

std::unique_ptr<UnitNode>
parseExpression(std::vector<UnitToken>::iterator &it) {
  return parseTerm(it);
}

// Evaluate the unit expression tree.
Unit evaluateUnitTree(const UnitNode *node) {
  if (!node->left && !node->right) {
    Unit unit;
    unit.addUnit(node->value, 1);
    return unit;
  }

  Unit leftUnit = evaluateUnitTree(node->left.get());
  Unit rightUnit = evaluateUnitTree(node->right.get());

  if (node->value == "*") {
    return leftUnit * rightUnit;
  } else if (node->value == "/") {
    return leftUnit / rightUnit;
  } else if (node->value == "^") {
    int exponent = std::stoi(node->right->value);
    Unit result;
    result.addUnit(node->left->value, exponent);
    return result;
  }

  throw std::runtime_error("Unknown operator in unit expression");
}

Unit parseAnnotation(const std::string &annotation) {
  // Handle explicit unit annotations
  if (annotation == "explicit_unit_annotation") {
    return Unit(); // Return empty unit to indicate explicit annotation
  }

  if (annotation.find("unit:") != 0) {
    throw std::runtime_error("Invalid unit annotation format");
  }
  std::string unitString = annotation.substr(5);
  auto tokens = tokenize(unitString);
  auto it = tokens.begin();
  auto unitTree = parseExpression(it);

  if (it->type != END) {
    throw std::runtime_error("Unexpected tokens in unit string");
  }

  return evaluateUnitTree(unitTree.get());
}

// Add helper function to get all annotations for a declaration
std::vector<std::string> getAllAnnotations(const Decl *D) {
  std::vector<std::string> annotations;
  for (const auto *attr : D->specific_attrs<AnnotateAttr>()) {
    annotations.push_back(attr->getAnnotation().str());
  }
  return annotations;
}

// Add helper function to check if a declaration has explicit unit annotation
bool hasExplicitUnitAnnotation(const Decl *D) {
  for (const auto *attr : D->specific_attrs<AnnotateAttr>()) {
    if (attr->getAnnotation().str() == "explicit_unit_annotation") {
      return true;
    }
  }
  return false;
}

// Add helper function to get unit from annotations
Unit getUnitFromAnnotations(const Decl *D) {
  Unit resultUnit;
  bool foundUnit = false;

  for (const auto *attr : D->specific_attrs<AnnotateAttr>()) {
    std::string annotation = attr->getAnnotation().str();
    if (annotation.find("unit:") == 0) {
      if (foundUnit) {
        // Multiple unit annotations found - this is an error
        throw std::runtime_error(
            "Multiple unit annotations found on declaration");
      }
      resultUnit = parseAnnotation(annotation);
      foundUnit = true;
    }
  }
  return resultUnit;
}

class DimensionalAnalysisCheck : public clang::tidy::ClangTidyCheck {
  std::map<const VarDecl *, Unit> varUnits;
  std::map<const FunctionDecl *, Unit> functionUnits;
  std::map<const ParmVarDecl *, Unit> paramUnits;

public:
  DimensionalAnalysisCheck(StringRef Name,
                           clang::tidy::ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(MatchFinder *Finder) override {
    // Match variable declarations with initializers
    Finder->addMatcher(varDecl(hasInitializer(expr().bind("init"))).bind("var"),
                       this);

    Finder->addMatcher(
        varDecl(hasInitializer(initListExpr().bind("initList"))).bind("var"),
        this);

    // Match function declarations with annotations
    Finder->addMatcher(
        functionDecl(hasAttr(attr::Annotate)).bind("annotatedFunc"), this);

    // Match function calls
    Finder->addMatcher(
        callExpr(callee(functionDecl().bind("callee"))).bind("call"), this);

    // Match return statements in annotated functions
    Finder->addMatcher(
        returnStmt(
            hasReturnValue(expr().bind("returnValue")),
            hasAncestor(
                functionDecl(hasAttr(attr::Annotate)).bind("containingFunc")))
            .bind("return"),
        this);

    // Match `if` conditions
    Finder->addMatcher(
        ifStmt(hasCondition(expr().bind("ifCondition"))).bind("ifStmt"), this);
  }

private:
  Unit inferExpressionUnit(const Expr *E,
                           const MatchFinder::MatchResult &Result) {
    if (!E)
      return Unit();

    E = E->IgnoreParenImpCasts();

    if (const auto *initList = dyn_cast<InitListExpr>(E)) {
      std::cout << "HERE!" << std::endl;
      const auto *varDecl =
          dyn_cast_or_null<VarDecl>(Result.Nodes.getNodeAs<VarDecl>("var"));
      if (!varDecl)
        return Unit();

      const auto *type = varDecl->getType().getTypePtrOrNull();
      if (const auto *recordType = type ? type->getAs<RecordType>() : nullptr) {
        const auto *structDecl = dyn_cast<CXXRecordDecl>(recordType->getDecl());
        if (structDecl) {
          auto fieldIt = structDecl->field_begin();
          for (unsigned i = 0; i < initList->getNumInits(); ++i, ++fieldIt) {
            if (fieldIt == structDecl->field_end())
              break;

            const auto *field = *fieldIt;
            const auto *fieldInit = initList->getInit(i);

            Unit initUnit = inferExpressionUnit(fieldInit, Result);
            Unit declaredUnit = getUnitFromAnnotations(field);

            if (!(declaredUnit == initUnit)) {
              diag(fieldInit->getExprLoc(),
                   "Unit mismatch: field '%0' declared with unit %1 but "
                   "initialized with unit %2")
                  << field->getNameAsString() << declaredUnit.toString()
                  << initUnit.toString();
            }
          }
        }
      }
      return Unit();
    }

    // Handle literal constants in explicit unit context
    if (isa<IntegerLiteral>(E) || isa<FloatingLiteral>(E)) {
      const auto &parents = Result.Context->getParents(*E);
      if (!parents.empty()) {
        const auto *parent = parents[0].get<Decl>();
        if (const auto *varDecl = dyn_cast_or_null<VarDecl>(parent)) {
          if (hasExplicitUnitAnnotation(varDecl)) {
            return getUnitFromAnnotations(varDecl);
          }
        }
        // Handle function returns
        else if (const auto *returnStmt = parents[0].get<ReturnStmt>()) {
          const auto &returnParents = Result.Context->getParents(*returnStmt);
          if (!returnParents.empty()) {
            if (const auto *funcDecl = returnParents[0].get<FunctionDecl>()) {
              if (hasExplicitUnitAnnotation(funcDecl)) {
                return getUnitFromAnnotations(funcDecl);
              }
            }
          }
        }
      }
    }

    // Handle member expressions
    if (const auto *memberExpr = dyn_cast<MemberExpr>(E)) {
      if (const auto *memberDecl = memberExpr->getMemberDecl()) {
        if (const auto *annotateAttr = memberDecl->getAttr<AnnotateAttr>()) {
          return parseAnnotation(annotateAttr->getAnnotation().str());
        }
      }
    }

    // Handle function calls
    if (const auto *call = dyn_cast<CallExpr>(E)) {
      if (const auto *callee = call->getDirectCallee()) {
        if (const auto *annotateAttr = callee->getAttr<AnnotateAttr>()) {
          return parseAnnotation(annotateAttr->getAnnotation().str());
        }
        auto it = functionUnits.find(callee);
        if (it != functionUnits.end()) {
          return it->second;
        }
      }
    }
    // Handle parameter references and variables
    else if (const auto *declRef = dyn_cast<DeclRefExpr>(E)) {
      if (const auto *parmDecl = dyn_cast<ParmVarDecl>(declRef->getDecl())) {
        if (const auto *annotateAttr = parmDecl->getAttr<AnnotateAttr>()) {
          return parseAnnotation(annotateAttr->getAnnotation().str());
        }
        auto it = paramUnits.find(parmDecl);
        if (it != paramUnits.end()) {
          return it->second;
        }
      } else if (const auto *varDecl = dyn_cast<VarDecl>(declRef->getDecl())) {
        // First check for direct unit annotation
        if (const auto *annotateAttr = varDecl->getAttr<AnnotateAttr>()) {
          return parseAnnotation(annotateAttr->getAnnotation().str());
        }
        // Then check our stored units
        auto it = varUnits.find(varDecl);
        if (it != varUnits.end()) {
          return it->second;
        }
      }
    }

    // Handle binary operators
    if (const auto *binOp = dyn_cast<BinaryOperator>(E)) {
      Unit leftUnit = inferExpressionUnit(binOp->getLHS(), Result);
      Unit rightUnit = inferExpressionUnit(binOp->getRHS(), Result);

      switch (binOp->getOpcode()) {
      case BO_Add:
      case BO_Sub:
        if (!(leftUnit == rightUnit)) {
          diag(binOp->getOperatorLoc(),
               "Mismatched units in addition/subtraction");
        }
        return leftUnit;
      case BO_Mul:
        return leftUnit * rightUnit;
      case BO_Div:
        return leftUnit / rightUnit;
      case BO_LT:
      case BO_GT:
      case BO_LE:
      case BO_GE:
      case BO_EQ:
      case BO_NE:
        if (!(leftUnit == rightUnit)) {
          diag(binOp->getOperatorLoc(), "Comparison of mismatched units");
        }
        return Unit();
      default:
        return Unit();
      }
    }

    return Unit();
  }

  void checkFunctionBody(const Stmt *S, const FunctionDecl *FD,
                         const MatchFinder::MatchResult &Result) {
    if (!S)
      return;

    if (const auto *returnStmt = dyn_cast<ReturnStmt>(S)) {
      if (const Expr *returnValue = returnStmt->getRetValue()) {
        Unit returnUnit = inferExpressionUnit(returnValue, Result);
        auto it = functionUnits.find(FD);
        if (it != functionUnits.end() && !(it->second == returnUnit)) {
          diag(returnValue->getExprLoc(),
               "Return value unit mismatch: function expects %0 but returning "
               "%1")
              << it->second.toString() << returnUnit.toString();
        }
      }
    }

    for (const Stmt *child : S->children()) {
      if (child) {
        checkFunctionBody(child, FD, Result);
      }
    }
  }

  void storeFunctionParameterUnits(const FunctionDecl *FD) {
    for (const ParmVarDecl *param : FD->parameters()) {
      if (const auto *annotateAttr = param->getAttr<AnnotateAttr>()) {
        paramUnits[param] =
            parseAnnotation(annotateAttr->getAnnotation().str());
      }
    }
  }

public:
  void check(const MatchFinder::MatchResult &Result) override {
    if (const auto *VD = Result.Nodes.getNodeAs<VarDecl>("var")) {
      if (const auto *Init = Result.Nodes.getNodeAs<Expr>("init")) {
        // First, infer the unit of the initializer
        Unit initUnit = inferExpressionUnit(Init, Result);

        // Get any unit annotations on the variable
        bool hasAnnotation = VD->hasAttr<AnnotateAttr>();
        bool isExplicit = hasExplicitUnitAnnotation(VD);
        Unit declaredUnit;

        if (hasAnnotation) {
          try {
            declaredUnit = getUnitFromAnnotations(VD);
            varUnits[VD] = declaredUnit;
          } catch (const std::runtime_error &e) {
            diag(VD->getLocation(), "Unit annotation error: %0") << e.what();
            return;
          }
        }

        // Special case: explicit unit annotation with literal initialization
        if (isExplicit &&
            (isa<IntegerLiteral>(Init) || isa<FloatingLiteral>(Init))) {
          return;
        }

        // Now perform the checks:

        // Case 1: Variable has no unit annotation but initializer has units
        if (!hasAnnotation && initUnit.toString() != "unitless") {
          diag(VD->getLocation(),
               "Unit safety violation: cannot drop units - assigning value "
               "with unit %0 to variable without unit annotation")
              << initUnit.toString();
          return;
        }

        // Case 2: Variable has unit annotation but units don't match
        if (hasAnnotation && !(declaredUnit == initUnit)) {
          // Allow explicit unit annotation to override unit mismatch
          if (!isExplicit) {
            diag(VD->getLocation(),
                 "Unit mismatch: variable declared with unit %0 but "
                 "initialized with expression of unit %1")
                << declaredUnit.toString() << initUnit.toString();
            return;
          }
        }

        // Store the unit information
        varUnits[VD] = hasAnnotation ? declaredUnit : initUnit;
      }
    }

    if (const auto *ifStmt = Result.Nodes.getNodeAs<IfStmt>("ifStmt")) {
      if (const auto *cond = Result.Nodes.getNodeAs<Expr>("ifCondition")) {
        inferExpressionUnit(
            cond, Result); // This will trigger diagnostics for mismatched units
      }
    }

    if (const auto *call = Result.Nodes.getNodeAs<CallExpr>("call")) {
      if (const auto *callee = Result.Nodes.getNodeAs<FunctionDecl>("callee")) {
        unsigned numParams = callee->getNumParams();
        unsigned numArgs = call->getNumArgs();

        for (unsigned i = 0; i < numParams && i < numArgs; ++i) {
          const ParmVarDecl *param = callee->getParamDecl(i);
          const Expr *arg = call->getArg(i);

          if (const auto *paramAnnot = param->getAttr<AnnotateAttr>()) {
            Unit expectedUnit =
                parseAnnotation(paramAnnot->getAnnotation().str());
            Unit argUnit = inferExpressionUnit(arg, Result);

            if (!(expectedUnit == argUnit)) {
              diag(arg->getExprLoc(), "Argument unit mismatch: parameter "
                                      "expects unit %0 but provided unit %1")
                  << expectedUnit.toString() << argUnit.toString();
            }
          }
        }
      }
    }

    if (const auto *FD =
            Result.Nodes.getNodeAs<FunctionDecl>("annotatedFunc")) {
      if (const auto *annotateAttr = FD->getAttr<AnnotateAttr>()) {
        Unit functionUnit =
            parseAnnotation(annotateAttr->getAnnotation().str());
        functionUnits[FD] = functionUnit;
        storeFunctionParameterUnits(FD);

        if (FD->hasBody()) {
          checkFunctionBody(FD->getBody(), FD, Result);
        }
      }
    }

    if (const auto *returnStmt = Result.Nodes.getNodeAs<ReturnStmt>("return")) {
      if (const auto *containingFunc =
              Result.Nodes.getNodeAs<FunctionDecl>("containingFunc")) {
        if (const Expr *returnValue = returnStmt->getRetValue()) {
          Unit returnUnit = inferExpressionUnit(returnValue, Result);
          auto it = functionUnits.find(containingFunc);
          if (it != functionUnits.end() && !(it->second == returnUnit)) {
            diag(returnValue->getExprLoc(),
                 "Return value unit mismatch: function expects %0 but "
                 "returning %1")
                << it->second.toString() << returnUnit.toString();
          }
        }
      }
    }
  }
};

// Register the check
class DimensionalAnalysisModule : public clang::tidy::ClangTidyModule {
public:
  void
  addCheckFactories(clang::tidy::ClangTidyCheckFactories &Factories) override {
    Factories.registerCheck<DimensionalAnalysisCheck>(
        "dimensional-analysis-check");
  }
};

static clang::tidy::ClangTidyModuleRegistry::Add<DimensionalAnalysisModule>
    X("dimensional-analysis-module", "Adds dimensional analysis checks.");