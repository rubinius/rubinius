class Class

  def initialize(sclass=Object, name=nil, under=nil)
    raise TypeError, "already initialized class" if @instance_type
    raise TypeError, "can't make subclass of Class" if sclass.equal?(Class)

    set_superclass sclass

    # Things (rails) depend on the fact that a normal class is in the constant
    # table and have a name BEFORE inherited is run.
    under.const_set name, self if under

    if sclass
      Rubinius.privately do
        sclass.inherited self
      end
    end
    super()
  end
  private :initialize

  ##
  # Specialized initialize_copy because Class needs additional protection
  def initialize_copy(other)
    # The code in super (Module#initialize_copy) will duplicate
    # the method table. Therefore do this check here to see whether
    # we've already initialized this class before continuing.
    unless @method_table == other.method_table
      raise TypeError, "already initialized class"
    end

    @module_name = nil
    super
  end

  def inherited(name)
  end
  private :inherited

  def superclass
    cls = direct_superclass
    unless cls
      return nil if self == BasicObject
      raise TypeError, "uninitialized class"
    end

    while cls and cls.kind_of? Rubinius::IncludedModule
      cls = cls.direct_superclass
    end

    return cls
  end
end
