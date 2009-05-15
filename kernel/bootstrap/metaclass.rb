class MetaClass
  # Do not define #methods as an accessor for @methods, use #method_table instead
  #
  attr_reader :attached_instance
  attr_reader :encloser
  attr_reader :instance_fields
  attr_reader :instance_flags
  attr_reader :method_cache
  attr_reader :method_table
  attr_reader :name
  attr_reader :superclass

  def constants_table
    @constants
  end

  def inspect
    "#<MetaClass: #{@attached_instance.inspect}>"
  end

  alias_method :to_s, :inspect
end
