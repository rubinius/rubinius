class Module
  def alias_method(new_name, current_name)
    new_name = Type.coerce_to_symbol(new_name)
    current_name = Type.coerce_to_symbol(current_name)
    mod, entry = lookup_method(current_name)

    if entry
      # If we're aliasing a method we contain, just reference it directly, no
      # need for the alias wrapper
      if mod == self
        @method_table.store new_name, entry.method, entry.visibility
      else
        @method_table.alias new_name, entry.visibility, current_name,
                            entry.method, mod
      end

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
      vs = Rubinius::VariableScope.of_sender
      if scr = vs.method.scope.script
        if scr.eval? and scr.eval_binding
          scr.eval_binding.variables.method_visibility = :module
        end
      end

      Rubinius::VariableScope.of_sender.method_visibility = :module
    else
      mc = Rubinius.object_metaclass(self)
      args.each do |meth|
        method_name = Type.coerce_to_symbol meth
        method = find_method_in_hierarchy(method_name)
        mc.method_table.store method_name, method.method, :public
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

      mod.send :append_features, self
      mod.send :included, self
    end
  end

  # Called when this Module is being included in another Module.
  # This may be overridden for custom behaviour. The default
  # is to add constants, instance methods and module variables
  # of this Module and all Modules that this one includes to +klass+.
  #
  # See also #include.
  #
  def append_features(klass)
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
