class Module
  def const_defined?(name, search_parents=true)
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

  def name
    @module_name ? @module_name.to_s : nil
  end

  def attr(*attributes)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    if attributes.size == 2 && (attributes.last.is_a?(TrueClass) || attributes.last.is_a?(FalseClass))
      name = attributes.first
      Rubinius.add_reader name, self, vis
      Rubinius.add_writer name, self, vis if attributes.last
    else
      attributes.each do |name|
        raise TypeError, "#{name} is not a symbol" unless name.respond_to(:to_str)
        Rubinius.add_reader(name, self, vis)
      end
    end

    return nil
  end
end
