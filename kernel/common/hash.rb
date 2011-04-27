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
      @next     = nil
    end

    def match?(key, key_hash)
      case key
      when Symbol, Fixnum
        return key.equal?(@key)
      end

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

  # #entries is a method provided by Enumerable which calls #to_a,
  # so we have to not collide with that.
  def __entries__
    @entries
  end

  attr_reader :capacity
  attr_reader :max_entries

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
      if obj.kind_of? Hash
        return new.replace(obj)
      elsif obj.respond_to? :to_hash
        return new.replace(Rubinius::Type.coerce_to(obj, Hash, :to_hash))
      elsif obj.is_a?(Array) # See redmine # 1385
        h = new
        args.first.each do |arr|
          next unless arr.respond_to? :to_ary
          arr = arr.to_ary
          next unless (1..2).include? arr.size
          h[arr.at(0)] = arr.at(1)
        end
        return h
      end
    end

    return new if args.empty?

    if args.size & 1 == 1
      raise ArgumentError, "Expected an even number, got #{args.length}"
    end

    hash = new
    i = 0
    total = args.size

    while i < total
      hash[args[i]] = args[i+1]
      i += 2
    end

    hash
  end

  def self.new_from_literal(size)
    new
  end

  # Creates a fully-formed instance of Hash.
  def self.allocate
    hash = super()
    Rubinius.privately { hash.__setup__ }
    hash
  end

  def ==(other)
    return true if self.equal? other
    unless other.kind_of? Hash
      return false unless other.respond_to? :to_hash
      return other == self
    end

    return false unless other.size == size

    Thread.detect_recursion self, other do
      each_entry do |entry|
        other_entry = other.find_entry(entry.key)

        # Other doesn't even have this key
        return false unless other_entry

        # Order of the comparison matters! We must compare our value with
        # the other Hash's value and not the other way around.
        return false unless entry.value == other_entry.value
      end
    end
    true
  end

  def eql?(other)
    # Just like ==, but uses eql? to compare values.
    return true if self.equal? other
    unless other.kind_of? Hash
      return false unless other.respond_to? :to_hash
      return other.eql?(self)
    end

    return false unless other.size == size

    Thread.detect_recursion self, other do
      each_entry do |entry|
        other_entry = other.find_entry(entry.key)

        # Other doesn't even have this key
        return false unless other_entry

        # Order of the comparison matters! We must compare our value with
        # the other Hash's value and not the other way around.
        return false unless entry.value.eql?(other_entry.value)
      end
    end
    true
  end

  def hash
    val = size
    Thread.detect_outermost_recursion self do
      each_entry do |entry|
        val ^= entry.key.hash
        val ^= entry.value.hash
      end
    end

    return val
  end

  def [](key)
    if entry = find_entry(key)
      entry.value
    else
      default key
    end
  end

  def []=(key, value)
    Ruby.check_frozen

    redistribute @entries if @size > @max_entries

    key_hash = key.hash
    index = key_hash & @mask

    entry = @entries[index]
    unless entry
      @entries[index] = new_entry key, key_hash, value
      return value
    end

    if entry.match? key, key_hash
      return entry.value = value
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

  # Used internally to get around subclasses redefining #[]=
  alias_method :__store__, :[]=

  def clear
    __setup__
    self
  end

  def default(key = undefined)
    # current MRI documentation comment is wrong.  Actual behavior is:
    # Hash.new { 1 }.default # => nil
    if @default_proc
      key.equal?(undefined) ? nil : @default.call(self, key)
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
    Ruby.check_frozen

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

  def delete_if(&block)
    Ruby.check_frozen

    return to_enum(:delete_if) unless block_given?

    select(&block).each { |k, v| delete k }
    self
  end

  def each_entry
    idx = 0
    cap = @capacity
    entries = @entries

    while idx < cap
      entry = entries[idx]
      while entry
        yield entry
        entry = entry.next
      end

      idx += 1
    end
  end

  def each
    return to_enum(:each) unless block_given?

    idx = 0
    cap = @capacity
    entries = @entries

    while idx < cap
      entry = entries[idx]
      while entry
        yield [entry.key, entry.value]
        entry = entry.next
      end

      idx += 1
    end

    self
  end

  def each_key
    return to_enum(:each_key) unless block_given?

    each_entry { |e| yield e.key }
    self
  end

  def each_pair
    return to_enum(:each_pair) unless block_given?

    each_entry do |entry|
      yield entry.key, entry.value
    end
    self
  end

  def each_value
    return to_enum(:each_value) unless block_given?

    each_entry do |entry|
      yield entry.value
    end
    self
  end

  # Returns true if there are no entries.
  def empty?
    @size == 0
  end

  def fetch(key, default=undefined)
    if entry = find_entry(key)
      return entry.value
    end

    return yield(key) if block_given?
    return default unless default.equal?(undefined)
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

  def index(value)
    each_entry do |entry|
      return entry.key if entry.value == value
    end
    nil
  end

  def initialize(default=undefined, &block)
    Ruby.check_frozen

    if !default.equal?(undefined) and block
      raise ArgumentError, "Specify a default or a block, not both"
    end

    if block
      @default = block
      @default_proc = true
    elsif !default.equal?(undefined)
      @default = default
      @default_proc = false
    end

    self
  end
  private :initialize

  def initialize_copy(other)
    replace other
  end
  private :initialize_copy

  def inspect
    out = []
    return '{...}' if Thread.detect_recursion self do
      each_entry do |entry|
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
    each_entry do |entry|
      inverted[entry.value] = entry.key
    end
    inverted
  end

  def key?(key)
    find_entry(key) != nil
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
    ary = []
    each_entry do |entry|
      ary << entry.key
    end
    ary
  end

  def merge(other, &block)
    dup.merge!(other, &block)
  end

  def merge!(other)
    other = Rubinius::Type.coerce_to other, Hash, :to_hash

    if block_given?
      other.each_entry do |entry|
        key = entry.key
        if key? key
          __store__ key, yield(key, self[key], entry.value)
        else
          __store__ key, entry.value
        end
      end
    else
      other.each_entry do |entry|
        key = entry.key
        __store__ key, entry.value
      end
    end
    self
  end
  alias_method :update, :merge!

  # Returns a new +Entry+ instance having +key+, +key_hash+,
  # and +value+. If +key+ is a kind of +String+, +key+ is
  # duped and frozen.
  def new_entry(key, key_hash, value)
    if key.kind_of?(String) and !key.frozen?
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
    __setup__ @capacity * 2, @max_entries * 2, @size

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

  def reject
    return to_enum(:reject) unless block_given?

    hsh = self.class.new
    hsh.taint if self.tainted?
    self.each { |k,v| hsh[k] = v unless yield(k,v) }
    hsh
  end

  def reject!
    Ruby.check_frozen

    return to_enum(:reject!) unless block_given?

    capacity = @capacity
    entries = @entries
    change = 0

    i = -1
    while (i += 1) < capacity
      prev_entry = nil
      entry = entries[i]
      while entry
        if yield(entry.key,entry.value)
          change += 1
          if !prev_entry
            entries[i] = entry.next
          else
            prev_entry.next = entry.next
            prev_entry = entry.next
          end
        end
        entry = entry.next
      end
    end

    if change > 0
      @size -= change
      return self
    end
    nil
  end

  def replace(other)
    Ruby.check_frozen

    other = Rubinius::Type.coerce_to other, Hash, :to_hash
    return self if self.equal? other

    # Normally this would be a call to __setup__,
    # but that will create a new unused Tuple
    # that we would wind up replacing anyways.
    @entries = other.__entries__.dup
    @capacity = other.capacity
    @mask     = @capacity - 1
    @max_entries = other.max_entries
    @size     = other.size

    # We now contain a list of the other hash's
    # Entry objects. We need to re-map them to our
    # own.
    i = 0
    while i < @capacity
      if orig = @entries[i]
        @entries[i] = self_entry = orig.dup
        while orig = orig.next
          self_entry.next = orig.dup
          self_entry = self_entry.next
        end
      end
      i += 1
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

  def select
    return to_enum(:select) unless block_given?

    selected = []

    each_entry do |entry|
      if yield(entry.key, entry.value)
        selected << [entry.key, entry.value]
      end
    end

    selected
  end

  def shift
    Ruby.check_frozen

    return default(nil) if empty?

    idx = 0
    cap = @capacity
    entries = @entries

    while idx < cap
      if entry = entries[idx]
        entries[idx] = entry.next
        @size -= 1
        break
      end

      idx += 1
    end

    return entry.key, entry.value
  end

  # Sets the underlying data structures.
  #
  # @capacity is the maximum number of +@entries+.
  # @max_entries is the maximum number of entries before redistributing.
  # @size is the number of pairs, equivalent to <code>hsh.size</code>.
  # @entrien is the vector of storage for the entry chains.
  def __setup__(capacity=MIN_SIZE, max=MAX_ENTRIES, size=0)
    @capacity = capacity
    @mask     = capacity - 1
    @max_entries = max
    @size     = size
    @entries  = Entries.new capacity
  end
  private :__setup__

  def sort(&block)
    to_a.sort(&block)
  end

  def to_a
    ary = []

    each_entry do |entry|
      ary << [entry.key, entry.value]
    end

    ary
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
    each_entry do |entry|
      return true if entry.value == value
    end
    false
  end

  alias_method :has_value?, :value?

  def values
    ary = []

    each_entry do |entry|
      ary << entry.value
    end

    ary
  end

  def values_at(*args)
    args.collect { |key| self[key] }
  end

  alias_method :indexes, :values_at
  alias_method :indices, :values_at
end
