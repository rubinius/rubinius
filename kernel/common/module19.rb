class Module
  def const_defined?(name, search_parents=true)
    @constants.has_key? normalize_const_name(name)
  end
end
