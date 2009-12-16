module ObjectSpace

  def self._id2ref(id)
    raise NotImplementedError
  end

  # @todo rewrite each_object

  # Tryes to handle as much as it can.
  def self.each_object(what=nil, &block)
    return to_enum :each_object, what unless block_given?

    raise TypeError, "class or module required" if what and not what.is_a? Module

    # Any way to get a list of all objects?
    if what == nil
      raise ArgumentError, "ObjectSpace cannot loop through all objects yet"
    end

    # Finds all classes by recursivly looping through subclasses.
    if what == Class
      return recursive_loop(Object, block) { |a_class| a_class.__subclasses__ }
    end

    # Finds all modules by looping through the constants of all classes (hence
    # __subclasses__) and checking whether those are modules.
    if  what == Module
      return recursive_loop(Object, block) do |a_module|
        # Get all modules that are helt in a_module.constants plus
        # subclasses of a_module if a_module is a class.
        a_module.constants.inject([]) do |list, const|
          begin
            const = a_module.const_get const
            list << const if const.is_a? Module
            list += const.__subclasses__ if const.is_a? Class
          rescue NameError, LoadError # Handles autoloading.
          end
          list
        end
      end
    end

    # This is what MRI does:
    return 0 if [Fixnum, Symbol].include? what

    # Those are singeltons.
    if [TrueClass, FalseClass, NilClass].include? what
      yield what.new
      return 1
    end

    # This list is available.
    if what == Thread
      Thread.list.each(&block)
      return Thread.list.size
    end

    # In the unlikely case that someone would create another instance
    # of GlobalVariables, this wouldn't work.
    if what == Rubinius::GlobalVariables
      yield Rubinius::Globals
      return 1
    end

    # If this is a Singelton, check whether it already has an instance.
    if defined?(Singleton) and what.ancestors.include?(Singleton)
      return 0 unless what.instance_eval { _instantiate? }
      yield what.instance
      return 1
    end

    # Simply loop through all instances of superclass and check.
    # Note: Rescue may be removed as soon as each_object(Object) is supported,
    # if ever.
    begin
      count = 0
      return count unless what.superclass
      each_object(what.superclass) do |obj|
        if obj.is_a? what
          count += 1
          yield obj
        end
      end
      count
    rescue ArgumentError => e
      if e.message =~ /^ObjectSpace/
        e.message = "ObjectSpace doesn't support '#{what}' yet"
      end
      raise e
    end

  end
  

  def self.recursive_loop(start, each_block, skip = [], &grepper)
    list = yield start
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
