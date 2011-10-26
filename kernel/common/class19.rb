class Class
  ##
  # Returns the Class object that this Class inherits from. Included Modules
  # are not considered for this purpose.

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
