module Rubinius
  module ThrownValue
    def self.register(sym)
      cur = (Thread.current[:__catches__] ||= [])
      cur << sym

      begin
        yield
      ensure
        cur.pop
      end
    end

    def self.available?(sym)
      cur = Thread.current[:__catches__]
      return false unless cur
      cur.include? sym
    end
  end
end

module Kernel
  def catch(sym, &block)
    raise LocalJumpError unless block_given?

    symbol = Rubinius::Type.coerce_to_symbol(sym)

    Rubinius::ThrownValue.register(symbol) do
      return Rubinius.catch(symbol, block)
    end
  end
  module_function :catch

  def throw(sym, value=nil)
    symbol = Rubinius::Type.coerce_to_symbol(sym)

    unless Rubinius::ThrownValue.available? symbol
      raise NameError.new("uncaught throw `#{sym}'", symbol)
    end

    Rubinius.throw symbol, value
  end
  module_function :throw
end
