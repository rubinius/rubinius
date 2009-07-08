class Hash

  include Enumerable

  # Entry stores key, value pairs in Hash. The key's hash
  # is also cached in the entry and recalculated when the
  # Hash#rehash method is called.

  class Entry
    attr_accessor :key
    attr_accessor :key_hash
    attr_accessor :value
    attr_accessor :next

    def initialize(key, key_hash, value)
      @key      = key
      @key_hash = key_hash
      @value    = value
    end

    def match?(key, key_hash)
      @key_hash == key_hash and key.eql? @key
    end
  end

  # An external iterator that returns only entry chains from the
  # Hash storage, never nil bins. While somewhat following the API
  # of Enumerator, it is named Iterator because it does not provide
  # <code>#each</code> and should not conflict with +Enumerator+ in
  # MRI 1.8.7+. Returned by <code>Hash#to_iter</code>.

  class Iterator
    attr_reader :index

    def initialize(entries, capacity)
      @entries  = entries
      @capacity = capacity
      @index    = -1
    end

    # Returns the next object or +nil+.
    def next(entry)
      if entry and entry = entry.next
        return entry
      end

      while (@index += 1) < @capacity
        if entry = @entries[@index]
          return entry
        end
      end
    end
  end

  # Hash methods

  attr_reader :size

  alias_method :length, :size

  Entries = Rubinius::Tuple

  # Initial size of Hash. MUST be a power of 2.
  MIN_SIZE = 16

  # Allocate more storage when this full. This value grows with
  # the size of the Hash so that the max load factor is 0.75.
  MAX_ENTRIES = 12

  # Overridden in lib/1.8.7 or lib/1.9
  def self.[](*args)
    if args.size == 1
      obj = args.first
      if obj.kind_of? Hash or obj.respond_to? :to_hash
        return new.replace(obj)
      end
    end

    if args.size & 1 == 1
      raise ArgumentError, "Expected an even number, got #{args.length}"
    end

    hash = new
    i = args.to_iter 2
    while i.next
      hash[i.item] = i.at(1)
    end
    hash
  end

  # Creates a fully-formed instance of Hash.
  def self.allocate
    hash = super()
    Rubinius.privately { hash.setup }
    hash
  end

  def ==(other)
    return true if self.equal? other
    unless other.kind_of? Hash
      return false unless other.respond_to? :to_hash
      return other == self
    end

    return false unless other.size == size

    # Pickaxe claims that defaults are compared, but MRI 1.8.[46] doesn't actually do that
    # return false unless other.default == default
    Thread.detect_recursion self, other do
      i = to_iter
      while entry = i.next(entry)
        return false unless other[entry.key] == entry.value
      end
    end
    true
  end

  def [](key)
    if entry = find_entry(key)
      entry.value
    else
      default key
    end
  end

  def []=(key, value)
    redistribute @entries if @size > @max

    key_hash = key.hash
    index = key_index key_hash

    unless entry = @entries[index]
      @entries[index] = new_entry key, key_hash, value
      return value
    else
      if entry.match? key, key_hash
        return entry.value = value
      end
    end

    last = entry
    entry = entry.next
    while entry
      if entry.match? key, key_hash
        return entry.value = value
      end

      last = entry
      entry = entry.next
    end
    last.next = new_entry key, key_hash, value

    value
  end

  alias_method :store, :[]=

  def clear
    setup
    self
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
    key_hash = key.hash

    index = key_index key_hash
    if entry = @entries[index]
      if entry.match? key, key_hash
        @entries[index] = entry.next
        @size -= 1
        return entry.value
      end

      last = entry
      while entry = entry.next
        if entry.match? key, key_hash
          last.next = entry.next
          @size -= 1
          return entry.value
        end
        last = entry
      end
    end

    return yield(key) if block_given?
  end

  # Overridden in lib/1.8.7 or lib/1.9
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

  # Overridden in lib/1.8.7 or lib/1.9
  def each
    i = to_iter
    while entry = i.next(entry)
      yield [entry.key, entry.value]
    end
    self
  end

  # Overridden in lib/1.8.7 or lib/1.9
  def each_key
    i = to_iter
    while entry = i.next(entry)
      yield entry.key
    end
    self
  end

  # Overridden in lib/1.8.7 or lib/1.9
  def each_pair
    i = to_iter
    while entry = i.next(entry)
      yield entry.key, entry.value
    end
    self
  end

  # Overridden in lib/1.8.7 or lib/1.9
  def each_value
    i = to_iter
    while entry = i.next(entry)
      yield entry.value
    end
    self
  end

  # Returns true if there are no entries.
  def empty?
    @size == 0
  end

  def fetch(key, default = Undefined)
    if entry = find_entry(key)
      return entry.value
    end

    return yield(key) if block_given?
    return default unless default.equal?(Undefined)
    raise IndexError, 'key not found'
  end

  # Searches for an entry matching +key+. Returns the entry
  # if found. Otherwise returns +nil+.
  def find_entry(key)
    key_hash = key.hash

    entry = @entries[key_index(key_hash)]
    while entry
      if entry.match? key, key_hash
        return entry
      end
      entry = entry.next
    end
  end
  private :find_entry

  def index(value)
    i = to_iter
    while entry = i.next(entry)
      return entry.key if entry.value == value
    end
    nil
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

  def inspect
    out = []
    return '{...}' if Thread.detect_recursion self do
      i = to_iter
      while entry = i.next(entry)
        str =  entry.key.inspect
        str << '=>'
        str << entry.value.inspect
        out << str
      end
    end
    "{#{out.join ', '}}"
  end

  def invert
    inverted = {}
    i = to_iter
    while entry = i.next(entry)
      inverted[entry.value] = entry.key
    end
    inverted
  end

  def key?(key)
    return true if find_entry key

    false
  end

  alias_method :has_key?, :key?
  alias_method :include?, :key?
  alias_method :member?, :key?

  # Calculates the +@entries+ slot given a key_hash value.
  def key_index(key_hash)
    key_hash & @mask
  end
  private :key_index

  def keys
    map { |key, value| key }
  end

  def merge(other, &block)
    dup.merge!(other, &block)
  end

  def merge!(other)
    other = Type.coerce_to other, Hash, :to_hash
    i = other.to_iter
    while entry = i.next(entry)
      key = entry.key
      if block_given? and key? key
        self[key] = yield key, self[key], entry.value
      else
        self[key] = entry.value
      end
    end
    self
  end
  alias_method :update, :merge!

  # Returns a new +Entry+ instance having +key+, +key_hash+,
  # and +value+. If +key+ is a kind of +String+, +key+ is
  # duped and frozen.
  def new_entry(key, key_hash, value)
    if key.kind_of? String
      key = key.dup
      key.freeze
    end

    @size += 1
    Entry.new key, key_hash, value
  end
  private :new_entry

  # Adjusts the hash storage and redistributes the entries among
  # the new bins. Any Iterator instance will be invalid after a
  # call to #redistribute. Does not recalculate the cached key_hash
  # values. See +#rehash+.
  def redistribute(entries)
    capacity = @capacity

    # TODO: grow smaller too
    setup @capacity * 2, @max * 2, @size

    i = -1
    while (i += 1) < capacity
      next unless old = entries[i]
      while old
        old.next = nil if nxt = old.next

        index = key_index old.key_hash
        if entry = @entries[index]
          old.next = entry
        end
        @entries[index] = old

        old = nxt
      end
    end
  end

  # Recalculates the cached key_hash values and reorders the entries
  # into a new +@entries+ vector. Does NOT change the size of the
  # hash. See +#redistribute+.
  def rehash
    capacity = @capacity
    entries  = @entries

    @entries = Entries.new @capacity

    i = -1
    while (i += 1) < capacity
      next unless old = entries[i]
      while old
        old.next = nil if nxt = old.next

        index = key_index(old.key_hash = old.key.hash)
        if entry = @entries[index]
          old.next = entry
        end
        @entries[index] = old

        old = nxt
      end
    end

    self
  end

  # Overridden in lib/1.8.7 or lib/1.9
  def reject(&block)
    hsh = dup
    hsh.reject! &block
    hsh
  end

  # Overridden in lib/1.8.7 or lib/1.9
  def reject!
    rejected = select { |k, v| yield k, v }
    return if rejected.empty?

    rejected.each { |k, v| delete k }
    self
  end

  def replace(other)
    other = Type.coerce_to other, Hash, :to_hash
    return self if self.equal? other

    setup
    i = other.to_iter
    while entry = i.next(entry)
      self[entry.key] = entry.value
    end

    if other.default_proc
      @default = other.default_proc
      @default_proc = true
    else
      @default = other.default
      @default_proc = false
    end

    self
  end

  # Overridden in lib/1.8.7 or lib/1.9
  def select
    selected = []
    i = to_iter
    while entry = i.next(entry)
      if yield(entry.key, entry.value)
        selected << [entry.key, entry.value]
      end
    end

    selected
  end

  def shift
    return default(nil) if empty?

    i = to_iter
    if entry = i.next(entry)
      @entries[i.index] = entry.next
      @size -= 1
      return entry.key, entry.value
    end
  end

  # Sets the underlying data structures.
  #
  # @capacity is the maximum number of +@entries+.
  # @max is the maximum number of entries before redistributing.
  # @size is the number of pairs, equivalent to <code>hsh.size</code>.
  # @entrien is the vector of storage for the entry chains.
  def setup(capacity=MIN_SIZE, max=MAX_ENTRIES, size=0)
    @capacity = capacity
    @mask     = capacity - 1
    @max      = max
    @size     = size
    @entries  = Entries.new capacity
  end
  private :setup

  def sort(&block)
    to_a.sort(&block)
  end

  def to_a
    select { true }
  end

  # Returns an external iterator for the bins. See +Iterator+
  def to_iter
    Iterator.new @entries, @capacity
  end

  def to_hash
    self
  end

  def to_s
    to_a.join
  end

  def value?(value)
    i = to_iter
    while entry = i.next(entry)
      return true if entry.value == value
    end
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
