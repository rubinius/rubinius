# These methods are overriden by lib/1.8.7/... or lib/1.9/...
class Struct
  ##
  # call-seq:
  #   struct.each {|obj| block }  => struct
  #
  # Calls <em>block</em> once for each instance variable, passing the value
  # as a parameter.
  #
  #    Customer = Struct.new(:name, :address, :zip)
  #    joe = Customer.new("Joe Smith", "123 Maple, Anytown NC", 12345)
  #    joe.each {|x| puts(x) }
  #
  # <em>produces:</em>
  #
  #    Joe Smith
  #    123 Maple, Anytown NC
  #    12345

  def each(&block)
    return values.each(&block)
  end

  ##
  # call-seq:
  #   struct.each_pair {|sym, obj| block }     => struct
  #
  # Calls <em>block</em> once for each instance variable, passing the name
  # (as a symbol) and the value as parameters.
  #
  #    Customer = Struct.new(:name, :address, :zip)
  #    joe = Customer.new("Joe Smith", "123 Maple, Anytown NC", 12345)
  #    joe.each_pair {|name, value| puts("#{name} => #{value}") }
  #
  # <em>produces:</em>
  #
  #    name => Joe Smith
  #    address => 123 Maple, Anytown NC
  #    zip => 12345

  def each_pair
    _attrs.map { |var| yield var, instance_variable_get("@#{var}") }
  end
end