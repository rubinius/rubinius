class Module
  def const_defined?(name)
    @constant_table.has_key? normalize_const_name(name)
  end
end
