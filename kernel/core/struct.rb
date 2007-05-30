class Struct
  include Enumerable

  # TODO: Use fields when RBX kinks are worked out.
  #self.instance_fields = 4
  #ivar_as_index :index => 1, :map => 2, :generated => 3

  # Branch on whether this is a Struct/Struct subclass or a generated anonymous class
  def self.new(*args, &block)
    if @generated
      super
    else
      new_anonymous_class(*args, &block)
    end
  end

  ##
  # Struct class methods

  def self.new_anonymous_class(*args, &block)
    name, *attributes = args.dup

    unless constant = constantize(name)
      attributes.unshift name
    end

    attributes.collect! do |attribute|
      validate_attribute_type(attribute)
    end

    create_anonymous_class(attributes, constant, &block)
  end

  def self.constantize(name)
    return unless String === name
    return name if name =~ /^[A-Z]\w*$/
    raise NameError, "identifier #{name} needs to be a constant"
  end

  def self.validate_attribute_type(attribute)
    unless attribute.respond_to?(:to_sym)
      raise TypeError, "#{attribute} is not a symbol"
    end

    if symbol = attribute.to_sym
      symbol
    else
      raise ArgumentError, "#{attribute} is not a symbol"
    end
  end

  def self.create_anonymous_class(attributes, name = nil, &block)
    klass = Class.new(Struct, &block)
    klass.define_attributes(attributes)
    klass.instance_variable_set(:@generated, true)
    self.const_set(name, klass) if name
    klass
  end

  ##
  # Anonymous class methods

  def self.[](*args, &block)
    new(*args, &block)
  end

  # Expects an array of symbols.  Order matters.
  def self.define_attributes(attributes)
    member_accessor(*attributes)
    @index = attributes
  end

  def self.member_accessor(*attributes)
    attributes.each do |member|
      define_method(member) { self[member] }
      define_method("#{member}=") { |value| self[member] = value }
    end
  end

  def self.members
    @index.map { |i| i.to_s }
  end

  ##
  # Anonymous instance methods

  def initialize(*args, &block)
    @index = self.class.instance_variable_get(:@index)
    @map   = {}

    args.each_with_index do |value, index|
      if member = @index.at(index)
        @map[member] = value
      else
        raise ArgumentError, 'struct size differs' 
      end
    end
  end

  def length
    @index.size
  end
  alias_method :size, :length

  def members
    self.class.members
  end

  def to_a
    @index.map { |member| @map[member] }
  end
  alias_method :values, :to_a

  def values_at(*args)
    args = args.first.to_a if Range === args.first

    indices = args.map do |arg|
      validate_index_type(arg)
    end

    @index.values_at(*indices).map { |member| @map[member] }
  end

  def [](member_or_index)
    @map[validate_member_type(member_or_index)]
  end

  def []=(member_or_index, value)
    @map[validate_member_type(member_or_index)] = value
  end

  def member?(member)
    @index.include?(member.to_sym) rescue false
  end

  def validate_member_type(member)
    if Symbol === member || String === member
      unless member?(sym = member.to_sym)
        raise NameError, "no member '#{member}' in struct" 
      end

      return sym
    else
      @index.at(validate_index_type(member))
    end
  end

  def validate_index_type(index)
    if index.respond_to?(:to_int)
      index = index.to_int
    else
      raise TypeError, "can't convert #{index.class} into Integer"
    end

    if index > size - 1
      raise IndexError, "offset #{index} too large for struct(size:#{size})"
    elsif index < 0
      raise IndexError, "offset #{index} too small for struct(size:#{size})"
    end

    index
  end

  def each(&block)
    to_a.each(&block)
    self
  end

  def each_pair
    raise LocalJumpError unless block_given?

    @index.each do |member|
      yield member, @map[member]
    end
    self
  end

  def ==(other)
    self.class == other.class && members.all? { |member| self[member] == other[member] }
  end

  def inspect
    string = "#<struct #{self.class.name}"

    each_pair do |member, value|
      string << " #{member}=#{value.inspect},"
    end

    string[0...-1] << '>'
  end
  alias_method :to_s, :inspect

  def hash
    @map.hash
  end
end
