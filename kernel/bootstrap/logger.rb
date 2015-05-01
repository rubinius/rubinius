module Rubinius
  class Logger
    attr_accessor :name
    attr_accessor :format

    def self.allocate
      Rubinius.primitive :logger_allocate
      raise PrimitiveFailure, "Rubinius::Logger.allocate failed"
    end

    def self.system
      @system_logger ||= new "system"
    end

    def self.log_exception(message, e)
      log = system

      log.error message
      STDERR.puts message, "\n"

      msg = "#{e.message} (#{e.class})"
      log.error msg
      STDERR.puts msg, "\nBacktrace:\n\n"

      color = STDERR.tty?
      exception = e

      while exception
        if custom_trace = exception.custom_backtrace
          custom_trace.each do |line|
            log.error line
            STDERR.puts line
          end
        end

        exception.backtrace.each { |line| log.error line }
        STDERR.puts exception.awesome_backtrace.show("\n", color)

        if exception = exception.parent
          msg = "Caused by: #{exception.message} (#{exception.class})"
          log.error msg
          STDERR.puts "\n", msg, "\n"
        end
      end
    end

    def initialize(name)
      @name = name
      @format = "#{name}: %s"
    end

    def write(message)
      Rubinius.primitive :logger_write
      raise PrimitiveFailure, "Rubinius::Logger#write primitive failed"
    end

    def fatal(message)
      Rubinius.primitive :logger_fatal
      raise PrimitiveFailure, "Rubinius::Logger#fatal primitive failed"
    end

    def error(message)
      Rubinius.primitive :logger_error
      raise PrimitiveFailure, "Rubinius::Logger#error primitive failed"
    end

    def warn(message)
      Rubinius.primitive :logger_warn
      raise PrimitiveFailure, "Rubinius::Logger#warn primitive failed"
    end

    def info(message)
      Rubinius.primitive :logger_info
      raise PrimitiveFailure, "Rubinius::Logger#info primitive failed"
    end

    def debug(message)
      Rubinius.primitive :logger_debug
      raise PrimitiveFailure, "Rubinius::Logger#debug primitive failed"
    end
  end
end
