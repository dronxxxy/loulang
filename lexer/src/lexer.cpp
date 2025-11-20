#include <helpers/optional.hpp>
#include <algorithm>
#include <array>
#include <functional>
#include <lexer/lexer.hpp>
#include <map>
#include <string>
#include <string_view>

using namespace lexer;

Lexer::Lexer(
  std::shared_ptr<logger::Logger> logger,
  std::string_view::iterator begin,
  std::string_view::iterator end
) :
  logger(std::move(logger)),
  position(std::move(begin)),
  end(std::move(end)),
  skippedTokens() {}

bool Lexer::nextCharIsFilter(std::function<bool (char)> filter) {
  if (position == end) {
    return false;
  }
  if (filter(*position)) {
    position++;
    return true;
  }
  return false;
}

bool Lexer::nextCharIsNotFilter(std::function<bool (char)> filter) {
  if (position == end) {
    return true;
  }
  if (!filter(*position)) {
    position++;
    return true;
  }
  return false;
}

bool Lexer::nextCharIs(char c) {
  return this->nextCharIsFilter([&](char other) {
    return c == other;
  });
}

bool Lexer::nextCharIsNot(char c) {
  return this->nextCharIsFilter([&](char other) {
    return c == other;
  });
}

std::optional<char> Lexer::getNextChar() {
  if (position == end) {
    return std::nullopt;
  }
  return *(position++);
}

void Lexer::skipCharsFilter(std::function<bool (char)> filter) {
  while (this->nextCharIsFilter(filter)) {}
}

void Lexer::skipWhitespaces() {
  this->skipCharsFilter([] (char c) {
    auto whitespaces = std::array { '\n', '\t', ' ', '\r' };
    return std::find(whitespaces.begin(), whitespaces.end(), c) != whitespaces.end();
  });
}

void Lexer::skipComment() {
  if (this->nextCharIs('#')) {
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

Token Lexer::next() {
  if (!skippedTokens.empty()) {
    Token token = skippedTokens.front();
    skippedTokens.pop();
    return token;
  }

  this->skipNonToken();
  std::string_view::iterator prev = this->position;
  auto optC = this->getNextChar();
  if (!optC) {
    return Token(std::string_view(prev, prev), SimpleTokenData::EndOfInput);
  }
  auto getSlice = [&]() { return std::string_view(prev, this->position); };
  auto makeToken = [&](TokenData data) { return Token(getSlice(), data); };
  switch (auto c = optC.value()) {
    case '(': return makeToken(SimpleTokenData::OpeningCircleBrace);
    case ')': return makeToken(SimpleTokenData::ClosingCircleBrace);
    case '[': return makeToken(SimpleTokenData::OpeningSquareBrace);
    case ']': return makeToken(SimpleTokenData::ClosingSquareBrace);
    case '{': return makeToken(SimpleTokenData::OpeningFigureBrace);
    case '}': return makeToken(SimpleTokenData::ClosingFigureBrace);
    case ':': return makeToken(SimpleTokenData::Colon);
    case ',': return makeToken(SimpleTokenData::Comma);
    case '<':
      if (this->nextCharIs('=')) return makeToken(SimpleTokenData::LessOrEquals);
      return makeToken(SimpleTokenData::LessThan);
    case '>':
      if (this->nextCharIs('=')) return makeToken(SimpleTokenData::GreaterOrEquals);
      return makeToken(SimpleTokenData::GreaterThan);
    case '=':
      if (this->nextCharIs('=')) return makeToken(SimpleTokenData::Equals);
      return makeToken(SimpleTokenData::Assign);
    default: {
      if (charIsIdentStart(c)) {
        skipCharsFilter(charIsIdentContinue);
        std::map<std::string_view, SimpleTokenData> keywords = {
          { "meta", SimpleTokenData::Meta },
          { "const", SimpleTokenData::Const },
          { "final", SimpleTokenData::Final },
          { "var", SimpleTokenData::Var },
          { "public", SimpleTokenData::Public },
          { "extern", SimpleTokenData::Extern },
          { "fun", SimpleTokenData::Fun },
        };
        std::string_view slice = getSlice();
        if (keywords.contains(slice)) {
          return Token(slice, keywords[slice]);
        }
        return Token(slice, IdentTokenData(slice));
      }
      if (charIsDigit(c)) {
        skipCharsFilter(charIsDigit);
        return Token(getSlice(), IntTokenData(std::stoll(std::string(getSlice())))); // :P
      }
      logger->error("unknown token {}", getSlice());
    }
  }
  return this->next();
}

bool Lexer::nextIs(TokenDataFilter filter) {
  auto next = this->next();
  if (filter(next.getData())) {
    return true;
  }
  return false;
}

bool Lexer::skipTo(TokenDataFilter filter) {
  auto token = this->next();
  while (!token.isEoi() && filter(token.getData())) {
    token = this->next();
  }
  skippedTokens.emplace(token);
  return !token.isEoi();
  // TODO: brace level
}

bool Lexer::expect(TokenDataFilter filter, TokenDataFilter fallback) {
  if (!this->nextIs(filter)) {
    this->skipTo(fallback); 
    return false;
  }
  return true;
}

Token Lexer::peek() {
  auto token = this->next();
  skippedTokens.emplace(token);
  return token;
}

std::vector<Token> Lexer::peekN(size_t n) {
  std::vector<Token> result;
  result.reserve(n);
  for (size_t i = 0; i < n; i++) {
    auto token = this->next();
    result.push_back(token);
    skippedTokens.emplace(token);
  }
  return result;
}

void Lexer::nextN(size_t n) {
  for (size_t i = 0; i < n; i++) {
    this->next();
  }
}
