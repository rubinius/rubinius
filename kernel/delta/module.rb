class Module
  #--
  # HACK: This should work after after the bootstrap is loaded,
  # but it seems to blow things up, so it's only used after
  # core is loaded. I think it's because the bootstrap Class#new
  # doesn't use a privileged send.
  #++

  def __method_added__(name)
    if name == :initialize
      private :initialize
    end

    method_added(name) if self.respond_to? :method_added
  end

  def alias_method(new_name, current_name)
    new_name = Type.coerce_to_symbol(new_name)
    current_name = Type.coerce_to_symbol(current_name)
    entry = find_method_in_hierarchy(current_name)
    if entry
      @method_table.store new_name, entry.method, entry.visibility
      Rubinius::VM.reset_method_cache(new_name)
    else
      if ai = __metaclass_object__
        raise NameError, "Unable to find '#{current_name}' for object #{ai.inspect}"
      else
        thing = kind_of?(Class) ? "class" : "module"
        raise NameError, "undefined method `#{current_name}' for #{thing} `#{self.name}'"
      end
    end
  end

  def module_function(*args)
    if args.empty?
      # NOTE: Previously, we had special case code for whether this was
      # called inside an eval. Investigate if problems arise. Otherwise
      # remove this comment if all specs are passing

      Rubinius::VariableScope.of_sender.method_visibility = :module
    else
      mc = self.metaclass
      args.each do |meth|
        method_name = Type.coerce_to_symbol meth
        method = find_method_in_hierarchy(method_name)
        mc.method_table.store method_name, method.method, :public
        set_visibility method_name, :private
      end
    end

    return self
  end

  def include(*modules)
    modules.reverse_each do |mod|
      if !mod.kind_of?(Module) or mod.kind_of?(Class)
        raise TypeError, "wrong argument type #{mod.class} (expected Module)"
      end

      raise ArgumentError, "cyclic include detected" if mod == self

      if ancestors.include? mod
        ancestor = superclass_chain.find do |m|
          m.module == mod if m.kind_of?(Rubinius::IncludedModule)
        end
        ancestor.module.included_modules.each do |included_mod|
          included_mod.send :append_features, ancestor
          included_mod.send :included, ancestor
        end
      else
        mod.send :append_features, self
      end

      mod.send :included, self
    end
  end

  def private(*args)
    if args.empty?
      Rubinius::VariableScope.of_sender.method_visibility = :private
      return
    end

    args.each { |meth| set_visibility(meth, :private) }
  end

  # Called when this Module is being included in another Module.
  # This may be overridden for custom behaviour, but the default
  # is to add constants, instance methods and module variables
  # of this Module and all Modules that this one includes to the
  # includer Module, which is passed in as the parameter +other+.
  #
  # See also #include.
  #
  def append_features(other)
    hierarchy = other.ancestors

    superclass_chain.reverse_each do |ancestor|
      if ancestor.instance_of? Rubinius::IncludedModule and
          not hierarchy.include? ancestor.module
        Rubinius::IncludedModule.new(ancestor.module).attach_to other
      end
    end

    Rubinius::IncludedModule.new(self).attach_to other
  end

  def attr_reader(*names)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    names.each { |name| Rubinius.add_reader name, self, vis }

    return nil
  end

  def attr_writer(*names)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    names.each { |name| Rubinius::add_writer name, self, vis }

    return nil
  end

  def attr_accessor(*names)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    names.each do |name|
      Rubinius.add_reader name, self, vis
      Rubinius.add_writer name, self, vis
    end

    return nil
  end

  def attr(name,writeable=false)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    Rubinius.add_reader name, self, vis
    Rubinius.add_writer name, self, vis if writeable

    return nil
  end

  private :alias_method
end
