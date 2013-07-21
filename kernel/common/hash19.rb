# -*- encoding: us-ascii -*-

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
          Rubinius::Type.object_equal other_key, this_key
        end
      end

      self
    end

    def match?(this_key, this_hash, other_key, other_hash)
      other_hash == this_hash and (Rubinius::Type::object_equal(other_key, this_key) or other_key.eql?(this_key))
    end
  end

  # Bucket stores key, value pairs in Hash. The key's hash
  # is also cached in the item and recalculated when the
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
    def next(item)
      if item
        return item if item = item.next
      else
        return @state.head
      end
    end
  end

  # Hash methods

  def self.try_convert(obj)
    Rubinius::Type.try_convert obj, Hash, :to_hash
  end

  def self.new_from_associate_array(associate_array)
    hash = new
    associate_array.each do |array|
      next unless array.respond_to? :to_ary
      array = array.to_ary
      next unless (1..2).cover? array.size
      hash[array.at(0)] = array.at(1)
    end
    hash
  end
  private_class_method :new_from_associate_array

  def []=(key, value)
    Rubinius.check_frozen

    redistribute @entries if @size > @max_entries

    key_hash = Rubinius.privately { key.hash }
    index = key_hash & @mask

    item = @entries[index]
    unless item
      @entries[index] = new_bucket key, key_hash, value
      return value
    end

    if @state.match? item.key, item.key_hash, key, key_hash
      return item.value = value
    end

    last = item
    item = item.link
    while item
      if @state.match? item.key, item.key_hash, key, key_hash
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

  def assoc(key)
    each_item { |e| return e.key, e.value if key == e.key }
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
    if @default_proc and !undefined.equal?(key)
      @default_proc.call(self, key)
    else
      @default
    end
  end

  def default_proc
    @default_proc
  end

  # Sets the default proc to be executed on each key lookup
  def default_proc=(prc)
    Rubinius.check_frozen
    prc = Rubinius::Type.coerce_to prc, Proc, :to_proc

    if prc.lambda? and prc.arity != 2
      raise TypeError, "default proc must have arity 2"
    end

    @default = nil
    @default_proc = prc
  end

  def delete(key)
    Rubinius.check_frozen
    key_hash = Rubinius.privately { key.hash }

    index = key_index key_hash
    if item = @entries[index]
      if item.delete key, key_hash
        @entries[index] = item.link
        @size -= 1
        return item.value
      end

      last = item
      while item = item.link
        if item.delete key, key_hash
          last.link = item.link
          @size -= 1
          return item.value
        end
        last = item
      end
    end

    return yield(key) if block_given?
  end

  def each_item
    return unless @state

    item = @state.head
    while item
      yield item
      item = item.next
    end
  end

  def each
    return to_enum(:each) unless block_given?

    return unless @state

    item = @state.head
    while item
      yield [item.key, item.value]
      item = item.next
    end

    self
  end

  alias_method :each_pair, :each

  def fetch(key, default=undefined)
    if item = find_item(key)
      return item.value
    end

    return yield(key) if block_given?
    return default unless undefined.equal?(default)
    raise KeyError, "key #{key} not found"
  end

  # Searches for an item matching +key+. Returns the item
  # if found. Otherwise returns +nil+.
  def find_item(key)
    key_hash = Rubinius.privately { key.hash }

    item = @entries[key_index(key_hash)]
    while item
      if @state.match? item.key, item.key_hash, key, key_hash
        return item
      end
      item = item.link
    end
  end

  def flatten(level=1)
    to_a.flatten(level)
  end

  alias_method :key, :index

  def keep_if
    return to_enum(:keep_if) unless block_given?

    Rubinius.check_frozen

    each_item { |e| delete e.key unless yield(e.key, e.value) }

    self
  end

  def initialize(default=undefined, &block)
    Rubinius.check_frozen

    if !undefined.equal?(default) and block
      raise ArgumentError, "Specify a default or a block, not both"
    end

    if block
      @default = nil
      @default_proc = block
    elsif !undefined.equal?(default)
      @default = default
      @default_proc = nil
    end

    self
  end
  private :initialize

  alias_method :to_s, :inspect

  def merge!(other)
    Rubinius.check_frozen

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
        __store__ item.key, item.value
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
        if item = @entries[index]
          old.link = item
        end
        @entries[index] = old

        old = nxt
      end
    end
  end

  def rassoc(value)
    each_item { |e| return e.key, e.value if value == e.value }
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

    other.each_item do |item|
      __store__ item.key, item.value
    end

    @default = other.default
    @default_proc = other.default_proc

    self
  end

  alias_method :initialize_copy, :replace
  private :initialize_copy

  def select
    return to_enum(:select) unless block_given?

    selected = Hash.allocate

    each_item do |item|
      if yield(item.key, item.value)
        selected[item.key] = item.value
      end
    end

    selected
  end

  def select!
    return to_enum(:select!) unless block_given?

    Rubinius.check_frozen

    return nil if empty?

    size = @size
    each_item { |e| delete e.key unless yield(e.key, e.value) }
    return nil if size == @size

    self
  end

  def shift
    Rubinius.check_frozen

    return default(nil) if empty?

    item = @state.head
    delete item.key

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
    @state    = State.new
  end
  private :__setup__

  # Returns an external iterator for the bins. See +Iterator+
  def to_iter
    Iterator.new @state
  end
end
end
