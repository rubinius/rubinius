class Struct
  def each(&block)
    return to_enum :each unless block_given?
    values.each(&block)
  end

  def each_pair
    return to_enum :each_pair unless block_given?
    _attrs.map { |var| yield var, instance_variable_get("@#{var}") }
  end
end