# depends on: class.rb

module Rubinius
  class ThrownValue < Exception
    def initialize(name, value, ctx)
      super(nil)
      @name = name
      @value = value
    end

    attr_reader :name
    attr_reader :value

    def message
      "A thrown value from Kernel#throw"
    end

    def self.register(sym)
      cur = Thread.current[:__catches__]
      if cur.nil?
        cur = []
        Thread.current[:__catches__] = cur
      end

      cur << sym

      begin
        yield
      ensure
        cur.pop
      end
    end

    def self.available?(sym)
      cur = Thread.current[:__catches__]
      return false if cur.nil?
      cur.include? sym
    end
  end
end

module Kernel
  def catch(sym)
    symbol = Type.coerce_to_symbol(sym)
    begin
      Rubinius::ThrownValue.register(symbol) do
        yield
      end
    rescue Rubinius::ThrownValue => val
      return val.value if val.name == symbol
      Rubinius.asm(val) do |v|
        v.bytecode(self)
        raise_exc
      end
    end
  end
  module_function :catch

  def throw(sym, value = nil)
    symbol = Type.coerce_to_symbol(sym)

    unless Rubinius::ThrownValue.available? symbol
      raise NameError.new("uncaught throw `#{sym}'", symbol)
    end

    exc = Rubinius::ThrownValue.new(symbol, value, Rubinius::VariableScope.of_sender)
    Rubinius.asm(exc) do |v|
      v.bytecode(self)
      raise_exc
    end
  end
  module_function :throw
end
