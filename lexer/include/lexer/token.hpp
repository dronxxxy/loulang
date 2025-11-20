#pragma once

#include <string_view>
#include <variant>
#include <stdint.h>

namespace lexer {
  enum class SimpleTokenData {
    OpeningCircleBrace, ClosingCircleBrace,
    OpeningFigureBrace, ClosingFigureBrace,
    OpeningSquareBrace, ClosingSquareBrace,
    LessThan, GreaterThan,
    LessOrEquals, GreaterOrEquals,
    Assign, Equals,
    Meta, Const, Final, Var, Public,
    Extern, Fun,
    Colon,
    EndOfInput,
  };

  template <typename T>
  class ComplexTokenData {
    private:
      T value;
    public:
      ComplexTokenData(T value) : value(value) {}
      bool operator==(const ComplexTokenData<T> &other) const { return this->value == other.value; }
      T getValue() const { return this->value; }
  };

  class IdentTokenData final : public ComplexTokenData<std::string_view> {};
  class IntTokenData final : public ComplexTokenData<uint64_t> {};

  typedef std::variant<
    SimpleTokenData,
    IdentTokenData,
    IntTokenData
  > TokenData;

  class Token {
  private:
    TokenData data;
    std::string_view slice;

  public:
    const TokenData& getData() const;
    explicit Token(std::string_view slice, TokenData data);
    bool operator==(const Token &other) const;
    std::string toString() const;
    bool isEoi() const;
    const SimpleTokenData *isSimple() const;
  };
};
