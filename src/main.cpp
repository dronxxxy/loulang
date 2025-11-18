#include <memory>
#include <logger/std.hpp>
#include <lexer/lexer.hpp>

using namespace logger;
using namespace lexer;

int main(int argc, char** argv) {
  std::shared_ptr<Logger> logger = std::make_shared<StdLogger>();
  logger->error("hello, {} - {}?", "world", 2);
  logger->warning("hello, {} - {}?", "world", 5);
  logger->info("hello, {} - {}?", "world", 1);
  logger->debug("hello, {} - {}?", "world", 8);
  return 0;
}
