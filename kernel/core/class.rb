# depends on: object.rb

class Class

  def self.new(sclass=Object)
    raise TypeError, "superclass must be a Class (#{sclass.class.name} given)" unless sclass.kind_of?(Class)

    obj = Rubinius.class_constitute(sclass, nil)
    block = block_given?
    obj.class_eval(&block) if block
    # add clas to sclass's subclass list, for ObjectSpace.each_object(Class)
    # NOTE: This is non-standard; Ruby does not normally track subclasses
    sclass.inherited(obj) if sclass.respond_to?(:inherited)
    obj
  end

  def opened_class_cv(cls)
    cls = Object unless cls
    cls.add_subclass(self)

    # Fire all the inherited hooks
    sup = cls
    while sup and sup != Object
      sup.inherited(self) if sup.respond_to?(:inherited)
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
  
  def inspect
    if name
      name
    else
      super.inspect
    end
  end
  
  def self.after_loaded
    alias_method :opened_class, :opened_class_cv
    alias_method :add_subclass, :add_subclass_cv
    alias_method :subclasses, :subclasses_cv
  end

end

class MetaClass
  def inspect
    "#<MetaClass #{attached_instance.inspect}>"
  end  
end
