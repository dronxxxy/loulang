#pragma once

#include <string_view>
#include <format>

namespace logger {
  enum class Level { Error, Warning, Info, Debug };

  class Logger {
  protected:
    virtual void log(Level level, std::string_view string) = 0;

  public:
    template<typename ... Args>
    void error(std::string_view message, Args ...args) {
      this->log(Level::Error, std::vformat(message, std::make_format_args(args...)));
    }

    template<typename ... Args>
    void warning(std::string_view message, Args ...args) {
      this->log(Level::Warning, std::vformat(message, std::make_format_args(args...)));
    }

    template<typename ... Args>
    void info(std::string_view message, Args ...args) {
      this->log(Level::Info, std::vformat(message, std::make_format_args(args...)));
    }

    template<typename ... Args>
    void debug(std::string_view message, Args ...args) {
      this->log(Level::Debug, std::vformat(message, std::make_format_args(args...)));
    }
  };
}
