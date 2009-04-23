class WeakRef

  class RefError < RuntimeError; end

  def initialize(object)
    @wr = Rubinius::Tuple.create_weakref(object)
  end

  def inspect
    "#<WeakRef:0x#{object_id.to_s(16)} object=#{@wr.at(0).inspect}>"
  end

  def weakref_alive?
    !@wr.at(0).nil?
  end

  def object
    obj = @wr.at(0)
    raise RefError, "Object has been collected as garbage" unless obj
    return obj
  end

end
