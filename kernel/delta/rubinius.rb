module Rubinius
  Terminal = STDIN.tty?
  AtExit = []

  def self.open_class_under(name, sup, mod)
    unless mod.kind_of? Module
      raise TypeError, "'#{mod.inspect}' is not a class/module"
    end

    tbl = mod.constants_table
    if !tbl.key?(name)
      # Create the class
      sup = Object unless sup
      obj = Class.new sup
      obj.set_name_if_necessary name, mod
      mod.const_set name, obj
    else
      obj = tbl[name].value
      if obj.kind_of? Autoload
        obj = obj.call
      end

      if obj.kind_of? Class
        if sup and obj.superclass != sup
          raise TypeError, "Superclass mismatch: #{obj.superclass} != #{sup}"
        end
      else
        raise TypeError, "#{name} is not a class"
      end
    end
    return obj
  end

  def self.open_class(name, sup, scope)
    if scope
      under = scope.module
    else
      under = Object
    end

    open_class_under name, sup, under
  end

  def self.open_module_under(name, mod)
    unless mod.kind_of? Module
      raise TypeError, "'#{mod.inspect}' is not a class/module"
    end

    tbl = mod.constants_table
    if !tbl.key?(name)
      # Create the module
      obj = Module.new
      obj.set_name_if_necessary name, mod
      mod.const_set name, obj
    else
      obj = tbl[name].value
      if obj.kind_of? Autoload
        obj = obj.call
      end

      unless obj.kind_of? Module
        raise TypeError, "#{name} is not a class"
      end
    end
    return obj
  end

  def self.open_module(name, scope)
    if scope
      under = scope.module
    else
      under = Object
    end

    open_module_under name, under
  end

  def self.add_defn_method(name, executable, static_scope, vis)
    executable.serial = 1
    executable.scope = static_scope if executable.respond_to? :scope=

    mod = static_scope.for_method_definition

    add_method name, executable, mod, vis
  end

  def self.add_method(name, executable, mod, vis)
    vis ||= :public

    if vis == :module or name == :initialize or name == :initialize_copy
      visibility = :private
    else
      visibility = vis
    end
    cm_vis = CompiledMethod::Visibility.new executable, visibility

    mod.method_table[name] = cm_vis
    Rubinius::VM.reset_method_cache(name)

    mod.module_function name if vis == :module
    mod.method_added name if mod.respond_to? :method_added

    return executable
  end

  def self.add_reader(name, mod, vis)
    normalized = Type.coerce_to_symbol(name)
    add_method normalized, AccessVariable.get_ivar(normalized), mod, vis
  end

  def self.add_writer(name, mod, vis)
    normalized = Type.coerce_to_symbol(name)
    writer_name = "#{normalized}=".to_sym
    add_method writer_name, AccessVariable.set_ivar(normalized), mod, vis
  end

  def self.received_signal(sig)
    Signal.run_handler(sig)
  end
end
