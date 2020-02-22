#ifndef RBX_LOGGER_HPP
#define RBX_LOGGER_HPP

#include "defines.hpp"
#include "spinlock.hpp"

#include <functional>
#include <string>
#include <semaphore.h>
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


    void open(logger_type type, logger_level level, const char* identifier, ...);
    void reset();
    void lock();
    bool try_lock();
    void unlock();
    void close();

    typedef void (*PrintFunction)(const char* message, ...);

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

    void write(std::function<void (PrintFunction writer)> f);
    void fatal(std::function<void (PrintFunction writer)> f);
    void error(std::function<void (PrintFunction writer)> f);
    void warn(std::function<void (PrintFunction writer)> f);
    void info(std::function<void (PrintFunction writer)> f);
    void debug(std::function<void (PrintFunction writer)> f);

    NORETURN(void abort(const char* message, ...));
    NORETURN(void abort(const char* message, va_list args));

    void set_loglevel(logger_level level);

    class Logger {
      rubinius::locks::spinlock_mutex lock_;

#define LOGGER_TIME_SIZE    16

      char formatted_time_[LOGGER_TIME_SIZE];

    public:
      Logger()
        : lock_()
      { }

      virtual ~Logger() { }

      virtual void write(const char* message, int size) = 0;
      virtual void fatal(const char* message, int size) = 0;
      virtual void error(const char* message, int size) = 0;
      virtual void warn(const char* message, int size) = 0;
      virtual void info(const char* message, int size) = 0;
      virtual void debug(const char* message, int size) = 0;

      char* timestamp();

      rubinius::locks::spinlock_mutex& spinlock() {
        return lock_;
      }

      void lock() {
        lock_.lock();
      }

      bool try_lock() {
        return lock_.try_lock();
      }

      void unlock() {
        lock_.unlock();
      }
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
    };

    class FileLogger : public Logger {
      class SemaphoreLock {
        sem_t* semaphore_;

        const static uint64_t cLockLimit = 100000;

      public:
        SemaphoreLock(sem_t* semaphore)
          : semaphore_(semaphore)
        {
          lock();
        }

        virtual ~SemaphoreLock() {
          unlock();
        }

        void lock();
        void unlock();
      };

      std::string path_;
      std::string label_;
      int logger_fd_;

      std::string ipc_handle_;
      long shm_size_;
      void* rotate_shm_;
      uint64_t rotate_flag_;
      sem_t* semaphore_;

      long limit_;
      long archives_;
      int perms_;
      int write_status_;

      void write_log(const char* level, const char* message, int size);
      void rotate();
      void cleanup();
      void reopen();
      void signal_reopen();

      bool reopen_p();

      bool try_lock();
      void unlock();

    public:

      FileLogger(const char* path, va_list varargs);
      ~FileLogger();

      void write(const char* message, int size);
      void fatal(const char* message, int size);
      void error(const char* message, int size);
      void warn(const char* message, int size);
      void info(const char* message, int size);
      void debug(const char* message, int size);
    };
  }
}
#endif
