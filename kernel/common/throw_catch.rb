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
  def catch(obj = Object.new, &block)
    raise LocalJumpError unless block_given?

    Rubinius::ThrownValue.register(obj) do
      return Rubinius.catch(obj, block)
    end
  end
  module_function :catch

  def throw(obj, value=nil)
    unless Rubinius::ThrownValue.available? obj
      raise ArgumentError, "uncaught throw #{obj.inspect}"
    end

    Rubinius.throw obj, value
  end
  module_function :throw
end
