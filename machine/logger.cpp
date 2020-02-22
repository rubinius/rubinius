#include "util/file.hpp"

#include "logger.hpp"
#include "thread_phase.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <zlib.h>

#include <chrono>
#include <sstream>
#include <thread>

namespace rubinius {
  namespace logger {
    static Logger* logger_ = 0;
    static logger_level loglevel_ = eWarn;

    void open(logger_type type, logger_level level, const char* identifier, ...) {
      va_list varargs;

      switch(type) {
      case eSyslog:
        logger_ = new Syslog(identifier);
        break;
      case eConsoleLogger:
        logger_ = new ConsoleLogger(identifier);
        break;
      case eFileLogger:
        va_start(varargs, identifier);
        logger_ = new FileLogger(identifier, varargs);
        va_end(varargs);
        break;
      }

      loglevel_ = level;
    }

    void reset() {
      if(logger_) {
        logger_->spinlock().reset();
      }
    }

    void lock() {
      if(logger_) {
        logger_->lock();
      }
    }

    bool try_lock() {
      if(logger_) {
        return logger_->try_lock();
      } else {
        return false;
      }
    }

    void unlock() {
      if(logger_) {
        logger_->unlock();
      }
    }

    void close() {
      delete logger_;
    }

#define LOGGER_MSG_SIZE   1024

    static int append_newline(char* message) {
      char* end = (char*)memchr(message, '\0', LOGGER_MSG_SIZE);
      size_t bytes = end ? end - message : LOGGER_MSG_SIZE;

      if(bytes + 1 < LOGGER_MSG_SIZE) {
        if(message[bytes-1] != '\n') {
          message[bytes] = '\n';
          message[bytes += 1] = '\0';
        }
      } else {
        if(message[bytes-1] != '\n') message[bytes-1] = '\n';
      }

      return bytes;
    }

    void set_loglevel(logger_level level) {
      loglevel_ = level;
    }

    void write(std::function<void (PrintFunction writer)> f) {
      f(logger::write);
    }

    void fatal(std::function<void (PrintFunction writer)> f) {
      f(logger::fatal);
    }

    void error(std::function<void (PrintFunction writer)> f) {
      f(logger::error);
    }

    void warn(std::function<void (PrintFunction writer)> f) {
      f(logger::warn);
    }

    void info(std::function<void (PrintFunction writer)> f) {
      f(logger::info);
    }

    void debug(std::function<void (PrintFunction writer)> f) {
      f(logger::debug);
    }

    void write(const char* message, ...) {
      va_list args;
      va_start(args, message);
      write(message, args);
      va_end(args);
    }

    void fatal(const char* message, ...) {
      va_list args;
      va_start(args, message);
      fatal(message, args);
      va_end(args);
    }

    void error(const char* message, ...) {
      va_list args;
      va_start(args, message);
      error(message, args);
      va_end(args);
    }

    void warn(const char* message, ...) {
      va_list args;
      va_start(args, message);
      warn(message, args);
      va_end(args);
    }

    void info(const char* message, ...) {
      va_list args;
      va_start(args, message);
      info(message, args);
      va_end(args);
    }

    void debug(const char* message, ...) {
      va_list args;
      va_start(args, message);
      debug(message, args);
      va_end(args);
    }

    void abort(const char* message, ...) {
      va_list args;
      va_start(args, message);
      abort(message, args);
      va_end(args);
    }

    void write(const char* message, va_list args) {
      if(logger_) {
        std::lock_guard<locks::spinlock_mutex> guard(logger_->spinlock());

        char buf[LOGGER_MSG_SIZE];

        (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);

        logger_->write(buf, append_newline(buf));
      }
    }

    void fatal(const char* message, va_list args) {
      if(logger_) {
        if(loglevel_ < eFatal) return;

        std::lock_guard<locks::spinlock_mutex> guard(logger_->spinlock());

        char buf[LOGGER_MSG_SIZE];

        (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);

        logger_->fatal(buf, append_newline(buf));
      }
    }

    void error(const char* message, va_list args) {
      if(logger_) {
        if(loglevel_ < eError) return;

        std::lock_guard<locks::spinlock_mutex> guard(logger_->spinlock());

        char buf[LOGGER_MSG_SIZE];

        (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);

        logger_->error(buf, append_newline(buf));
      }
    }

    void warn(const char* message, va_list args) {
      if(logger_) {
        if(loglevel_ < eWarn) return;

        std::lock_guard<locks::spinlock_mutex> guard(logger_->spinlock());

        char buf[LOGGER_MSG_SIZE];

        (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);

        logger_->warn(buf, append_newline(buf));
      }
    }

    void info(const char* message, va_list args) {
      if(logger_) {
        if(loglevel_ < eInfo) return;

        std::lock_guard<locks::spinlock_mutex> guard(logger_->spinlock());

        char buf[LOGGER_MSG_SIZE];

        (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);

        logger_->info(buf, append_newline(buf));
      }
    }

    void debug(const char* message, va_list args) {
      if(logger_) {
        if(loglevel_ < eDebug) return;

        std::lock_guard<locks::spinlock_mutex> guard(logger_->spinlock());

        char buf[LOGGER_MSG_SIZE];

        (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);

        logger_->debug(buf, append_newline(buf));
      }
    }

    void abort(const char* message, va_list args) {
      char buf[LOGGER_MSG_SIZE];

      (void) vsnprintf(buf, LOGGER_MSG_SIZE, message, args);

      std::cerr << "logger: abort: " << buf << std::endl;
      ::abort();
    }

    char* Logger::timestamp() {
      time_t clock;
      struct tm lt;

      time(&clock);
      localtime_r(&clock, &lt);

      strftime(formatted_time_, LOGGER_TIME_SIZE, "%b %e %H:%M:%S", &lt);

      return formatted_time_;
    }

    Syslog::Syslog(const char* identifier)
      : Logger()
    {
      openlog(identifier, LOG_CONS | LOG_PID, LOG_LOCAL7);

      int logmask = eWarn;

      switch(loglevel_) {
      case eFatal:
        logmask = LOG_EMERG;
        break;
      case eError:
        logmask = LOG_ERR;
        break;
      case eWarn:
        logmask = LOG_WARNING;
        break;
      case eInfo:
        logmask = LOG_NOTICE;
        break;
      case eDebug:
        logmask = LOG_DEBUG;
        break;
      }

      setlogmask(LOG_UPTO(logmask));
    }

    Syslog::~Syslog() {
      closelog();
    }

    // Syslog doesn't give us the ability to write a message to the log
    // independent of a priority. Bummer.
    void Syslog::write(const char* message, int size) {
      syslog(LOG_INFO, "%s", message);
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
      , identifier_(identifier)
    {
      std::ostringstream label;
      label << identifier_ << "[" << getpid() << "]";
      label_ = std::string(label.str());
    }

    void ConsoleLogger::write_log(const char* level, const char* message, int size) {
      fprintf(stderr, "%s %s %s %s", timestamp(), label_.c_str(), level, message);
    }

#define LOGGER_LEVEL_FATAL  "<Fatal>"
#define LOGGER_LEVEL_ERROR  "<Error>"
#define LOGGER_LEVEL_WARN   "<Warn>"
#define LOGGER_LEVEL_INFO   "<Info>"
#define LOGGER_LEVEL_DEBUG  "<Debug>"

    void ConsoleLogger::write(const char* message, int size) {
      fprintf(stderr, "%s %s %s", timestamp(), label_.c_str(), message);
    }

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

    void FileLogger::SemaphoreLock::lock() {
      int nanoseconds = 0;

      while(sem_trywait(semaphore_) != 0) {
        switch(errno) {
          case EAGAIN:
          case EINTR:
            break;
          case EINVAL:
            logger::abort("%s: unable to lock semaphore", strerror(errno));
          case EDEADLK:
            logger::abort("%s: unable to lock semaphore", strerror(errno));
          default:
            logger::abort("%s: unable to lock semaphore", strerror(errno));
        }

        {
          static int i = 0;
          static int delay[] = {
            133, 464, 254, 306, 549, 287, 358, 638, 496, 81,
            472, 288, 131, 31, 435, 258, 221, 73, 537, 854
          };
          static int modulo = sizeof(delay) / sizeof(int);

          int ns = delay[i++ % modulo];
          std::this_thread::sleep_for(std::chrono::nanoseconds(ns));

          nanoseconds += ns;

          if(nanoseconds > cLockLimit * 2) {
            logger::abort("logger: possible invalid semaphore state detected, unable to reset");
          } else if(nanoseconds > cLockLimit) {
            std::cerr << "logger: possible invalid semaphore state detected, forcibly resetting semaphore" << std::endl;
            sem_post(semaphore_);
          }
        }
      }
    }

    void FileLogger::SemaphoreLock::unlock() {
      sem_post(semaphore_);
    }

#define LOGGER_MAX_COPY_BUF 1048576
#define LOGGER_OPEN_FLAGS   (O_CREAT | O_APPEND | O_WRONLY | O_CLOEXEC)
#define LOGGER_REOPEN_FLAGS (O_CREAT | O_TRUNC | O_APPEND | O_WRONLY | O_CLOEXEC)
#define LOGGER_FROM_FLAGS   (O_RDONLY | O_CLOEXEC)
#define LOGGER_TO_FLAGS     (O_CREAT | O_TRUNC | O_APPEND | O_WRONLY | O_CLOEXEC)

#ifdef __FreeBSD__
#define LOGGER_SEM_OPEN_FLAGS   (O_CREAT)
#else
#define LOGGER_SEM_OPEN_FLAGS   (O_CREAT | O_RDWR)
#endif
#define LOGGER_SEM_OPEN_PERMS   (S_IRUSR | S_IWUSR)

#define LOGGER_SHM_OPEN_FLAGS   (O_CREAT | O_RDWR)
#define LOGGER_SHM_OPEN_PERMS   (S_IRUSR | S_IWUSR)

    FileLogger::FileLogger(const char* path, va_list varargs)
      : Logger()
      , path_(path)
      , label_()
      , logger_fd_(-1)
      , ipc_handle_()
      , shm_size_(sizeof(uint64_t))
      , rotate_shm_(NULL)
      , rotate_flag_(0)
      , semaphore_(NULL)
    {
      std::ostringstream label;
      label << " [" << getpid() << "] ";
      label_ = label.str();

      limit_ = va_arg(varargs, long);
      archives_ = va_arg(varargs, long);
      perms_ = va_arg(varargs, int);

      if(const char* str = strrchr(path, '/')) {
        ipc_handle_.assign(str);
      } else {
        ipc_handle_.assign(path);
      }

      if((semaphore_ = ::sem_open(ipc_handle_.c_str(),
              LOGGER_SEM_OPEN_FLAGS, LOGGER_SEM_OPEN_PERMS, 1)) == SEM_FAILED)
      {
        logger::abort("%s: logger: unable to open semaphore", strerror(errno));
      }

      int fd = 0;
      if((fd = ::shm_open(ipc_handle_.c_str(),
              LOGGER_SHM_OPEN_FLAGS, LOGGER_SHM_OPEN_PERMS)) < 0) {
        logger::abort("%s: logger: unable to open shared memory", strerror(errno));
      }

      /* This call is necessary on a newly opened shared memory region file
       * descriptor, but will fail if the region is already allocated. So, we
       * make the call and ignore any failure.
       */
      (void)ftruncate(fd, shm_size_);

      if((rotate_shm_ = ::mmap(NULL, shm_size_,
                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
      {
        logger::abort("%s: logger: unable to map shared memory", strerror(errno));
      }

      rotate_flag_ = *((uint64_t*)rotate_shm_);
      ::close(fd);

      if((logger_fd_ = ::open(path, LOGGER_OPEN_FLAGS, perms_)) < 0) {
        logger::abort("%s: logger: unable to open %s", strerror(errno), path);
      }

      // The umask setting will override our permissions for open().
      if(chmod(path, perms_) < 0) {
        logger::abort("%s: logger: unable to set mode %s", strerror(errno), path);
      }
    }

    FileLogger::~FileLogger() {
      if(semaphore_ != NULL) {
        if(sem_close(semaphore_) < 0) {
          std::cerr << strerror(errno) << ": logger: failed closing semaphore" << std::endl;
        }
      }

      if(rotate_shm_ != NULL) {
        if(munmap(rotate_shm_, shm_size_) < 0) {
          std::cerr << strerror(errno) << ": logger: failed unmapping memory" << std::endl;
        }
      }

      cleanup();
    }

    void FileLogger::cleanup() {
      if(logger_fd_ > 0) {
        ::close(logger_fd_);
        logger_fd_ = -1;
      }
    }

    void FileLogger::signal_reopen() {
      ((uint64_t*)rotate_shm_)[0] = ++rotate_flag_;
    }

    bool FileLogger::reopen_p() {
      return rotate_flag_ < *((uint64_t*)rotate_shm_);
    }

    void FileLogger::reopen() {
      logger_fd_ = ::open(path_.c_str(), LOGGER_REOPEN_FLAGS, perms_);
    }

    void FileLogger::rotate() {
      struct stat st;

      void* buf = malloc(LOGGER_MAX_COPY_BUF);
      if(!buf) return;

      char* from = (char*)malloc(MAXPATHLEN);
      if(!from) {
        free(buf);
        return;
      }

      char* to = (char*)malloc(MAXPATHLEN);
      if(!to) {
        free(buf);
        free(from);
        return;
      }

      for(int i = archives_; i > 0; i--) {
        if(i > 1) {
          snprintf(from, MAXPATHLEN, "%s.%d.Z", path_.c_str(), i - 1);
        } else {
          snprintf(from, MAXPATHLEN, "%s", path_.c_str());
        }

        if(stat(from, &st) || !S_ISREG(st.st_mode)) continue;

        snprintf(to, MAXPATHLEN, "%s.%d.Z", path_.c_str(), i);

        int from_fd = ::open(from, LOGGER_FROM_FLAGS, perms_);
        if(from_fd < 0) continue;

        int to_fd = ::open(to, LOGGER_TO_FLAGS, perms_);
        if(to_fd < 0) {
          ::close(from_fd);
          continue;
        }

        int bytes;

        if(i > 1) {
          while((bytes = ::read(from_fd, buf, LOGGER_MAX_COPY_BUF)) > 0) {
            if(::write(to_fd, buf, bytes) < 0) break;
          }
        } else {
          gzFile gzf;

          gzf = gzdopen(to_fd, "a");
          if(gzf == Z_NULL) continue;

          while((bytes = ::read(from_fd, buf, LOGGER_MAX_COPY_BUF)) > 0) {
            if(gzwrite(gzf, buf, bytes) <= 0) break;
          }

          gzclose(gzf);
        }

        ::close(from_fd);
        ::close(to_fd);
      }

      free(buf);
      free(from);
      free(to);

      reopen();
    }

    void FileLogger::write_log(const char* level, const char* message, int size) {
      SemaphoreLock guard(semaphore_);

      if(lseek(logger_fd_, 0, SEEK_END) > limit_) {
        cleanup();
        rotate();
      } else if(reopen_p()) {
        cleanup();
        reopen();
      }

      const char* time = timestamp();
      write_status_ = ::write(logger_fd_, time, strlen(time));
      write_status_ = ::write(logger_fd_, label_.c_str(), label_.size());
      if(level) {
        write_status_ = ::write(logger_fd_, level, strlen(level));
        write_status_ = ::write(logger_fd_, " ", 1);
      }
      write_status_ = ::write(logger_fd_, message, size);
    }

    void FileLogger::write(const char* message, int size) {
      write_log(NULL, message, size);
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
