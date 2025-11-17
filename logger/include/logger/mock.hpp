#pragma once

#include <logger/logger.hpp>
#include <queue>

namespace logger {
  class MockLogger final : public Logger {
  private:
    struct Record {
      Level level;
      std::string message;
    };

    std::queue<Record> records;

  protected:
    void log(Level level, std::string_view string) override;

  public:
    explicit MockLogger();
    ~MockLogger();

    void expects(Level level, std::string_view message);
  };
};

