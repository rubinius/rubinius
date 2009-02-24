class VariableScope
  attr_reader :parent
  attr_reader :block

  def self=(obj)
    @self = obj
  end

  def dynamic_locals
    @dynamic_locals ||= LookupTable.new
  end

  def set_eval_local(name, val)
    dynamic_locals[name] = val
  end

  def get_eval_local(name)
    dynamic_locals[name]
  end
end
