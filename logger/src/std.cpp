#include <logger/std.hpp>
#include <iostream>

using namespace logger;

StdLogger::StdLogger() {}

void StdLogger::log(Level level, std::string_view string) {
  switch (level) {
    case Level::Error: std::cout << "\e[31;1merror: "; break;
    case Level::Warning: std::cout << "\e[33;1mwarning: "; break;
    case Level::Info: std::cout << "\e[32;1minfo: "; break;
    case Level::Debug: std::cout << "\e[34;1mdebug: "; break;
  }
  std::cout << "\e[0m";
  std::cout << string << std::endl;
}
