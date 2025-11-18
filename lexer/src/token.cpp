#include <lexer/token.hpp>
#include <string>
#include <utility>

using namespace lexer;

Token::Token(std::string_view slice, TokenData data) :
  data(std::move(data)), slice(std::move(slice)) {}

bool Token::operator==(const Token &other) const {
  return this->slice == other.slice &&
    this->data == other.data;
}

template<class... Ts> struct overloads : Ts... { using Ts::operator()...; };

std::string Token::toString() const {
  return std::visit(overloads {
    [](SimpleTokenData data) {
      switch (data) {
        case SimpleTokenData::OpeningCircleBrace: return std::string("OpeningCircleBrace");
        case SimpleTokenData::ClosingCircleBrace: return std::string("ClosingCircleBrace");
        case SimpleTokenData::OpeningFigureBrace: return std::string("OpeningFigureBrace");
        case SimpleTokenData::ClosingFigureBrace: return std::string("ClosingFigureBrace");
        case SimpleTokenData::OpeningSquareBrace: return std::string("OpeningSquareBrace");
        case SimpleTokenData::ClosingSquareBrace: return std::string("ClosingSquareBrace");
        case SimpleTokenData::LessThan: return std::string("LessThan");
        case SimpleTokenData::GreaterThan: return std::string("GreaterThan");
        case SimpleTokenData::LessOrEquals: return std::string("LessOrEquals");
        case SimpleTokenData::GreaterOrEquals: return std::string("GreaterOrEquals");
        case SimpleTokenData::Assign: return std::string("Assign");
        case SimpleTokenData::Equals: return std::string("Equals");
        case SimpleTokenData::Meta: return std::string("Meta");
        case SimpleTokenData::Const: return std::string("Const");
        case SimpleTokenData::Final: return std::string("Final");
        case SimpleTokenData::Var: return std::string("Var");
      }
      std::unreachable();
    },
    [](IntTokenData data) { return std::to_string(data.getValue()); },
    [](IdentTokenData data) { return std::string(data.getValue()); },
  }, this->data);
}

const TokenData& Token::getData() const {
  return this->data;
}
