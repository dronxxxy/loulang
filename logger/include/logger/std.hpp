#pragma once

#include <logger/logger.hpp>

namespace logger {
  class StdLogger final : public Logger {
  protected:
    void log(Level level, std::string_view string) override;

  public:
    explicit StdLogger();
  };
};
