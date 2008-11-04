# depends on: enumerable.rb misc.rb class.rb

class Hash

  include Enumerable

  def self.[](*args)
    if args.size == 1
      obj = args.first
      if obj.kind_of? Hash
        return new.replace(obj)
      elsif obj.respond_to? :to_hash
        return new.replace(Type.coerce_to(obj, Hash, :to_hash))
      end
    end

    if args.size & 1 == 1
      raise ArgumentError, "Expected an even number, got #{args.length}"
    end

    hsh = new
    i = 0
    while i < args.size
      hsh[args[i]] = args[i+1]
      i += 2
    end
    hsh
  end

  def initialize(default = Undefined, &block)
    if !default.equal?(Undefined) and block
      raise ArgumentError, "Specify a default or a block, not both"
    end

    if block
      @default = block
      @default_proc = true
    elsif !default.equal?(Undefined)
      @default = default
      @default_proc = false
    end
  end
  private :initialize

  def initialize_copy(other)
    replace other
  end
  private :initialize_copy

  def ==(other)
    return true if self.equal? other
    return false unless other.is_a? Hash or other.respond_to? :to_hash

    other = Type.coerce_to other, Hash, :to_hash
    return false unless other.size == size

    # Pickaxe claims that defaults are compared, but MRI 1.8.[46] doesn't actually do that
    # return false unless other.default == default
    each_item { |k, v| return false unless other[k] == v }
    true
  end

  def fetch(key, default = Undefined)
    hsh = key_hash key
    bin = entry_bin hsh

    if entry = bins[bin]
      result = entry.find key, hsh
      return result.value if result
    end

    return yield(key) if block_given?
    return default unless default.equal?(Undefined)
    raise IndexError, 'key not found'
  end

  def [](key)
    hsh = key_hash key
    bin = entry_bin hsh

    if entry = bins[bin]
      result = entry.find key, hsh
      return result.value if result
    end

    default key
  end

  def []=(key, value)
    key = key.dup if key.kind_of? String
    # TODO: freeze if String

    hsh = key_hash key
    entry = self.entry key, hsh
    self.count += 1 if entry.set(key, value, hsh)

    value
  end

  alias_method :store, :[]=

  def clear
    __initialize__
    self
  end

  def clone
    hash = dup
    hash.freeze if frozen?
    hash
  end

  def default(key = Undefined)
    # current MRI documentation comment is wrong.  Actual behavior is:
    # Hash.new { 1 }.default # => nil
    if @default_proc
      key.equal?(Undefined) ? nil : @default.call(self, key)
    else
      @default
    end
  end

  def default=(value)
    @default_proc = false
    @default = value
  end

  def default_proc
    @default if @default_proc
  end

  def delete(key)
    key = key.dup if key.kind_of? String # to bypass singleton hash method

    hsh = key_hash key
    bin = entry_bin hsh

    if entry = bins[bin]
      result = entry.delete key, hsh

      bins[bin] = entry.next if result.nil?
      unless result
        self.count -= 1
        return entry.value
      end
    end

    return yield(key) if block_given?
  end

  def delete_if(&block)
    select(&block).each { |k, v| delete k }

    self
  end

  def dup
    hash = self.class.new
    hash.send :initialize_copy, self
    hash.taint if self.tainted?
    hash
  end

  def each_key
    each_item { |k, v| yield k }

    self
  end

  def each
    each_item { |k, v| yield [k, v] }

    self
  end

  # Yields key, value for each item in the Hash. This method
  # is necessary to protect the essential iterator from subclasses
  # (e.g. REXML::Attribute) that replace #each with a version
  # that is incompatible with the dependencies here (e.g. defining
  # #each -> #each_attribute -> #each_value, where we had been
  # defining #each_value in terms of #each).
  def each_item
    i = to_iter
    while entry = i.next
      begin
        yield entry.key, entry.value
      end while entry = entry.next
    end

    self
  end

  alias_method :each_pair, :each_item

 def each_value
    each_item { |k, v| yield v }

    self
  end

  # Returns +size+ == 0. Does not use #count so that a subclass's
  # idea of size will be consistent with emptiness.
  def empty?
    size == 0
  end

  def index(value)
    each_item { |k, v| return k if v == value }
    nil
  end

  def inspect
    # recursively_inspect
    return '{...}' if RecursionGuard.inspecting?(self)

    out = []
    RecursionGuard.inspect(self) do
      each_item do |key, value|
        str =  key.inspect
        str << '=>'
        str << value.inspect
        out << str
      end
    end

    "{#{out.join ', '}}"
  end

  def invert
    inverted = {}
    each_item { |key, value| inverted[value] = key }
    inverted
  end

  def key?(key)
    i = to_iter
    while entry = i.next
      begin
        return true if entry.key_hash == key.hash and entry.key.eql? key
      end while entry = entry.next
    end
    false
  end

  alias_method :has_key?, :key?
  alias_method :include?, :key?
  alias_method :member?, :key?

  def keys
    map { |key, value| key }
  end

  def merge(other, &block)
    dup.merge!(other, &block)
  end

  def merge!(other)
    other = Type.coerce_to other, Hash, :to_hash
    other.each_item do |k, v|
      if block_given? and key? k
        self[k] = yield k, self[k], v
      else
        self[k] = v
      end
    end
    self
  end
  alias_method :update, :merge!

  alias_method :rehash, :redistribute

  def reject(&block)
    hsh = dup
    hsh.reject! &block
    hsh
  end

  def reject!
    rejected = select { |k, v| yield k, v }
    return if rejected.empty?

    rejected.each { |k, v| delete k }
    self
  end

  def replace(other)
    other = Type.coerce_to other, Hash, :to_hash
    return self if self.equal? other

    clear
    other.each_item { |k, v| self[k] = v }

    if other.default_proc
      @default = other.default_proc
      @default_proc = true
    else
      @default = other.default
      @default_proc = false
    end

    self
  end

  def select
    selected = []
    i = to_iter
    while e = i.next
      begin
        selected << [e.key, e.value] if yield(e.key, e.value)
      end while e = e.next
    end

    selected
  end

  def shift
    return default(nil) if empty?

    i = to_iter
    if entry = i.next
      bins[i.index] = entry.next
      self.count -= 1
      return entry.key, entry.value
    end
  end

  alias_method :length, :count
  alias_method :size,   :count

  def sort(&block)
    to_a.sort(&block)
  end

  def to_a
    select { true }
  end

  def to_hash
    self
  end

  def to_s
    to_a.join
  end

  def value?(value)
    each_item { |k, v| return true if v == value }
    false
  end
  alias_method :has_value?, :value?

  def values
    map { |key, value| value }
  end

  def values_at(*args)
    args.collect { |key| self[key] }
  end
  alias_method :indexes, :values_at
  alias_method :indices, :values_at

end
