# -*- encoding: us-ascii -*-

unless Rubinius::Config['hash.hamt']
class Hash
  attr_reader :size
  attr_reader :capacity
  attr_reader :max_entries

  alias_method :length, :size

  Entries = Rubinius::Tuple

  # Initial size of Hash. MUST be a power of 2.
  MIN_SIZE = 16

  # Allocate more storage when this full. This value grows with
  # the size of the Hash so that the max load factor is 0.75.
  MAX_ENTRIES = 12

  def self.new_from_literal(size)
    allocate
  end

  # Creates a fully-formed instance of Hash.
  def self.allocate
    hash = super()
    Rubinius.privately { hash.__setup__ }
    hash
  end

  # #entries is a method provided by Enumerable which calls #to_a,
  # so we have to not collide with that.
  attr_reader_specific :entries, :__entries__

  def self.[](*args)
    if args.size == 1
      obj = args.first
      if hash = Rubinius::Type.check_convert_type(obj, Hash, :to_hash)
        return allocate.replace(hash)
      elsif associate_array = Rubinius::Type.check_convert_type(obj, Array, :to_ary)
        return new_from_associate_array(associate_array)
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
        unless Rubinius::Type::object_equal(item.value, other_item.value) or
               item.value == other_item.value
          return false
        end
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
        unless Rubinius::Type::object_equal(item.value, other_item.value) or
               item.value.eql?(other_item.value)
          return false
        end
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

    val
  end

  def [](key)
    if item = find_item(key)
      item.value
    else
      default key
    end
  end

  def clear
    Rubinius.check_frozen
    __setup__
    self
  end

  def default=(value)
    @default_proc = nil
    @default = value
  end

  def delete_if(&block)
    return to_enum(:delete_if) unless block_given?

    Rubinius.check_frozen

    select(&block).each { |k, v| delete k }
    self
  end

  def each_key
    return to_enum(:each_key) unless block_given?

    each_item { |item| yield item.key }
    self
  end

  def each_value
    return to_enum(:each_value) unless block_given?

    each_item { |item| yield item.value }
    self
  end

  # Returns true if there are no entries.
  def empty?
    @size == 0
  end

  def index(value)
    each_item do |item|
      return item.key if item.value == value
    end
  end

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

  def reject(&block)
    return to_enum(:reject) unless block_given?

    hsh = dup.delete_if(&block)
    hsh.taint if tainted?
    hsh
  end

  def reject!(&block)
    return to_enum(:reject!) unless block_given?

    Rubinius.check_frozen

    unless empty?
      size = @size
      delete_if(&block)
      return self if size != @size
    end

    nil
  end

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

  def to_hash
    self
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

  alias_method :indices, :values_at
  alias_method :indexes, :values_at
end
end
