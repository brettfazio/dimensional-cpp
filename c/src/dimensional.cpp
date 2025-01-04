#include <map>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <cctype>
#include <algorithm>
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang-tidy/ClangTidyCheck.h"
#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/ClangTidyModuleRegistry.h"

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

    UnitNode(const std::string &val) : value(val), left(nullptr), right(nullptr) {}
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

std::unique_ptr<UnitNode> parseExpression(std::vector<UnitToken>::iterator &it) {
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

// Parse and evaluate a unit annotation string.
Unit parseAnnotation(const std::string &annotation) {
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

class DimensionalAnalysisCheck : public clang::tidy::ClangTidyCheck {
public:
    DimensionalAnalysisCheck(StringRef Name, clang::tidy::ClangTidyContext *Context)
        : ClangTidyCheck(Name, Context) {}

    void registerMatchers(MatchFinder *Finder) override {
        Finder->addMatcher(varDecl(hasAttr(attr::Annotate)).bind("annotatedVar"), this);
    }

    void check(const MatchFinder::MatchResult &Result) override {
        const auto *VD = Result.Nodes.getNodeAs<VarDecl>("annotatedVar");
        if (!VD)
            return;

        if (const auto *annotateAttr = VD->getAttr<AnnotateAttr>()) {
            try {
                // Convert llvm::StringRef to std::string
                Unit varUnit = parseAnnotation(annotateAttr->getAnnotation().str());
                // You can add more checks related to dimensional analysis here
                diag(VD->getLocation(), "Variable %0 has a valid unit annotation")
                    << VD->getName();
            } catch (const std::runtime_error &e) {
                diag(VD->getLocation(), "Invalid unit annotation: %0") << e.what();
            }
        }
    }
};

// Register the Clang-Tidy module and check.
class DimensionalAnalysisModule : public clang::tidy::ClangTidyModule {
public:
    void addCheckFactories(clang::tidy::ClangTidyCheckFactories &Factories) override {
        Factories.registerCheck<DimensionalAnalysisCheck>("custom-dim-analys-check");
    }
};

// Register the module using this statically initialized variable.
static clang::tidy::ClangTidyModuleRegistry::Add<DimensionalAnalysisModule>
    X("dimensional-analysis-module", "Adds dimensional analysis checks.");