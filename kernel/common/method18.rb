# -*- encoding: us-ascii -*-

class Method
  def name
    @name.to_s
  end

  ##
  # Method objects are equal if they have the same body and are bound to the
  # same object.

  def ==(other)
    other.class == Method and
      @receiver.equal?(other.receiver) and
      @executable == other.executable
  end
end

class UnboundMethod
  def name
    @name.to_s
  end

  ##
  # Convenience method for #binding to the given receiver object and calling
  # it with the optionally supplied arguments.

  def call_on_instance(obj, *args, &block)
    unless Rubinius::Type.object_kind_of?(obj, @defined_in)
      raise TypeError, "Must be bound to an object of kind #{@defined_in}"
    end

    if Rubinius::Type.singleton_class_object(@defined_in)
      raise TypeError, "illegal attempt to rebind a singleton method to another object"
    end

    @executable.invoke(@name, @defined_in, obj, args, block)
  end
end
