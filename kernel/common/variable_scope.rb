class VariableScope
  attr_reader :parent
  attr_reader :block

  # CompiledMethod this scope is for.
  #
  attr_reader :method

  def self=(obj)
    @self = obj
  end

  def dynamic_locals
    @dynamic_locals ||= LookupTable.new
  end

  def set_eval_local(name, val)
    @parent.dynamic_locals[name] = val
  end

  def get_eval_local(name)
    scope = @parent
    while scope
      if scope.dynamic_locals.key? name
        return scope.dynamic_locals[name]
      end

      scope = scope.parent
    end

    nil
  end

  def exitted?
    @exitted
  end
end
