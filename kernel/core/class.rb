# depends on: object.rb

class Class

  protected :object_type
  protected :has_ivars
  protected :needs_cleanup

  def initialize(sclass=Object)
    unless sclass.kind_of?(Class)
      raise TypeError, "superclass must be a Class (#{sclass.class} given)"
    end

    @instance_fields = sclass.instance_fields
    @has_ivar = sclass.has_ivars
    @needs_cleanup = sclass.needs_cleanup
    @object_type = sclass.object_type
    @superclass = sclass

    super()

    mc = self.metaclass
    mc.set_superclass sclass.metaclass

    block = block_given?
    instance_eval(&block) if block
    # add clas to sclass's subclass list, for ObjectSpace.each_object(Class)
    # NOTE: This is non-standard; Ruby does not normally track subclasses
    sclass.__send__ :inherited, self
  end

  def opened_class_cv(cls)
    cls = Object unless cls
    cls.add_subclass(self)

    # Fire all the inherited hooks
    sup = cls
    while sup and sup != Object
      sup.send :inherited, self
      sup = sup.superclass
    end

    # FIXME: We shouldn't have to do this; hook calls should preserve the stack
    self
  end

  # NOTE: The next two methods are not standard Ruby; JRuby implements them, but not public
  def add_subclass_cv(cls)
    @subclasses ||= []
    @subclasses << cls
  end

  def subclasses_cv(descend = false)
    if descend
      subclasses_descend()
    else
      @subclasses.dup
    end
  end

  def subclasses_descend(all = [])
    return unless @subclasses
    @subclasses.each {|cls| all << cls; cls.subclasses_descend(all)}
    all
  end

  def superclass
    cls = direct_superclass
    return nil unless cls
    while cls and cls.kind_of? IncludedModule
      cls = cls.direct_superclass
    end
    return cls
  end
  
  def inherited(name)
  end
  
  def self.after_loaded
    alias_method :opened_class, :opened_class_cv
    alias_method :add_subclass, :add_subclass_cv
    alias_method :subclasses, :subclasses_cv
  end

end

class MetaClass

  ivar_as_index :superclass => 6

  # Called when 'def obj.name' syntax is used in userland
  def attach_method(name, object)
    cur = method_table[name]
    if cur and cur.kind_of? Tuple
      # Override the slot which points to the method, so that we
      # retain visibility.
      cur[1] = object
    else
      method_table[name] = Tuple[:public, object]
    end

    object.inherit_scope MethodContext.current.sender.method
    Rubinius::VM.reset_method_cache(name)
    return object
  end

  def set_superclass(obj)
    @superclass = obj
  end

  def inspect
    "#<MetaClass #{attached_instance.inspect}>"
  end  
end
