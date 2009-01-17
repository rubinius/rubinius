class Class
  attr_accessor :instance_fields
  attr_accessor :instance_type

  attr_reader :instance_flags
end

# TODO: is this obsolete?
class RuntimePrimitive < Executable
  attr_writer :variable

  def initialize(prim_name)
    @primitive = prim_name
    @serial = 0
  end
end
