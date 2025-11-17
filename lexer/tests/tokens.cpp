#include "logger/mock.hpp"
#include <catch2/catch_test_macros.hpp>
#include <lexer/lexer.hpp>
#include <memory>

using namespace lexer;
using namespace logger;

namespace Catch {
  template <>
  struct StringMaker<Token> {
    static auto convert(const Token &value) -> std::string {
      std::ostringstream oss;
      oss << value.toString();
      return std::move(oss).str();
    }
  };
};

void lexerTest(const std::string_view content, std::vector<lexer::Token> tokens) {
  auto logger = std::make_shared<MockLogger>();
  Lexer lexer = Lexer(logger, content.begin(), content.end());
  for (auto token : tokens) {
    auto lexed = lexer.next();
    REQUIRE(lexed.has_value());
    REQUIRE(token == lexed.value());
  }
  REQUIRE(!lexer.next().has_value());
}

void lexerTestSimple(const std::string_view content, lexer::SimpleTokenData data) {
  lexerTest(content, { Token(content, data) });
}

TEST_CASE("single simple tokens should be lexed", "[Lexer]") {
  lexerTestSimple("(", SimpleTokenData::OpeningCircleBrace);
  lexerTestSimple(")", SimpleTokenData::ClosingCircleBrace);
  lexerTestSimple("[", SimpleTokenData::OpeningSquareBrace);
  lexerTestSimple("]", SimpleTokenData::ClosingSquareBrace);
  lexerTestSimple("{", SimpleTokenData::OpeningFigureBrace);
  lexerTestSimple("}", SimpleTokenData::ClosingFigureBrace);
  lexerTestSimple("=", SimpleTokenData::Assign);
  lexerTestSimple("<", SimpleTokenData::LessThan);
  lexerTestSimple(">", SimpleTokenData::GreaterThan);
  lexerTestSimple("<=", SimpleTokenData::LessOrEquals);
  lexerTestSimple(">=", SimpleTokenData::GreaterOrEquals);
  lexerTestSimple("==", SimpleTokenData::Equals);
}

TEST_CASE("spaces should be removed", "[Lexer]") {
  lexerTest("  \n\r( \n\r", { Token("(", SimpleTokenData::OpeningCircleBrace) });
}

TEST_CASE("ident token should be lexed", "[Lexer]") {
  lexerTest("a", { Token("a", IdentTokenData(std::string_view("a"))) });
  lexerTest("multipleLetters", { Token("multipleLetters", IdentTokenData(std::string_view("multipleLetters"))) });
  lexerTest("_withUndescore", { Token("_withUndescore", IdentTokenData(std::string_view("_withUndescore"))) });
  lexerTest("_withNumbers123", { Token("_withNumbers123", IdentTokenData(std::string_view("_withNumbers123"))) });
}

TEST_CASE("integer token should be lexed", "[Lexer]") {
  lexerTest("123", { Token("123", IntTokenData(123)) });
  lexerTest("12", { Token("12", IntTokenData(12)) });
}

