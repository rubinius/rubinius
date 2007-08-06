class Hash
  include Enumerable
  
  # VM specific: keys, fetch, []=, delete
  # We choose fetch instead of [] because it doesn't need Hash#default
  # Alternative subset: keys, [], []=, delete, initialize, default, default_proc
  
  alias :store :[]=
  
  # TODO: Ought to use  default = (no_default = true; nil)
  # Compiler currently chokes on it. -- flgr
  def initialize(*args, &default_proc)
    if args.size > 1 then
      raise(ArgumentError, "wrong number of arguments (%d for 1)" % args.size)
    end
    
    has_default = args.size == 1
    default = args.first if has_default
    
    if default_proc and has_default then
      raise(ArgumentError, "can't supply both default proc and default value")
    end
    
    @default_proc = default_proc
    @default = default
  end
  
  def self.[](*args)
    unless args.size % 2 == 0 or (args.size == 1 and args[0].is_a?(Hash))
      raise ArgumentError, "odd number of arguments for Hash" 
    end
    
    hsh = self.new
    
    while args.size >= 2
      k = args.shift
      v = args.shift
      hsh[k] = v
    end
    
    if args.size == 1
      hsh.merge! args[0]
    end
    
    return hsh
  end  

  # TODO: This really ought to work -- flgr
  #attr_reader :default_proc
  def default_proc() @default_proc end
  
  def default=(default)
    @default_proc = nil
    @default = default
  end  
  
  def default(key = nil)
    default_proc ? default_proc.call(self, key) : @default
  end

  def key?(key)
    # Slightly wrong: should test bins directly
    key_hash = key.hash
    keys.any? { |k| key_hash == k.hash and key.eql?(k) }
  end unless instance_methods.include?("key?")
  
  alias :has_key? :key?
  alias :include? :key?
  alias :member?  :key?
    
  def [](key)
    key?(key) ? fetch(key) : default(key)
  end

  def each_key(&block)
    # Slightly wrong: should be affected by modification from block
    keys.each(&block)
    return self
  end unless instance_methods.include?("each_key")

  def each_value(&block)
    values.each(&block)
    return self
  end

  def each_pair()
    each_key do |key|
      yield(key, self[key])
    end
  end

  def each()
    each_pair do |*pair|
      yield(pair)
    end
  end

  def values()
    keys.map { |key| self[key] }
  end

  def index(value)
    keys.find { |key| self[key] == value }
  end

  def value?(value)
    keys.any? { |key| self[key] == value }
  end
  alias :has_value? :value?

  def clear()
    # Non-empty hashes would however raise because of delete()
    raise(TypeError, "can't modify frozen hash") if frozen?
    
    keys.each { |key| delete(key) }
    return self
  end

  def size()
    keys.size
  end
  alias :length :size
  
  def empty?()
    size == 0
  end

  def delete_if()
    # Non-empty hashes would however raise because of delete()
    raise(TypeError, "can't modify frozen hash") if frozen?

    each_pair do |key, value|
      delete(key) if yield(key, value)
    end
    return self
  end
  
  def reject!(&block)
    old_size = size
    delete_if(&block)
    old_size == size ? nil : self
  end

  def reject(&block)
    self.dup.delete_if(&block)
  end

  def values_at(*keys)
    keys.map { |key| self[key] }
  end
  alias :indexes :values_at
  alias :indices :values_at

  def invert()
    result = {}
    each_pair do |key, value|
      result[value] = key
    end
    return result
  end
  
  def merge!(other)
    other.to_hash.each do |key, value| 
      if key?(key) and block_given?
        self[key] = yield(key, self[key], value)
      else
        self[key] = value
      end
    end
    
    return self
  end
  alias :update :merge!

  def merge(other, &block)
    dup.merge!(other, &block)
  end
  
  def replace(other)
    other = other.to_hash
    
    unless equal?(other)
      clear
      
      other.each do |key, value|
        self[key] = value
      end

      if other.default_proc then
        @default_proc = other.default_proc
      else
        self.default = other.default
      end
    end
    
    return self
  end
  alias :initialize_copy :replace
  private :initialize_copy

  def rehash()
    replace(clone)
  end

  def shift()
    # Non-empty hashes would however raise because of delete()
    raise(TypeError, "can't modify frozen hash") if frozen?

    return default if empty?
    key = keys.first
    value = delete(key)
    return [key, value]
  end

  def to_hash()
    self
  end

  def ==(other)
    return true if equal?(other)
    
    self.size == other.size and
    other.all? do |key, value|
      key?(key) and
      self[key] == value
    end
  end

  def inspect()
    seen = (Thread.current[:__inspect_key__] ||= [])
    if seen.include?(object_id) then
      "{...}"
    else
      begin
        seen << object_id
        "{%s}" % map { |pair| "%p=>%p" % pair }.join(", ")
      ensure
        seen.pop
      end
    end
  end

  # These need to be defined as new instance methods
  def to_a() super end
  def sort() super end

  # Difference to Enumerable#select: yields key, value instead of [key, value]
  def select()
    super do |args|
      yield(*args)
    end
  end

  def to_s()
    to_a.join
  end
end
