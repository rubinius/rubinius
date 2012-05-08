# -*- encoding: us-ascii -*-

class Module

  def const_get(name, inherit = true)
    name = Rubinius::Type.coerce_to_constant_name name

    current, constant = self, undefined

    while current
      constant = current.constant_table.fetch name, undefined
      unless constant.equal?(undefined)
        constant = constant.call if constant.kind_of?(Autoload)
        return constant
      end

      unless inherit
        return const_missing(name)
      end

      current = current.direct_superclass
    end

    if instance_of?(Module)
      constant = Object.constant_table.fetch name, undefined
      unless constant.equal?(undefined)
        constant = constant.call if constant.kind_of?(Autoload)
        return constant
      end
    end

    const_missing(name)
  end

  def const_defined?(name, search_parents=true)
    name = Rubinius::Type.coerce_to_constant_name name
    return true if @constant_table.has_key? name

    # a silly special case
    if self.equal? Object
      mod = direct_superclass
      while mod.kind_of? Rubinius::IncludedModule
        return true if mod.constant_table.has_key? name
        mod = mod.direct_superclass
      end
    end

    if search_parents
      current = self.direct_superclass
      while current
        return true if current.constant_table.has_key? name
        current = current.direct_superclass
      end

      if instance_of?(Module)
        return true if Object.constant_table.has_key? name
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

    if existing = @constant_table[name]
      if existing.kind_of? Autoload
        # If there is already an Autoload here, just change the path to
        # autoload!
        existing.set_path(path)
      else
        # Trying to register an autoload for a constant that already exists,
        # ignore the request entirely.
      end

      return
    end

    constant_table[name] = Autoload.new(name, self, path)
    Rubinius.inc_global_serial
    return nil
  end

  def constants(all=undefined)
    tbl = Rubinius::LookupTable.new

    # When calling `constants` on "normal" classes the result does not include
    # their own class name. BasicObject is a special case in this regard.
    tbl[:BasicObject] = true if self == BasicObject

    @constant_table.each do |name, val|
      tbl[name] = true
    end

    if all
      current = self.direct_superclass

      while current and current != Object
        current.constant_table.each do |name, val|
          tbl[name] = true unless tbl.has_key? name
        end

        current = current.direct_superclass
      end
    end

    # special case: Module.constants returns Object's constants
    if self.equal?(Module) && all.equal?(undefined)
      Object.constant_table.each do |name, val|
        tbl[name] = true unless tbl.has_key? name
      end
    end

    Rubinius.convert_to_names tbl.keys
  end

  def private_constant(*names)
  end

  def public_constant(*names)
  end
end
