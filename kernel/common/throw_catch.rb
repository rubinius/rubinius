# depends on: class.rb

##
# Emulates the interface of Exception, but isn't one.

class ThrownValue < Exception
  def initialize(name, value, ctx)
    @name = name
    @value = value
    @context = ctx
  end
  
  attr_reader :name
  attr_reader :value
  
  def message
    "A thrown value from Kernel#throw"
  end
  
  def backtrace
    if @context.kind_of? MethodContext
      @context = Backtrace.backtrace(@context)
    end
    
    return @context
  end
  
  def set_backtrace(obj)
    @context = obj
  end
  
  def exception(msg)
    true
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

module Kernel
  def catch(sym)
    symbol = Type.coerce_to_symbol(sym)
    begin
      ThrownValue.register(symbol) do
        yield
      end
    rescue ThrownValue => val
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
    
    unless ThrownValue.available? symbol
      raise NameError.new("uncaught throw `#{sym}'", symbol)
    end

    exc = ThrownValue.new(symbol, value, MethodContext.current.sender)
    Rubinius.asm(exc) do |v|
      v.bytecode(self)
      raise_exc
    end
  end
  module_function :throw
end


