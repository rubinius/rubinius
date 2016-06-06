#ifndef RBX_LOGGER_HPP
#define RBX_LOGGER_HPP

#include "defines.hpp"

#include <string>
#include <stdarg.h>

namespace rubinius {
  namespace logger {
    enum logger_type {
      eSyslog,
      eConsoleLogger,
      eFileLogger
    };

    enum logger_level {
      eFatal  = 1,
      eError  = 2,
      eWarn   = 3,
      eInfo   = 4,
      eDebug  = 5
    };


    void open(logger_type type, const char* identifier, logger_level level=eWarn, ...);
    void close();

    void write(const char* message, ...);
    void fatal(const char* message, ...);
    void error(const char* message, ...);
    void warn(const char* message, ...);
    void info(const char* message, ...);
    void debug(const char* message, ...);

    void write(const char* message, va_list args);
    void fatal(const char* message, va_list args);
    void error(const char* message, va_list args);
    void warn(const char* message, va_list args);
    void info(const char* message, va_list args);
    void debug(const char* message, va_list args);

    void set_label();
    void set_loglevel(logger_level level);

    void reset_lock();

    class Logger {
#define LOGGER_TIME_SIZE    16

      char formatted_time_[LOGGER_TIME_SIZE];

    public:

      virtual ~Logger() { }

      virtual void write(const char* message, int size) = 0;
      virtual void fatal(const char* message, int size) = 0;
      virtual void error(const char* message, int size) = 0;
      virtual void warn(const char* message, int size) = 0;
      virtual void info(const char* message, int size) = 0;
      virtual void debug(const char* message, int size) = 0;

      virtual void set_label() = 0;

      char* timestamp();
    };

    class Syslog : public Logger {
    public:

      Syslog(const char* identifier);
      ~Syslog();

      void write(const char* message, int size);
      void fatal(const char* message, int size);
      void error(const char* message, int size);
      void warn(const char* message, int size);
      void info(const char* message, int size);
      void debug(const char* message, int size);

      void set_label();
    };

    class ConsoleLogger : public Logger {
      std::string identifier_;
      std::string label_;

      void write_log(const char* level, const char* message, int size);

    public:

      ConsoleLogger(const char* identifier);

      void write(const char* message, int size);
      void fatal(const char* message, int size);
      void error(const char* message, int size);
      void warn(const char* message, int size);
      void info(const char* message, int size);
      void debug(const char* message, int size);

      void set_label();
    };

    class FileLogger : public Logger {
      std::string path_;
      std::string label_;
      int logger_fd_;
      long limit_;
      long archives_;
      int perms_;
      int write_status_;

      void write_log(const char* level, const char* message, int size);
      void rotate();
      void cleanup();

    public:

      FileLogger(const char* path, va_list varargs);
      ~FileLogger();

      void write(const char* message, int size);
      void fatal(const char* message, int size);
      void error(const char* message, int size);
      void warn(const char* message, int size);
      void info(const char* message, int size);
      void debug(const char* message, int size);

      void set_label();
    };
  }
}
#endif
