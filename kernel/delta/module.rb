class Module
  def alias_method(new_name, current_name)
    new_name = Rubinius::Type.coerce_to_symbol(new_name)
    current_name = Rubinius::Type.coerce_to_symbol(current_name)
    mod, entry = lookup_method(current_name, true, false)

    if entry
      # If we're aliasing a method we contain, just reference it directly, no
      # need for the alias wrapper
      #
      # The 'and entry.method' is there to force us to use the alias code
      # when the original method exists only to change the visibility of
      # a parent method.
      if mod == self and entry.method
        @method_table.store new_name, entry.method, entry.visibility
      else
        @method_table.alias new_name, entry.visibility, current_name,
                            entry.method, mod
      end

      Rubinius::VM.reset_method_cache(new_name)
    else
      if Rubinius::Type.object_kind_of?(self, Class) and
         ai = Rubinius::Type.singleton_class_object(self)
        raise NameError, "Unable to find '#{current_name}' for object #{ai.inspect}"
      else
        thing = Rubinius::Type.object_kind_of?(self, Class) ? "class" : "module"
        raise NameError, "undefined method `#{current_name}' for #{thing} `#{self.name}'"
      end
    end
  end

  def module_function(*args)
    if args.empty?
      vs = Rubinius::VariableScope.of_sender
      if scr = vs.method.scope.script
        if scr.eval? and scr.eval_binding
          scr.eval_binding.variables.method_visibility = :module
        end
      end

      Rubinius::VariableScope.of_sender.method_visibility = :module
    else
      sc = Rubinius::Type.object_singleton_class(self)
      args.each do |meth|
        method_name = Rubinius::Type.coerce_to_symbol meth
        mod, method = lookup_method(method_name)
        sc.method_table.store method_name, method.method, :public
        Rubinius::VM.reset_method_cache method_name
        set_visibility method_name, :private
      end
    end

    return self
  end

  def private(*args)
    if args.empty?
      Rubinius::VariableScope.of_sender.method_visibility = :private
      return
    end

    args.each { |meth| set_visibility(meth, :private) }
  end

  # Invokes <code>Module#append_features</code> and
  # <code>Module#included</code> on each argument, passing in self.
  #
  def include(*modules)
    modules.reverse_each do |mod|
      if !mod.kind_of?(Module) or mod.kind_of?(Class)
        raise TypeError, "wrong argument type #{mod.class} (expected Module)"
      end

      Rubinius.privately do
        mod.append_features self
      end

      Rubinius.privately do
        mod.included self
      end
    end
    self
  end

  # Add all constants, instance methods and module variables
  # of this Module and all Modules that this one includes to +klass+
  #
  # This method is aliased as append_features as the default implementation
  # for that method. Kernel#extend calls this method directly through
  # Module#extend_object, because Kernel#extend should not use append_features.
  def include_into(klass)
    unless klass.kind_of? Module
      raise TypeError, "invalid argument class #{klass.class}, expected Module"
    end

    # check other.frozen
    # taint other from self

    insert_at = klass
    mod = self
    changed = false

    while mod

      # Check for a cyclic include
      if mod == klass
        raise ArgumentError, "cyclic include detected"
      end

      # Try and detect check_mod in klass's heirarchy, and where.
      #
      # I (emp) tried to use Module#< here, but we need to also know
      # where in the heirarchy the module is to change the insertion point.
      # Since Module#< doesn't report that, we're going to just search directly.
      #
      superclass_seen = false
      add = true

      k = klass.direct_superclass
      while k
        if k.kind_of? Rubinius::IncludedModule
          # Oh, we found it.
          if k == mod
            # ok, if we're still within the directly included modules
            # of klass, then put future things after mod, not at the
            # beginning.
            insert_at = k unless superclass_seen
            add = false
            break
          end
        else
          superclass_seen = true
        end

        k = k.direct_superclass
      end

      if add
        if mod.kind_of? Rubinius::IncludedModule
          original_mod = mod.module
        else
          original_mod = mod
        end

        im = Rubinius::IncludedModule.new(original_mod).attach_to insert_at
        insert_at = im

        changed = true
      end

      mod = mod.direct_superclass
    end

    if changed
      method_table.each do |meth, obj, vis|
        Rubinius::VM.reset_method_cache meth
      end
    end

    return self
  end

  # Called when this Module is being included in another Module.
  # This may be overridden for custom behaviour. The default
  # is to add constants, instance methods and module variables
  # of this Module and all Modules that this one includes to +klass+.
  #
  # See also #include.
  #
  alias_method :append_features, :include_into

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

  private :alias_method, :attr_reader, :attr_writer, :attr_accessor
end
