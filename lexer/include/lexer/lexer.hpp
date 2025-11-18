#pragma once

#include <functional>
#include <optional>
#include <memory>
#include <logger/logger.hpp>
#include <lexer/token.hpp>
#include <queue>

namespace lexer {
  typedef std::function<bool (const TokenData&)> TokenDataFilter;

  class Lexer {
  private:
    std::shared_ptr<logger::Logger> logger;
    std::string_view::iterator position;
    std::string_view::iterator end;
    std::queue<Token> skippedTokens;

    std::optional<char> getNextChar();
    bool nextCharIsFilter(std::function<bool (char)> filter);
    bool nextCharIsNotFilter(std::function<bool (char)> filter);

    bool nextCharIs(char c);
    bool nextCharIsNot(char c);

    void skipCharsFilter(std::function<bool (char)> filter);
    void skipWhitespaces();
    void skipComment();
    void skipNonToken();

  public:
    explicit Lexer(
      std::shared_ptr<logger::Logger> logger,
      std::string_view::iterator begin,
      std::string_view::iterator end
    );

    std::optional<Token> next();

    bool nextIs(TokenDataFilter filter);
    bool nextIsSimple(SimpleTokenData data);

    bool skipTo(TokenDataFilter filter);
    bool expect(TokenDataFilter filter, TokenDataFilter fallback);
  };
};
