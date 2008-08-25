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
    new_name = normalize_name(new_name)
    current_name = normalize_name(current_name)
    meth = find_method_in_hierarchy(current_name)
    # We valid +meth+ because all hell can break loose if method_table has unexpected
    # objects in it.
    if meth
      if meth.kind_of? Tuple
        meth = meth.dup

        if !meth[0].kind_of?(Symbol) or
             not (meth[1].kind_of?(CompiledMethod) or meth[1].kind_of?(AccessVarMethod))
          raise TypeError, "Invalid object found in method_table while attempting to alias '#{current_name}'"
        end

      else
        unless meth.kind_of? Executable or
               meth.kind_of? CompiledMethod::Visibility then
          meth.__show__
          raise TypeError, "Invalid object found in method_table while attempting to alias '#{current_name}' #{meth.inspect}"
        end
      end
      method_table[new_name] = meth
      Rubinius::VM.reset_method_cache(new_name)
    else
      if self.kind_of? MetaClass
        raise NameError, "Unable to find '#{current_name}' for object #{self.attached_instance.inspect}"
      else
        thing = self.kind_of?(Class) ? "class" : "module"
        raise NameError, "undefined method `#{current_name}' for #{thing} `#{self.name}'"
      end
    end
  end

  def module_function(*args)
    if args.empty?
      ctx = MethodContext.current.sender
      block_env = ctx.env if ctx.kind_of?(BlockContext)
      # Set the method_scope in the home context if this is an eval
      ctx = block_env.home_block if block_env and block_env.from_eval?
      ctx.method_scope = :module
    else
      mc = self.metaclass
      args.each do |meth|
        method_name = normalize_name meth
        method = find_method_in_hierarchy(method_name)
        mc.method_table[method_name] = method.dup
        mc.set_visibility method_name, :public
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

      next if ancestors.include? mod

      mod.send(:append_features, self)
      mod.send(:included, self)
    end
  end

  def private(*args)
    if args.empty?
      MethodContext.current.sender.method_scope = :private
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
      if ancestor.instance_of? IncludedModule and not hierarchy.include? ancestor.module
        IncludedModule.new(ancestor.module).attach_to other
      end
    end

    IncludedModule.new(self).attach_to other
  end

  def attr_reader(*names)
    names.each do |name|
      method_symbol = reader_method_symbol(name)
      access_method = AccessVarMethod.get_ivar(attribute_symbol(name), normalize_name(name))
      method_table[method_symbol] = access_method
    end

    return nil
  end

  def attr_writer(*names)
    names.each do |name|
      method_symbol = writer_method_symbol(name)
      access_method = AccessVarMethod.set_ivar(attribute_symbol(name), normalize_name("#{name}="))
      method_table[method_symbol] = access_method
    end

    return nil
  end

  def attr_accessor(*names)
    names.each do |name|
      attr(name,true)
    end

    return nil
  end

  def attach_function(name, a3, a4, a5=nil)
    if a5
      mname = a3
      args = a4
      ret = a5
    else
      mname = name.to_sym
      args = a3
      ret = a4
    end

    func = FFI.create_function @ffi_lib, name, args, ret

    raise FFI::NotFoundError.new(name, @ffi_lib) unless func

    metaclass.method_table[mname] = func
    return func
  end

  private :alias_method
end
