# -*- encoding: us-ascii -*-

class Module
  private :remove_class_variable

  def const_get(name)
    name = Rubinius::Type.coerce_to_constant_name name

    current, constant = self, undefined

    while current
      constant = current.constant_table.fetch name, undefined
      unless constant.equal?(undefined)
        constant = constant.call(current) if constant.kind_of?(Autoload)
        return constant
      end

      current = current.direct_superclass
    end

    if instance_of?(Module)
      constant = Object.constant_table.fetch name, undefined
      unless constant.equal?(undefined)
        constant = constant.call(current) if constant.kind_of?(Autoload)
        return constant
      end
    end

    const_missing(name)
  end

  def const_defined?(name)
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

  def constants
    tbl = Rubinius::LookupTable.new

    @constant_table.each do |name, val|
      tbl[name] = true
    end

    current = self.direct_superclass

    while current and current != Object
      current.constant_table.each do |name, val|
        tbl[name] = true unless tbl.has_key? name
      end

      current = current.direct_superclass
    end

    # special case: Module.constants returns Object's constants
    if self.equal? Module
      Object.constant_table.each do |name, val|
        tbl[name] = true unless tbl.has_key? name
      end
    end

    Rubinius::Type.convert_to_names tbl.keys
  end
end
