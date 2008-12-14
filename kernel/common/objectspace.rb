# depends on: module.rb

module ObjectSpace

  # HACK: Tryes to handle as much as it can.
  # Any way to get a list of all instances? Or of all objects?
  def self.each_object(what = nil, &block)
    # Now this will become ugly.
    case

    when (what and not what.is_a? Module)
      raise TypeError, "class or module required"

    when what == Class
      recursive_loop(Object, block) { |a_class| a_class.__subclasses__ }

    when what == Module
      recursive_loop(Object, block) do |a_module|
        (a_module.constants.inject([]) do |list, const|
          begin
            const = a_module.const_get const
            list << const if const.is_a? Module
            list += const.__subclasses__ if const.is_a? Class
          rescue NameError, LoadError # Handles autoloading.
          end
          list
        end).uniq
      end

    when what == Fixnum
      (Platform::Fixnum.MIN .. Platform::Fixnum.MAX).each(&block)

    when [TrueClass, FalseClass, NilClass].include?(what)
      block.call what.new
      return 1

    when what == GlobalVariables
      block.call(Globals)
      return 1

    when (defined?(Singleton) and what.ancestors.include?(Singleton))
      return 0 unless what.instance_eval { _instantiate? }
      block.call(what.instance)
      return 1

    when what.is_a?(MetaClass)
      block.call(what.attached_instance)
      return 1

    when what.respond_to?(:each_instance)
      return what.each_instance(&block)

    when what.is_a?(Enumerable)
      return what.inject(0) { |o| block.call(o) }

    when nil
      raise ArgumentError, "ObjectSpace cannot loop through all objects yet"

    else
      # Remove the following line when each_object(nil) is implemented.
      raise ArgumentError, "ObjectSpace doesn't support '#{what}' yet"
      count = 0
      each_object do |obj|
        if obj.is_a? what
          count += 1
          block.call(obj)
        end
      end
      count

    end
  end
  
  protected

  def self.recursive_loop(start, each_block, skip = [], &grepper)
    list = grepper.call(start)
    list -= skip
    list.inject(0) do |count, element|
      unless skip.include? element
        each_block.call(element)
        skip << element
        count += 1 + recursive_loop(element, each_block, skip, &grepper)
      end
      count
    end
  end

  public



  # Finalizer support. Uses WeakRef to detect object death.
  # WeakRef uses the GC to do all the real work.

  @finalizers = Hash.new

  def self.define_finalizer(obj, prc=nil, &block)
    prc ||= block

    if prc.nil? or !prc.respond_to?(:call)
      raise ArgumentError, "action must respond to call"
    end

    @finalizers[obj.object_id] = [WeakRef.new(obj), prc]
    return nil
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
