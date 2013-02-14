# -*- encoding: us-ascii -*-

class Proc
  def to_s
    if @ruby_method
      code = @ruby_method.executable
      if code.respond_to? :file
        file = code.file
        if code.lines
          line = code.first_line
        else
          line = -1
        end
      else
        file = "(unknown)"
        line = -1
      end
    elsif @bound_method
      if @bound_method.respond_to?(:source_location)
        file, line = @bound_method.source_location
      else
        file, line = nil
      end
    else
      file, line = @block.source_location
    end

    if file and line
      "#<#{self.class}:0x#{self.object_id.to_s(16)}@#{file}:#{line}>"
    else
      "#<#{self.class}:0x#{self.object_id.to_s(16)}>"
    end
  end

  alias_method :inspect, :to_s

  def self.__from_method__(meth)
    obj = __allocate__
    obj.ruby_method = meth

    return obj
  end

  def __yield__(*args, &block)
    # do a block style unwrap..
    if args.size == 1 and args.first.kind_of? Array and args.first.size > 1
      args = args.first
    end

    @ruby_method.call(*args, &block)
  end
end
