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

      exception = e
      exceptions = []

      while exception
        exceptions.unshift exception
        exception = exception.cause
      end

      color = STDERR.tty?

      while exception = exceptions.shift
        if custom_trace = exception.custom_backtrace
          msg = "User-defined backtrace:"
          log.error msg
          STDERR.puts msg

          custom_trace.reverse_each do |line|
            log.error line
            STDERR.puts line
          end

          msg = "Backtrace: "
          log.error msg
          STDERR.puts "\n", msg, "\n"
        end

        exception.backtrace.reverse_each { |line| log.error line }
        STDERR.puts exception.awesome_backtrace.show("\n", color)

        msg = "#{exception.message} (#{exception.class})"
        log.error msg
        STDERR.puts "\n", msg

        unless exceptions.empty?
          msg = "Causing:"
          log.error msg
          STDERR.puts "\n", msg, "\n"
        end
      end

      log.error message
      STDERR.puts "\n", message
    end

    def initialize(name)
      @name = name
      @format = "#{name}: %s"
    end

    private :initialize

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
