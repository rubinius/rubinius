# -*- encoding: us-ascii -*-

class Class
  def initialize(sclass=Object, name=nil, under=nil)
    raise TypeError, "already initialized class" if @instance_type

    set_superclass sclass

    # Things (rails) depend on the fact that a normal class is in the constant
    # table and have a name BEFORE inherited is run.
    under.const_set name, self if under

    super()
    if sclass
      Rubinius.privately do
        sclass.inherited self
      end
    end
  end
  private :initialize

  def superclass
    cls = direct_superclass
    raise TypeError, "uninitialized class" unless cls

    while cls and cls.kind_of? Rubinius::IncludedModule
      cls = cls.direct_superclass
    end

    return cls
  end
end
