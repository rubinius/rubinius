class Tuple
  attr_reader :size
  
  def self.[](*args)
    tuple = new(0)
    args.each { |obj| tuple.send :append, obj }
    tuple
  end
  
  def initialize(num)
    @size = @index = 0
    num.times { append nil }
  end
  
  # I'm not sure if these are 100% the right semantics. Please verify. -- flgr
  def copy_from(other_tuple, start_idx = 0)
    idx = 0
    other_tuple.each do |value|
      append(value) if idx >= start_idx
      idx += 1
    end
  end
  
  def at(index)
    guard(index)
    locate index
    @current.value if @current
  end
  alias :[] :at
  
  def put(index, value)
    guard(index)
    locate index
    @current.value = value if @current
  end
  alias :[]= :put
  
  def each
    return if empty?
    reset
    loop do
      yield @current.value
      break unless forward
    end
    self
  end
    
  def empty?
    size == 0
  end
  
  def to_s
    "#<Tuple:0x#{object_id.to_s(16)} #{fields} elements>"
  end

  def inspect
    str = "#<Tuple"
    if fields != 0
      str << ": #{join(", ", :inspect)}"
    end
    str << ">"
    return str
  end
  
  def join(sep, meth=:to_s)
    join_upto(sep, fields, meth)
  end
  
  def join_upto(sep, count, meth=:to_s)
    str = ""
    return str if count == 0 or empty?
    count = fields if count >= fields
    count -= 1
    i = 0
    while i < count
      str << at(i).__send__(meth)
      str << sep.dup
      i += 1
    end
    str << at(count).__send__(meth)
    return str
  end
  
  def shift
    return self unless @head and @head.after
    @head = @head.after
    @head.before = nil
    @size -= 1
    reset
    self
  end
  
  def shifted(num)
    tuple = Tuple.new(num + @size)
    @size.times { |i| tuple.put num + i, at(i) }
    tuple
  end
  
  def to_a
    ary = []
    each do |ent|
      ary << ent unless ent.nil?
    end
    return ary
  end
  
  alias :fields :size
  alias :length :size
  
  private
  
  def guard(index)
    raise InvalidIndex, "Index must be positive: attempted #{index} for #{self}" if index < 0
    raise InvalidIndex, "Index must be less than size: attempted #{index} for #{self}" if index >= size
  end
  
  def reset
    @index = 0
    @current = @head
  end
  
  def locate(index)
    forward while index > @index
    backward while index < @index
  end
  
  def forward
    return if @current == @tail
    if @current.after
      @index += 1
      @current = @current.after
    end
  end
  
  def backward
    return if @current == @head
    if @current.before
      @index -= 1
      @current = @current.before
    end
  end
  
  def insert(value, before, after)
    node = Node.new(before, after, value)
    @head = node if node.before.nil?
    @tail = node if node.after.nil?
    reset unless @current
    @size += 1
    node
  end
  
  def append(value)
    if @tail
      node = Node.new(@tail, nil, value)
    else
      @head = node = Node.new(nil, nil, value)
    end
    reset unless @current
    @size += 1
    @tail = node
  end
  
  class Node
    attr_accessor :before, :after, :value
    
    def initialize(before, after, value=nil)
      @value = value
      @before = before
      @after = after
      before.after = self if before
      after.before = self if after
    end  
  end
end
