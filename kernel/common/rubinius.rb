module Rubinius
  def self.instance_variable_validate(name)
    # adapted from rb_to_id
    case name
    when String
      return name.to_sym if name[0] == ?@
    when Symbol
      return name if name.is_ivar?
    when Fixnum
      raise ArgumentError, "#{name.inspect} is not a symbol"
    else
      name = Type.coerce_to(name, String, :to_str)
      return name.to_sym if name[0] == ?@
    end

    raise NameError, "`#{name}' is not allowed as an instance variable name"
  end
end
