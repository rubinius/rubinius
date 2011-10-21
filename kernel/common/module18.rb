class Module
  def const_defined?(name)
    name = normalize_const_name(name)
    return true if @constant_table.has_key? name

    # a silly special case
    if self.equal? Object
      mod = direct_superclass
      while mod.kind_of? Rubinius::IncludedModule
        return true if mod.constant_table.has_key? name
        mod = mod.direct_superclass
      end
    end

    return false
  end

  def attr(name, writeable = false)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    Rubinius.add_reader name, self, vis
    Rubinius.add_writer name, self, vis if writeable

    return nil
  end

  private :attr
end
