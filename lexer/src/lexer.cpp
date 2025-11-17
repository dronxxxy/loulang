#include <algorithm>
#include <array>
#include <functional>
#include <lexer/lexer.hpp>

using namespace lexer;

Lexer::Lexer(
  std::shared_ptr<logger::Logger> logger,
  std::string_view::iterator begin,
  std::string_view::iterator end
) :
  logger(std::move(logger)),
  position(std::move(begin)),
  end(std::move(end)) {}

bool Lexer::nextIsFilter(std::function<bool (char)> filter) {
  if (position == end) {
    return false;
  }
  if (filter(*position)) {
    position++;
    return true;
  }
  return false;
}

bool Lexer::nextIs(char c) {
  return this->nextIsFilter([&](char other) {
    return c == other;
  });
}

std::optional<char> Lexer::getNextChar() {
  if (position == end) {
    return std::nullopt;
  }
  return *(position++);
}

void Lexer::skipFilter(std::function<bool (char)> filter) {
  while (this->nextIsFilter(filter)) {}
}

void Lexer::skipWhitespaces() {
  this->skipFilter([] (char c) {
    auto whitespaces = std::array { '\n', '\t', ' ', '\r' };
    return std::find(whitespaces.begin(), whitespaces.end(), c) != whitespaces.end();
  });
}

std::optional<Token> Lexer::next() {
  this->skipWhitespaces();
  std::string_view::iterator prev = this->position;
  auto optC = this->getNextChar();
  if (!optC.has_value()) {
    return std::nullopt;
  }
  auto make_token = [&](TokenData data) {
    return Token(std::string_view(prev, this->position), data);
  };
  switch (optC.value()) {
    case '(': return make_token(SimpleTokenData::OpeningCircleBrace);
    case ')': return make_token(SimpleTokenData::ClosingCircleBrace);
    case '[': return make_token(SimpleTokenData::OpeningSquareBrace);
    case ']': return make_token(SimpleTokenData::ClosingSquareBrace);
    case '{': return make_token(SimpleTokenData::OpeningFigureBrace);
    case '}': return make_token(SimpleTokenData::ClosingFigureBrace);
    case '<':
      if (this->nextIs('=')) return make_token(SimpleTokenData::LessOrEquals);
      return make_token(SimpleTokenData::LessThan);
    case '>':
      if (this->nextIs('=')) return make_token(SimpleTokenData::GreaterOrEquals);
      return make_token(SimpleTokenData::GreaterThan);
    case '=':
      if (this->nextIs('=')) return make_token(SimpleTokenData::Equals);
      return make_token(SimpleTokenData::Assign);
    default:
      break;
  }
  return this->next();
}
