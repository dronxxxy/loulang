#include <algorithm>
#include <array>
#include <functional>
#include <lexer/lexer.hpp>
#include <string>

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

bool Lexer::nextIsNotFilter(std::function<bool (char)> filter) {
  if (position == end) {
    return true;
  }
  if (!filter(*position)) {
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

bool Lexer::nextIsNot(char c) {
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

void Lexer::skipComment() {
  if (this->nextIs('#')) {
    std::optional<char> c;
    do {
      c = this->getNextChar();
    } while (c != std::nullopt && c != '\n');
  }
}

void Lexer::skipNonToken() {
  std::string_view::iterator start;
  do {
    start = this->position;
    this->skipWhitespaces();
    this->skipComment();
  } while (start != this->position);
}

static inline bool charIsDigit(char c) {
  return c >= '0' && c <= '9';
}

static inline bool charIsIdentStart(char c) {
  return 
    c == '_' ||
    c >= 'a' && c <= 'z' ||
    c >= 'A' && c <= 'Z';
}

static inline bool charIsIdentContinue(char c) {
  return charIsIdentStart(c) || charIsDigit(c);
}

std::optional<Token> Lexer::next() {
  this->skipNonToken();
  std::string_view::iterator prev = this->position;
  auto optC = this->getNextChar();
  if (!optC.has_value()) {
    return std::nullopt;
  }
  auto getSlice = [&]() { return std::string_view(prev, this->position); };
  auto makeToken = [&](TokenData data) { return Token(getSlice(), data); };
  switch (char c = optC.value()) {
    case '(': return makeToken(SimpleTokenData::OpeningCircleBrace);
    case ')': return makeToken(SimpleTokenData::ClosingCircleBrace);
    case '[': return makeToken(SimpleTokenData::OpeningSquareBrace);
    case ']': return makeToken(SimpleTokenData::ClosingSquareBrace);
    case '{': return makeToken(SimpleTokenData::OpeningFigureBrace);
    case '}': return makeToken(SimpleTokenData::ClosingFigureBrace);
    case '<':
      if (this->nextIs('=')) return makeToken(SimpleTokenData::LessOrEquals);
      return makeToken(SimpleTokenData::LessThan);
    case '>':
      if (this->nextIs('=')) return makeToken(SimpleTokenData::GreaterOrEquals);
      return makeToken(SimpleTokenData::GreaterThan);
    case '=':
      if (this->nextIs('=')) return makeToken(SimpleTokenData::Equals);
      return makeToken(SimpleTokenData::Assign);
    default: {
      if (charIsIdentStart(c)) {
        skipFilter(charIsIdentContinue);
        return Token(getSlice(), IdentTokenData(getSlice()));
      }
      if (charIsDigit(c)) {
        skipFilter(charIsDigit);
        return Token(getSlice(), IntTokenData(std::stoll(std::string(getSlice())))); // :P
      }
      logger->error("unknown token {}", getSlice());
    }
  }
  return this->next();
}
