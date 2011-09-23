unless Rubinius::Config['hash.hamt']
class Hash
  include Enumerable

  class State
    attr_accessor :head
    attr_accessor :tail

    def self.from(state)
      new_state = new
      new_state.compare_by_identity if state and state.compare_by_identity?
      new_state
    end

    def initialize
      @compare_by_identity = false
      @head = nil
      @tail = nil
    end

    def compare_by_identity?
      @compare_by_identity
    end

    def compare_by_identity
      @compare_by_identity = true
      class << self
        def match?(this_key, this_hash, other_key, other_hash)
          other_key.equal? this_key
        end
      end
    end

    def match?(this_key, this_hash, other_key, other_hash)
      other_hash == this_hash and other_key.eql? this_key
    end
  end

  # Bucket stores key, value pairs in Hash. The key's hash
  # is also cached in the entry and recalculated when the
  # Hash#rehash method is called.

  class Bucket
    attr_accessor :key
    attr_accessor :key_hash
    attr_accessor :value
    attr_accessor :link
    attr_accessor :previous
    attr_accessor :next
    attr_accessor :state

    def initialize(key, key_hash, value, state)
      @key      = key
      @key_hash = key_hash
      @value    = value
      @link     = nil
      @state    = state

      if tail = state.tail
        @previous = tail
        state.tail = tail.next = self
      else
        state.head = state.tail = self
      end
    end

    def delete(key, key_hash)
      if @state.match? @key, @key_hash, key, key_hash
        remove
        self
      end
    end

    def remove
      if @previous
        @previous.next = @next
      else
        @state.head = @next
      end

      if @next
        @next.previous = @previous
      else
        @state.tail = @previous
      end
    end
  end

  # An external iterator that returns entries in insertion order.  While
  # somewhat following the API of Enumerator, it is named Iterator because it
  # does not provide <code>#each</code> and should not conflict with
  # +Enumerator+ in MRI 1.8.7+. Returned by <code>Hash#to_iter</code>.

  class Iterator
    def initialize(state)
      @state = state
    end

    # Returns the next object or +nil+.
    def next(entry)
      if entry
        return entry if entry = entry.next
      else
        return @state.head
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

  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, Hash, :to_hash
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
    Rubinius.check_frozen

    redistribute @entries if @size > @max_entries

    key_hash = key.hash
    index = key_hash & @mask

    entry = @entries[index]
    unless entry
      @entries[index] = new_bucket key, key_hash, value
      return value
    end

    if @state.match? entry.key, entry.key_hash, key, key_hash
      return entry.value = value
    end

    last = entry
    entry = entry.link
    while entry
      if @state.match? entry.key, entry.key_hash, key, key_hash
        return entry.value = value
      end

      last = entry
      entry = entry.link
    end
    last.link = new_bucket key, key_hash, value

    value
  end

  alias_method :store, :[]=

  # Used internally to get around subclasses redefining #[]=
  alias_method :__store__, :[]=

  def assoc(key)
    each_entry { |e| return e.key, e.value if key == e.key }
    nil
  end

  def clear
    Rubinius.check_frozen

    __setup__
    self
  end

  def compare_by_identity
    Rubinius.check_frozen

    @state = State.new unless @state
    @state.compare_by_identity
    self
  end

  def compare_by_identity?
    return false unless @state
    @state.compare_by_identity?
  end

  def default(key=undefined)
    if @default_proc and !key.equal?(undefined)
      @default_proc.call(self, key)
    else
      @default
    end
  end

  def default=(value)
    @default_proc = nil
    @default = value
  end

  def default_proc
    @default_proc
  end

  # Sets the default proc to be executed on each key lookup
  def default_proc=(prc)
    prc = Rubinius::Type.coerce_to prc, Proc, :to_proc

    if prc.lambda? and prc.arity != 2
      raise TypeError, "default proc must have arity 2"
    end

    @default = nil
    @default_proc = prc
  end

  def delete(key)
    Rubinius.check_frozen

    key_hash = key.hash

    index = key_index key_hash
    if entry = @entries[index]
      if entry.delete key, key_hash
        @entries[index] = entry.link
        @size -= 1
        return entry.value
      end

      last = entry
      while entry = entry.link
        if entry.delete key, key_hash
          last.link = entry.link
          @size -= 1
          return entry.value
        end
        last = entry
      end
    end

    return yield(key) if block_given?
  end

  def delete_if(&block)
    Rubinius.check_frozen

    return to_enum(:delete_if) unless block_given?

    each_entry { |e| delete e.key if yield(e.key, e.value) }
    self
  end

  def each_entry
    return unless @state

    entry = @state.head
    while entry
      yield entry
      entry = entry.next
    end
  end

  def each
    return to_enum(:each) unless block_given?

    return unless @state

    entry = @state.head
    while entry
      yield [entry.key, entry.value]
      entry = entry.next
    end

    self
  end

  alias_method :each_pair, :each

  def each_key
    return to_enum(:each_key) unless block_given?

    each_entry { |e| yield e.key }
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
      if @state.match? entry.key, entry.key_hash, key, key_hash
        return entry
      end
      entry = entry.link
    end
  end

  def flatten(level=1)
    to_a.flatten(level)
  end

  def key(value)
    each_entry do |entry|
      return entry.key if entry.value == value
    end
    nil
  end

  alias_method :index, :key

  def keep_if(&block)
    Rubinius.check_frozen

    return to_enum(:keep_if) unless block_given?

    each_entry { |e| delete e.key unless yield(e.key, e.value) }

    self
  end

  def initialize(default=undefined, &block)
    Rubinius.check_frozen

    if !default.equal?(undefined) and block
      raise ArgumentError, "Specify a default or a block, not both"
    end

    if block
      @default = nil
      @default_proc = block
    elsif !default.equal?(undefined)
      @default = default
      @default_proc = nil
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

  alias_method :to_s, :inspect

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
    Rubinius.check_frozen

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
        __store__ entry.key, entry.value
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
    Bucket.new key, key_hash, value, @state
  end
  private :new_bucket

  # Adjusts the hash storage and redistributes the entries among
  # the new bins. Any Iterator instance will be invalid after a
  # call to #redistribute. Does not recalculate the cached key_hash
  # values. See +#rehash+.
  def redistribute(entries)
    capacity = @capacity

    # Rather than using __setup__, initialize the specific values we need to
    # change so we don't eg overwrite @state.
    @capacity    = capacity * 2
    @entries     = Entries.new @capacity
    @mask        = @capacity - 1
    @max_entries = @max_entries * 2

    i = -1
    while (i += 1) < capacity
      next unless old = entries[i]
      while old
        old.link = nil if nxt = old.link

        index = key_index old.key_hash
        if entry = @entries[index]
          old.link = entry
        end
        @entries[index] = old

        old = nxt
      end
    end
  end

  def rassoc(value)
    each_entry { |e| return e.key, e.value if value == e.value }
    nil
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
        if entry = @entries[index]
          old.link = entry
        end
        @entries[index] = old

        old = nxt
      end
    end

    self
  end

  def reject(&block)
    return to_enum(:reject) unless block_given?

    hsh = dup.delete_if(&block)
    hsh.taint if tainted?
    hsh
  end

  def reject!(&block)
    Rubinius.check_frozen

    return to_enum(:reject!) unless block_given?

    unless empty?
      size = @size
      delete_if(&block)
      return self if size != @size
    end

    nil
  end

  def replace(other)
    Rubinius.check_frozen

    other = Rubinius::Type.coerce_to other, Hash, :to_hash
    return self if self.equal? other

    # Normally this would be a call to __setup__, but that will create a new
    # unused Tuple that we would wind up replacing anyways.
    @capacity = other.capacity
    @entries  = Entries.new @capacity
    @mask     = @capacity - 1
    @size     = 0
    @max_entries = other.max_entries

    @state = State.new
    @state.compare_by_identity if other.compare_by_identity?

    other.each_entry do |entry|
      __store__ entry.key, entry.value
    end

    @default = other.default
    @default_proc = other.default_proc

    self
  end

  def select
    return to_enum(:select) unless block_given?

    selected = Hash.allocate

    each_entry do |entry|
      if yield(entry.key, entry.value)
        selected[entry.key] = entry.value
      end
    end

    selected
  end

  def select!
    Rubinius.check_frozen

    return to_enum(:select!) unless block_given?

    return nil if empty?

    size = @size
    each_entry { |e| delete e.key unless yield(e.key, e.value) }
    return nil if size == @size

    self
  end

  def shift
    Rubinius.check_frozen

    return default(nil) if empty?

    entry = @state.head
    delete entry.key

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
    @state    = State.new
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
    Iterator.new @state
  end

  def to_hash
    self
  end

  def value?(value)
    each_entry do |entry|
      return true if value == entry.value
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
    args.map do |key|
      if entry = find_entry(key)
        entry.value
      else
        default key
      end
    end
  end

  alias_method :indexes, :values_at
  alias_method :indices, :values_at
end
end
