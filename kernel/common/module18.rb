# -*- encoding: us-ascii -*-

class Module
  private :remove_class_variable

  def const_get(name)
    Rubinius::Type.const_get(self, name)
  end

  def const_defined?(name)
    name = Rubinius::Type.coerce_to_constant_name name
    return true if @constant_table.has_name? name

    # a silly special case
    if self.equal? Object
      mod = direct_superclass
      while mod.kind_of? Rubinius::IncludedModule
        return true if mod.constant_table.has_name? name
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

  # Install a new Autoload object into the constants table
  # See kernel/common/autoload.rb
  def autoload(name, path)
    unless path.kind_of? String
      raise TypeError, "autoload filename must be a String"
    end

    raise ArgumentError, "empty file name" if path.empty?

    return if Rubinius::CodeLoader.feature_provided?(path)

    name = Rubinius::Type.coerce_to_constant_name name

    if entry = @constant_table.lookup(name)
      if entry.constant.kind_of? Autoload
        # If there is already an Autoload here, just change the path to
        # autoload!
        entry.constant.set_path(path)
      else
        # Trying to register an autoload for a constant that already exists,
        # ignore the request entirely.
      end

      return
    end

    constant_table.store(name, Autoload.new(name, self, path), :public)
    Rubinius.inc_global_serial
    return nil
  end

  def constants
    tbl = Rubinius::LookupTable.new

    @constant_table.each do |name, constant, visibility|
      tbl[name] = true
    end

    current = self.direct_superclass

    while current and current != Object
      current.constant_table.each do |name, constant, visibility|
        tbl[name] = true
      end

      current = current.direct_superclass
    end

    # special case: Module.constants returns Object's constants
    if self.equal? Module
      Object.constant_table.each do |name, constant, visibility|
        tbl[name] = true
      end
    end

    Rubinius::Type.convert_to_names tbl.keys
  end

  ##
  # Returns an UnboundMethod corresponding to the given name. The name will be
  # searched for in this Module as well as any included Modules or
  # superclasses. The UnboundMethod is populated with the method name and the
  # Module that the method was located in.
  #
  # Raises a TypeError if the given name.to_sym fails and a NameError if the
  # name cannot be located.

  def instance_method(name)
    name = Rubinius::Type.coerce_to_symbol name

    mod = self
    while mod
      if entry = mod.method_table.lookup(name)
        break if entry.visibility == :undef

        if meth = entry.method
          if meth.kind_of? Rubinius::Alias
            mod =  meth.original_module
            meth = meth.original_exec
          end

          mod = mod.module if mod.class == Rubinius::IncludedModule

          return UnboundMethod.new(mod, meth, self, name)
        end
      end

      mod = mod.direct_superclass
    end

    raise NameError, "undefined method `#{name}' for #{self}"
  end

end
