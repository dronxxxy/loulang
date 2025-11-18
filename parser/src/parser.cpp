#include <parser/parser.hpp>

using namespace parser;

Parser::Parser(
  std::shared_ptr<logger::Logger> logger,
  lexer::Lexer &&lexer
): lexer(std::move(lexer)), logger(logger) {}
