module ObjectSpace

  def self._id2ref(id)
    ary = []
    if find_object([:object_id, Integer(id)], ary) > 0
      return ary.first
    end

    return nil
  end

  def self.find_references(obj)
    ary = []
    find_object([:references, obj], ary)
    return ary
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

  def self.define_finalizer(obj, prc=nil, &block)
    prc ||= block

    if obj.equal? prc
      # This is allowed. This is the Rubinius specific API that calls
      # __finalize__ when the object is finalized.
    elsif !prc and obj.respond_to?(:__finalize__)
      # Allowed. Same thing as above
      prc = obj
    elsif !prc or !prc.respond_to?(:call)
      raise ArgumentError, "action must respond to call"
    end

    Rubinius.invoke_primitive :vm_set_finalizer, obj, prc

    [0, prc]
  end

  def self.undefine_finalizer(obj)
    Rubinius.invoke_primitive :vm_set_finalizer, obj, nil
  end

  def self.run_finalizers
  end

  def self.garbage_collect
    GC.start
  end

end
