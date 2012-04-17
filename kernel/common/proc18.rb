# -*- encoding: us-ascii -*-

class Proc
  def to_s
    "#<#{self.class}:0x#{self.object_id.to_s(16)}@#{@block.file}:#{@block.line}>"
  end

  alias_method :inspect, :to_s

  class Method < Proc
    def self.__from_method__(meth)
      obj = allocate()
      obj.bound_method = meth

      return obj
    end
  end
end
