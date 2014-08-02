#ifndef RBX_UTIL_LOGGER_HPP
#define RBX_UTIL_LOGGER_HPP

#include <string>

namespace rubinius {
  namespace utilities {
    namespace logger {
      enum logger_type {
        eSyslog,
        eConsoleLogger,
        eFileLogger
      };

      void open(logger_type type, const char* identifier);
      void close();

      void fatal(const char* message, ...);
      void error(const char* message, ...);
      void warn(const char* message, ...);
      void info(const char* message, ...);
      void debug(const char* message, ...);

      class Logger {
#define LOGGER_TIME_SIZE    16

        char formatted_time[LOGGER_TIME_SIZE];

      public:

        Logger() { }
        virtual ~Logger() { }

        virtual void fatal(const char* message, int size) = 0;
        virtual void error(const char* message, int size) = 0;
        virtual void warn(const char* message, int size) = 0;
        virtual void info(const char* message, int size) = 0;
        virtual void debug(const char* message, int size) = 0;

        char* timestamp();
      };

      class Syslog : public Logger {
      public:

        Syslog(const char* identifier);
        ~Syslog();

        void fatal(const char* message, int size);
        void error(const char* message, int size);
        void warn(const char* message, int size);
        void info(const char* message, int size);
        void debug(const char* message, int size);
      };

      class ConsoleLogger : public Logger {
        std::string* identifier_;

        void write_log(const char* level, const char* message, int size);

      public:

        ConsoleLogger(const char* identifier);
        ~ConsoleLogger();

        void fatal(const char* message, int size);
        void error(const char* message, int size);
        void warn(const char* message, int size);
        void info(const char* message, int size);
        void debug(const char* message, int size);
      };

      class FileLogger : public Logger {
        std::string* identifier_;
        int logger_fd_;

        void write_log(const char* level, const char* message, int size);

      public:

        FileLogger(const char* identifier);
        ~FileLogger();

        void fatal(const char* message, int size);
        void error(const char* message, int size);
        void warn(const char* message, int size);
        void info(const char* message, int size);
        void debug(const char* message, int size);
      };
    }
  }
}
#endif
