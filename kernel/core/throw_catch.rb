# depends on: class.rb

##
# Emulates the interface of Exception, but isn't one.

class ThrownValue
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
    begin
      ThrownValue.register(sym) do
        yield
      end
    rescue ThrownValue => val
      return val.value if val.name == sym
      Rubinius.asm(val) do |v|
        v.bytecode(self)
        raise_exc
      end
    end
  end
  module_function :catch

  def throw(sym, value = nil)
    unless ThrownValue.available? sym
      raise NameError.new("uncaught throw `#{sym}'", sym.to_sym)
    end
    
    exc = ThrownValue.new(sym, value, MethodContext.current.sender)
    Rubinius.asm(exc) do |v|
      v.bytecode(self)
      raise_exc
    end
  end
  module_function :throw
end


