#pragma once

#include <functional>
#include <optional>
#include <memory>
#include <logger/logger.hpp>
#include <lexer/token.hpp>

namespace lexer {
  class Lexer {
  private:
    std::shared_ptr<logger::Logger> logger;
    std::string_view::iterator position;
    std::string_view::iterator end;

    std::optional<char> getNextChar();
    bool nextIs(char c);
    bool nextIsFilter(std::function<bool (char)> filter);

    void skipFilter(std::function<bool (char)> filter);
    void skipWhitespaces();

  public:
    explicit Lexer(
      std::shared_ptr<logger::Logger> logger,
      std::string_view::iterator begin,
      std::string_view::iterator end
    );
    std::optional<Token> next();
  };
};
