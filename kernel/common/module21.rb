# -*- encoding: us-ascii -*-

class Module

  def const_get(name, inherit = true)
    Rubinius::Type.const_get(self, name, inherit)
  end

  def const_defined?(name, search_parents=true)
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

    if search_parents
      current = self.direct_superclass
      while current
        return true if current.constant_table.has_name? name
        current = current.direct_superclass
      end

      if instance_of?(Module)
        return true if Object.constant_table.has_name? name
      end
    end

    return false
  end

  def attr(*attributes)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    if attributes.size == 2 && (attributes.last.is_a?(TrueClass) || attributes.last.is_a?(FalseClass))
      name = attributes.first
      Rubinius.add_reader name, self, vis
      Rubinius.add_writer name, self, vis if attributes.last
    else
      attributes.each do |name|
        if name.is_a?(Symbol) || (name.respond_to?(:to_str) && name.to_str.is_a?(String) && !name.to_str.empty?)
          Rubinius.add_reader(name, self, vis)
        else
          raise TypeError, "#{name.inspect} is not a symbol"
        end
      end
    end

    return nil
  end

  private :attr

  # Install a new Autoload object into the constants table
  # See kernel/common/autoload.rb
  def autoload(name, path)
    path = Rubinius::Type.coerce_to_path(path)

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

    autoload_contant = Autoload.new(name, self, path)
    constant_table.store(name, autoload_contant, :public)
    if self == Kernel
      Object.singleton_class.constant_table.store(name, autoload_contant, :public)
    end

    Rubinius.inc_global_serial
    return nil
  end

  def constants(all=undefined)
    tbl = Rubinius::LookupTable.new

    # When calling `constants` on "normal" classes the result does not include
    # their own class name. BasicObject is a special case in this regard.
    tbl[:BasicObject] = true if self == BasicObject

    @constant_table.each do |name, constant, visibility|
      tbl[name] = true unless visibility == :private
    end

    if all
      current = self.direct_superclass

      while current and current != Object
        current.constant_table.each do |name, constant, visibility|
          tbl[name] = true unless visibility == :private
        end

        current = current.direct_superclass
      end
    end

    # special case: Module.constants returns Object's constants
    if self.equal?(Module) && undefined.equal?(all)
      Object.constant_table.each do |name, constant, visibility|
        tbl[name] = true unless visibility == :private
      end
    end

    Rubinius::Type.convert_to_names tbl.keys
  end

  def private_constant(*names)
    unknown_constants = names - @constant_table.keys
    if unknown_constants.size > 0
      raise NameError, "#{unknown_constants.size > 1 ? 'Constants' : 'Constant'} #{unknown_constants.map{|e| "#{name}::#{e}"}.join(', ')} undefined"
    end
    names.each do |name|
      entry = @constant_table.lookup(name)
      entry.visibility = :private
    end
  end

  def public_constant(*names)
    unknown_constants = names - @constant_table.keys
    if unknown_constants.size > 0
      raise NameError, "#{unknown_constants.size > 1 ? 'Constants' : 'Constant'} #{unknown_constants.map{|e| "#{name}::#{e}"}.join(', ')} undefined"
    end
    names.each do |name|
      entry = @constant_table.lookup(name)
      entry.visibility = :public
    end
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
