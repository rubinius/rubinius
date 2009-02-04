# depends on: io.rb

module Rubinius
  Terminal = STDIN.tty?

  def self.add_defn_method(name, executable, static_scope, vis)
    executable.serial = 1
    executable.scope = static_scope if executable.respond_to? :scope=

    mod = static_scope.module

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
end
