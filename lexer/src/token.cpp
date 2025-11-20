#include <lexer/token.hpp>
#include <string>
#include <utility>
#include <variant>
#include <magic_enum/magic_enum.hpp>

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
    [](SimpleTokenData data) { return std::string(magic_enum::enum_name(data)); },
    [](IntTokenData data) { return std::to_string(data.getValue()); },
    [](IdentTokenData data) { return std::string(data.getValue()); },
  }, this->data);
}

const SimpleTokenData *Token::isSimple() const {
  return std::get_if<SimpleTokenData>(&this->data);
}

const IdentTokenData *Token::isIdent() const {
  return std::get_if<IdentTokenData>(&this->data);
}

bool Token::isEqualsSimple(SimpleTokenData target) const {
  if (auto simple = this->isSimple()) {
    return *simple == target;
  }
  return false;
}

bool Token::isEoi() const {
  if (auto simple = this->isSimple()) {
    return *simple == SimpleTokenData::EndOfInput;
  }
  return false;
}

const TokenData& Token::getData() const {
  return this->data;
}
