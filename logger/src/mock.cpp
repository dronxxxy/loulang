#include <logger/mock.hpp>
#include <string>

using namespace logger;

MockLogger::MockLogger() : records() {}
MockLogger::~MockLogger() {
  if (!records.empty()) {
    throw std::runtime_error(std::format("all messages in mock logger should be caught: `{}`", records.front().message));
  }
}

void MockLogger::log(Level level, std::string_view string) {
  records.emplace(Record {
    .level = level,
    .message = std::string(string),
  });
}

static std::string_view levelToString(Level level) {
  switch (level) {
    case Level::Error: return "error";
    case Level::Warning: return "warning";
    case Level::Info: return "info";
    case Level::Debug: return "debug";
  }
}

void MockLogger::expects(Level level, std::string_view message) {
  if (records.empty()) {
    throw std::runtime_error(std::format("expected message `{}` but there is no message in queue", message));
  }
  Record record = records.front();
  records.pop();
  if (record.level != level) {
    throw std::runtime_error(std::format("expected {} got {}", levelToString(level), levelToString(record.level)));
  }
  if (record.message != message) {
    throw std::runtime_error(std::format("expected `{}` got `{}`", message, record.message));
  }
}
