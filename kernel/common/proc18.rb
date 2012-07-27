# -*- encoding: us-ascii -*-

class Proc
  def to_s
    "#<#{self.class}:0x#{self.object_id.to_s(16)}@#{@block.file}:#{@block.line}>"
  end

  alias_method :inspect, :to_s

  class Method < Proc
    def self.__from_method__(meth)
      obj = __allocate__
      obj.bound_method = meth

      return obj
    end

    def __yield__(*args, &block)
      # do a block style unwrap..
      if args.size == 1 and args.first.kind_of? Array and args.first.size > 1
        args = args.first
      end

      @bound_method.call(*args, &block)
    end
  end
end
