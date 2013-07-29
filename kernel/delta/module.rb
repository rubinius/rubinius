# -*- encoding: us-ascii -*-

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

      Rubinius::VM.reset_method_cache self, new_name

      if ai = Rubinius::Type.singleton_class_object(self)
        Rubinius.privately do
          ai.singleton_method_added new_name
        end
      else
        method_added new_name
      end

      self
    else
      if ai = Rubinius::Type.singleton_class_object(self)
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
      until vs.top_level_visibility?
        break unless vs.parent
        vs = vs.parent
      end
      vs.method_visibility = :module
    else
      sc = Rubinius::Type.object_singleton_class(self)
      args.each do |meth|
        method_name = Rubinius::Type.coerce_to_symbol meth
        mod, method = lookup_method(method_name)
        sc.method_table.store method_name, method.method, :public
        Rubinius::VM.reset_method_cache self, method_name
        set_visibility method_name, :private
      end
    end

    return self
  end

  def private(*args)
    if args.empty?
      Rubinius::VariableScope.of_sender.method_visibility = :private
    else
      args.each { |meth| set_visibility(meth, :private) }
    end

    self
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

    Rubinius::Type.include_modules_from(self, klass.origin)

    self
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

  private :remove_method, :undef_method, :alias_method,
          :module_function, :append_features, :extend_object,
          :include, :public, :private, :protected,
          :attr_reader, :attr_writer, :attr_accessor
end
