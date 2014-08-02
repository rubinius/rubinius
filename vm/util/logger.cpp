#include "util/logger.hpp"
#include "util/thread.hpp"

#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#include <ostream>

namespace rubinius {
  namespace utilities {
    namespace logger {
      static thread::Mutex mutex_ = 0;
      static Logger* logger_ = 0;

      void open(logger_type type, const char* identifier) {
        mutex_.init();

        switch(type) {
        case eSyslog:
          logger_ = new Syslog(identifier);
          break;
        case eConsoleLogger:
          logger_ = new ConsoleLogger(identifier);
          break;
        case eFileLogger:
          logger_ = new FileLogger(identifier);
          break;
        }
      }

      void close() {
        delete logger_;
      }

#define LOGGER_MSG_SIZE   1024

      static int append_newline(char* message) {
        char* end = (char*)memchr(message, '\0', LOGGER_MSG_SIZE);
        size_t bytes = end ? end - message : LOGGER_MSG_SIZE;

        if(bytes < LOGGER_MSG_SIZE) {
          if(message[bytes-1] != '\n') {
            message[bytes] = '\n';
            message[bytes += 1] = '\0';
          }
        } else {
          if(message[bytes-1] != '\n') message[bytes-1] = '\n';
        }

        return bytes;
      }

      void fatal(const char* message, ...) {
        thread::Mutex::LockGuard guard(mutex_);

        if(logger_) {
          char buf[LOGGER_MSG_SIZE];

          va_list args;

          va_start(args, message);
          (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);
          va_end(args);

          logger_->fatal(buf, append_newline(buf));
        }
      }

      void error(const char* message, ...) {
        thread::Mutex::LockGuard guard(mutex_);

        if(logger_) {
          char buf[LOGGER_MSG_SIZE];

          va_list args;
          va_start(args, message);
          (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);
          va_end(args);

          logger_->error(buf, append_newline(buf));
        }
      }

      void warn(const char* message, ...) {
        thread::Mutex::LockGuard guard(mutex_);

        if(logger_) {
          char buf[LOGGER_MSG_SIZE];

          va_list args;
          va_start(args, message);
          (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);
          va_end(args);

          logger_->warn(buf, append_newline(buf));
        }
      }

      void info(const char* message, ...) {
        thread::Mutex::LockGuard guard(mutex_);

        if(logger_) {
          char buf[LOGGER_MSG_SIZE];

          va_list args;
          va_start(args, message);
          (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);
          va_end(args);

          logger_->info(buf, append_newline(buf));
        }
      }

      void debug(const char* message, ...) {
        thread::Mutex::LockGuard guard(mutex_);

        if(logger_) {
          char buf[LOGGER_MSG_SIZE];

          va_list args;
          va_start(args, message);
          (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);
          va_end(args);

          logger_->debug(buf, append_newline(buf));
        }
      }

      char* Logger::timestamp() {
        time_t clock;

        time(&clock);
        strftime(formatted_time, LOGGER_TIME_SIZE, "%b %e %H:%M:%S",
            localtime(&clock));
        return formatted_time;
      }

      Syslog::Syslog(const char* identifier)
        : Logger()
      {
        openlog(identifier, LOG_CONS | LOG_PID, LOG_LOCAL7);
      }

      Syslog::~Syslog() {
        closelog();
      }

      void Syslog::fatal(const char* message, int size) {
        syslog(LOG_ERR, "%s", message);
        fprintf(stderr, "%s", message);
      }

      void Syslog::error(const char* message, int size) {
        syslog(LOG_ERR, "%s", message);
      }

      void Syslog::warn(const char* message, int size) {
        syslog(LOG_WARNING, "%s", message);
      }

      void Syslog::info(const char* message, int size) {
        syslog(LOG_INFO, "%s", message);
      }

      void Syslog::debug(const char* message, int size) {
        syslog(LOG_DEBUG, "%s", message);
      }

      ConsoleLogger::ConsoleLogger(const char* identifier)
        : Logger()
      {
        std::ostringstream str;
        str << identifier << "[" << getpid() << "]";

        identifier_ = new std::string(str.str());
      }

      ConsoleLogger::~ConsoleLogger() {
        delete identifier_;
      }

      void ConsoleLogger::write_log(const char* level, const char* message, int size) {
        fprintf(stderr, "%s %s %s %s", timestamp(), identifier_->c_str(), level, message);
      }

#define LOGGER_LEVEL_FATAL  "<Fatal>"
#define LOGGER_LEVEL_ERROR  "<Error>"
#define LOGGER_LEVEL_WARN   "<Warn>"
#define LOGGER_LEVEL_INFO   "<Info>"
#define LOGGER_LEVEL_DEBUG  "<Debug>"

      void ConsoleLogger::fatal(const char* message, int size) {
        write_log(LOGGER_LEVEL_FATAL, message, size);
      }

      void ConsoleLogger::error(const char* message, int size) {
        write_log(LOGGER_LEVEL_ERROR, message, size);
      }

      void ConsoleLogger::warn(const char* message, int size) {
        write_log(LOGGER_LEVEL_WARN, message, size);
      }

      void ConsoleLogger::info(const char* message, int size) {
        write_log(LOGGER_LEVEL_INFO, message, size);
      }

      void ConsoleLogger::debug(const char* message, int size) {
        write_log(LOGGER_LEVEL_DEBUG, message, size);
      }

#define LOGGER_MAX_FILE     5242880
#define LOGGER_OPEN_FLAGS   (O_CREAT | O_APPEND | O_WRONLY)
#define LOGGER_OPEN_PERMS   0644

      FileLogger::FileLogger(const char* identifier)
        : Logger()
      {
        std::ostringstream str;
        str << " [" << getpid() << "] ";

        identifier_ = new std::string(str.str());

        logger_fd_ = ::open(identifier, LOGGER_OPEN_FLAGS, LOGGER_OPEN_PERMS);

        if(logger_fd_ < 0 && errno == EACCES) {
          // Fall back to cwd if log directory is inaccessible.
          if(char* s = strrchr(identifier, '/')) {
            logger_fd_ = ::open(s+1, LOGGER_OPEN_FLAGS, LOGGER_OPEN_PERMS);
          }
        }

        // Round robin if log file exceeds the limit
        if(lseek(logger_fd_, 0, SEEK_END) > LOGGER_MAX_FILE) {
          lseek(logger_fd_, 0, SEEK_SET);
        }
      }

      FileLogger::~FileLogger() {
        delete identifier_;
        ::close(logger_fd_);
      }

      void FileLogger::write_log(const char* level, const char* message, int size) {
        const char* time = timestamp();
        write(logger_fd_, time, strlen(time));
        write(logger_fd_, identifier_->c_str(), identifier_->size());
        write(logger_fd_, level, strlen(level));
        write(logger_fd_, " ", 1);
        write(logger_fd_, message, size);
      }

      void FileLogger::fatal(const char* message, int size) {
        write_log(LOGGER_LEVEL_FATAL, message, size);
        fprintf(stderr, "%s", message);
      }

      void FileLogger::error(const char* message, int size) {
        write_log(LOGGER_LEVEL_ERROR, message, size);
      }

      void FileLogger::warn(const char* message, int size) {
        write_log(LOGGER_LEVEL_WARN, message, size);
      }

      void FileLogger::info(const char* message, int size) {
        write_log(LOGGER_LEVEL_INFO, message, size);
      }

      void FileLogger::debug(const char* message, int size) {
        write_log(LOGGER_LEVEL_DEBUG, message, size);
      }
    }
  }
}
