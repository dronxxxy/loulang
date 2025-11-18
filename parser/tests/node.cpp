#include <parser/ast/path.hpp>
#include <parser/ast/types/path.hpp>
#include <catch2/catch_test_macros.hpp>
#include <parser/ast/nodes/decl.hpp>

TEST_CASE("nodes equals", "[Parser]") {
  std::shared_ptr<parser::AstNode> n1 = std::make_shared<parser::AstDeclNode>(
    parser::AstDeclKind::Meta,
    std::nullopt,
    std::nullopt,
    std::make_shared<parser::AstPathType>(parser::AstPath({
      std::string_view("hello"), std::string_view("world")
    }))
  );
  std::shared_ptr<parser::AstNode> n2 = std::make_shared<parser::AstDeclNode>(parser::AstDeclKind::Final, std::nullopt, std::nullopt, std::nullopt);
  REQUIRE(*n1 == *n1);
  REQUIRE(*n1 != *n2);
}

