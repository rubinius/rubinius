# -*- encoding: us-ascii -*-

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

      @key_hash == key_hash and (Rubinius::Type::object_equal(key, @key) or key.eql?(@key))
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

  def self.new_from_associate_array(associate_array)
    hash = new
    associate_array.each do |array|
      next unless array.respond_to? :to_ary
      array = array.to_ary
      next unless (1..2).include? array.size
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

  def default(key = undefined)
    # current MRI documentation comment is wrong.  Actual behavior is:
    # Hash.new { 1 }.default # => nil
    if @default_proc
      undefined.equal?(key) ? nil : @default.call(self, key)
    else
      @default
    end
  end

  def default_proc
    @default if @default_proc
  end

  def delete(key)
    Rubinius.check_frozen
    key_hash = Rubinius.privately { key.hash }

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

  def each_pair
    return to_enum(:each_pair) unless block_given?

    each_item do |item|
      yield item.key, item.value
    end
    self
  end

  def fetch(key, default=undefined)
    if item = find_item(key)
      return item.value
    end

    return yield(key) if block_given?
    return default unless undefined.equal?(default)
    raise IndexError, 'key not found'
  end

  # Searches for an item matching +key+. Returns the item
  # if found. Otherwise returns +nil+.
  def find_item(key)
    key_hash = Rubinius.privately { key.hash }

    item = @entries[key_index(key_hash)]
    while item
      if item.match? key, key_hash
        return item
      end
      item = item.link
    end
  end

  def initialize(default=undefined, &block)
    Rubinius.check_frozen

    if !undefined.equal?(default) and block
      raise ArgumentError, "Specify a default or a block, not both"
    end

    if block
      @default = block
      @default_proc = true
    elsif !undefined.equal?(default)
      @default = default
      @default_proc = false
    end

    self
  end
  private :initialize

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

  # Returns an external iterator for the bins. See +Iterator+
  def to_iter
    Iterator.new @entries, @capacity
  end

  def to_s
    to_a.join
  end
end
