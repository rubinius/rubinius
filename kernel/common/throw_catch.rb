# -*- encoding: us-ascii -*-

module Rubinius
  module ThrownValue
    def self.register(obj)
      cur = (Thread.current[:__catches__] ||= [])
      cur << obj

      begin
        yield
      ensure
        cur.pop
      end
    end

    def self.available?(obj)
      cur = Thread.current[:__catches__]
      return false unless cur
      cur.each do |c|
        return true if Rubinius::Type.object_equal(c, obj)
      end
      false
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
