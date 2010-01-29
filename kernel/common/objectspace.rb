module ObjectSpace

  def self._id2ref(id)
    ary = []
    if find_object([:object_id, Integer(id)], ary) > 0
      return ary.first
    end

    return nil
  end

  # @todo rewrite each_object

  # Tryes to handle as much as it can.
  def self.each_object(what=nil, &block)
    return to_enum :each_object, what unless block_given?

    what ||= Object

    unless what.kind_of? Object
      raise TypeError, "class or module required"
    end

    case what
    when Fixnum, Symbol
      return 0
    when TrueClass
      yield true
      return 1
    when FalseClass
      yield false
      return 1
    when NilClass
      yield nil
      return 1
    else
      return find_object([:kind_of, what], block)
    end
  end


  # Finalizer support. Uses WeakRef to detect object death.
  # WeakRef uses the GC to do all the real work.

  @finalizers = Hash.new

  def self.define_finalizer(obj, prc=nil, &block)
    prc ||= block

    if prc.nil? or !prc.respond_to?(:call)
      raise ArgumentError, "action must respond to call"
    end

    @finalizers[obj.object_id] = [WeakRef.new(obj), prc]
    [0, prc]
  end

  def self.undefine_finalizer(obj)
    @finalizers.delete(obj.object_id)
  end

  def self.run_finalizers
    @finalizers.each_pair do |key, val|
      unless val[0].weakref_alive?
        @finalizers.delete key
        val[1].call(key)
      end
    end
  end

  def self.garbage_collect
    GC.start
  end

end
