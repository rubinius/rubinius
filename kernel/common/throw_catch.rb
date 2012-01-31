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
