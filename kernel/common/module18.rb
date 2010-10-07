class Module
  def const_defined?(name)
    @constants.has_key? normalize_const_name(name)
  end
end
