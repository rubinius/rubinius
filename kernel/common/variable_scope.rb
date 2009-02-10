class VariableScope
  def self=(obj)
    @self = obj
  end

  def dynamic_locals
    @dynamic_locals ||= LookupTable.new
  end
end
