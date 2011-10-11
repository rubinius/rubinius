class Hash

  include Enumerable

  # Bucket stores key, value pairs in Hash. The key's hash
  # is also cached in the item and recalculated when the
  # Hash#rehash method is called.

  class Bucket
    attr_accessor :key
    attr_accessor :key_hash
    attr_accessor :value
    attr_accessor :link

    def initialize(key, key_hash, value)
      @key      = key
      @key_hash = key_hash
      @value    = value
      @link     = nil
    end

    def match?(key, key_hash)
      case key
      when Symbol, Fixnum
        return key.equal?(@key)
      end

      @key_hash == key_hash and key.eql? @key
    end
  end

  # An external iterator that returns only item chains from the
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
    def next(item)
      if item and item = item.link
        return item
      end

      while (@index += 1) < @capacity
        if item = @entries[@index]
          return item
        end
      end
    end
  end

  # Hash methods

  attr_reader :size

  # #entries is a method provided by Enumerable which calls #to_a,
  # so we have to not collide with that.
  attr_reader_specific :entries, :__entries__

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
    hash = __allocate__
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
      each_item do |item|
        other_item = other.find_item(item.key)

        # Other doesn't even have this key
        return false unless other_item

        # Order of the comparison matters! We must compare our value with
        # the other Hash's value and not the other way around.
        return false unless item.value == other_item.value
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
      each_item do |item|
        other_item = other.find_item(item.key)

        # Other doesn't even have this key
        return false unless other_item

        # Order of the comparison matters! We must compare our value with
        # the other Hash's value and not the other way around.
        return false unless item.value.eql?(other_item.value)
      end
    end
    true
  end

  def hash
    val = size
    Thread.detect_outermost_recursion self do
      each_item do |item|
        val ^= item.key.hash
        val ^= item.value.hash
      end
    end

    return val
  end

  def [](key)
    if item = find_item(key)
      item.value
    else
      default key
    end
  end

  def []=(key, value)
    Rubinius.check_frozen

    redistribute @entries if @size > @max_entries

    key_hash = key.hash
    index = key_hash & @mask

    item = @entries[index]
    unless item
      @entries[index] = new_bucket key, key_hash, value
      return value
    end

    if item.match? key, key_hash
      return item.value = value
    end

    last = item
    item = item.link
    while item
      if item.match? key, key_hash
        return item.value = value
      end

      last = item
      item = item.link
    end
    last.link = new_bucket key, key_hash, value

    value
  end

  alias_method :store, :[]=

  # Used internally to get around subclasses redefining #[]=
  alias_method :__store__, :[]=

  def clear
    Rubinius.check_frozen
    __setup__
    self
  end

  def default(key = undefined)
    # current MRI documentation comment is wrong.  Actual behavior is:
    # Hash.new { 1 }.default # => nil
    if @default_proc
      Rubinius::Type.object_equal(key, undefined) ? nil : @default.call(self, key)
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
    Rubinius.check_frozen

    key_hash = key.hash

    ents = @entries

    index = key_index key_hash
    if item = ents[index]
      if item.match? key, key_hash
        ents[index] = item.link
        @size -= 1
        return item.value
      end

      last = item
      while item = item.link
        if item.match? key, key_hash
          last.link = item.link
          @size -= 1
          return item.value
        end
        last = item
      end
    end

    return yield(key) if block_given?
  end

  def delete_if(&block)
    Rubinius.check_frozen

    return to_enum(:delete_if) unless block_given?

    select(&block).each { |k, v| delete k }
    self
  end

  def each_item
    idx = 0
    cap = @capacity
    entries = @entries

    while idx < cap
      item = entries[idx]
      while item
        yield item
        item = item.link
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
      item = entries[idx]
      while item
        yield [item.key, item.value]
        item = item.link
      end

      idx += 1
    end

    self
  end

  def each_key
    return to_enum(:each_key) unless block_given?

    each_item { |e| yield e.key }
    self
  end

  def each_value
    return to_enum(:each_value) unless block_given?

    each_item do |item|
      yield item.value
    end
    self
  end

  def each_pair
    return to_enum(:each_pair) unless block_given?

    each_item do |item|
      yield item.key, item.value
    end
    self
  end

  # Returns true if there are no entries.
  def empty?
    @size == 0
  end

  def fetch(key, default=undefined)
    if item = find_item(key)
      return item.value
    end

    return yield(key) if block_given?
    return default unless default.equal?(undefined)
    raise IndexError, 'key not found'
  end

  # Searches for an item matching +key+. Returns the item
  # if found. Otherwise returns +nil+.
  def find_item(key)
    key_hash = key.hash

    item = @entries[key_index(key_hash)]
    while item
      if item.match? key, key_hash
        return item
      end
      item = item.link
    end
  end

  def index(value)
    each_item do |item|
      return item.key if item.value == value
    end
    nil
  end

  def initialize(default=undefined, &block)
    Rubinius.check_frozen

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

  def inspect
    out = []
    return '{...}' if Thread.detect_recursion self do
      each_item do |item|
        str =  item.key.inspect
        str << '=>'
        str << item.value.inspect
        out << str
      end
    end
    "{#{out.join ', '}}"
  end

  def invert
    inverted = {}
    each_item do |item|
      inverted[item.value] = item.key
    end
    inverted
  end

  def key?(key)
    find_item(key) != nil
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
    each_item do |item|
      ary << item.key
    end
    ary
  end

  def merge(other, &block)
    dup.merge!(other, &block)
  end

  def merge!(other)
    other = Rubinius::Type.coerce_to other, Hash, :to_hash

    if block_given?
      other.each_item do |item|
        key = item.key
        if key? key
          __store__ key, yield(key, self[key], item.value)
        else
          __store__ key, item.value
        end
      end
    else
      other.each_item do |item|
        key = item.key
        __store__ key, item.value
      end
    end
    self
  end

  alias_method :update, :merge!

  # Returns a new +Bucket+ instance having +key+, +key_hash+,
  # and +value+. If +key+ is a kind of +String+, +key+ is
  # duped and frozen.
  def new_bucket(key, key_hash, value)
    if key.kind_of?(String) and !key.frozen?
      key = key.dup
      key.freeze
    end

    @size += 1
    Bucket.new key, key_hash, value
  end
  private :new_bucket

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
        old.link = nil if nxt = old.link

        index = key_index old.key_hash
        if item = @entries[index]
          old.link = item
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
        old.link = nil if nxt = old.link

        index = key_index(old.key_hash = old.key.hash)
        if item = @entries[index]
          old.link = item
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
    Rubinius.check_frozen

    return to_enum(:reject!) unless block_given?

    capacity = @capacity
    entries = @entries
    change = 0

    i = -1
    while (i += 1) < capacity
      prev_item = nil
      item = entries[i]
      while item
        if yield(item.key,item.value)
          change += 1
          if !prev_item
            entries[i] = item.link
          else
            prev_item.link = item.link
            prev_item = item.link
          end
        end
        item = item.link
      end
    end

    if change > 0
      @size -= change
      return self
    end
    nil
  end

  def replace(other)
    Rubinius.check_frozen

    other = Rubinius::Type.coerce_to other, Hash, :to_hash
    return self if self.equal? other

    size = other.size

    # If we're replacing ourself with a Hash that is empty,
    # then fast path.
    if 0 == size

      __setup__

    else

      # Normally this would be a call to __setup__,
      # but that will create a new unused Tuple
      # that we would wind up replacing anyways.
      ents = @entries = other.__entries__.dup
      capa = @capacity = other.capacity
      @mask     = @capacity - 1
      @max_entries = other.max_entries
      @size     = size

      # We now contain a list of the other hash's
      # Bucket objects. We need to re-map them to our
      # own.
      i = 0

      while i < capa
        if orig = ents[i]
          ents[i] = self_item = orig.dup
          while orig = orig.link
            self_item.link = orig.dup
            self_item = self_item.link
          end
        end
        i += 1
      end

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

  alias_method :initialize_copy, :replace
  private :initialize_copy

  def select
    return to_enum(:select) unless block_given?

    selected = []

    each_item do |item|
      if yield(item.key, item.value)
        selected << [item.key, item.value]
      end
    end

    selected
  end

  def shift
    Rubinius.check_frozen

    return default(nil) if empty?

    idx = 0
    cap = @capacity
    entries = @entries

    while idx < cap
      if item = entries[idx]
        entries[idx] = item.link
        @size -= 1
        break
      end

      idx += 1
    end

    return item.key, item.value
  end

  # Sets the underlying data structures.
  #
  # @capacity is the maximum number of +@entries+.
  # @max_entries is the maximum number of entries before redistributing.
  # @size is the number of pairs, equivalent to <code>hsh.size</code>.
  # @entrien is the vector of storage for the item chains.
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

    each_item do |item|
      ary << [item.key, item.value]
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
    each_item do |item|
      return true if item.value == value
    end
    false
  end

  alias_method :has_value?, :value?

  def values
    ary = []

    each_item do |item|
      ary << item.value
    end

    ary
  end

  def values_at(*args)
    args.map do |key|
      if item = find_item(key)
        item.value
      else
        default key
      end
    end
  end

  alias_method :indexes, :values_at
  alias_method :indices, :values_at
end
