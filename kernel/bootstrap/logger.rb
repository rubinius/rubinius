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

    def initialize(name)
      @name = name
      @format = "#{name}: %s"
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
