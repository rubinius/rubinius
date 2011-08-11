if Rubinius::Config['hash.hamt']
class Hash
  include Enumerable

  Vector = Rubinius::Tuple

  class State
    attr_accessor :head
    attr_accessor :tail
    attr_accessor :size

    def self.from(state)
      new_state = new
      new_state.compare_by_identity if state and state.compare_by_identity?
      new_state
    end

    def initialize
      @size = 0
      @head = nil
      @tail = nil
      @compare_by_identity = false
    end

    def compare_by_identity?
      @compare_by_identity
    end

    def compare_by_identity
      @compare_by_identity = true
      class << self
        def match?(this_key, other_key)
          other_key.equal? this_key
        end
      end
    end

    def match?(this_key, other_key)
      other_key.eql? this_key
    end
  end

  class Entry
    attr_accessor :state
    attr_accessor :previous
    attr_accessor :next
    attr_accessor :key
    attr_accessor :value

    def initialize(key, value, state)
      if key.kind_of?(String) and !key.frozen?
        key = key.dup
        key.freeze
      end

      @key = key
      @value = value
      @state = state

      if tail = state.tail
        @previous = tail
        state.tail = tail.next = self
      else
        state.head = state.tail = self
      end

      state.size += 1
    end

    def empty?
      not @state
    end

    def key_hash
      @key.hash
    end

    def lookup(key, key_hash)
      return self if @state.match? @key, key
    end

    def insert(key, key_hash, value)
      return unless @state.match? @key, key
      @value = value
      self
    end

    def delete(key, key_hash)
      if @state.match? @key, key

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

        @state.size -= 1
        @state = nil

        self
      end
    end
  end

  class List
    attr_accessor :state
    attr_accessor :key_hash
    attr_accessor :entries

    def initialize(state, key_hash)
      @state = state
      @key_hash = key_hash
      @entries = Vector.new 2
    end

    def empty?
      not @entries
    end

    def lookup(key, key_hash)
      @entries.each { |e| return e if e.lookup(key, key_hash) }
      nil
    end

    def insert(key, key_hash, value)
      @entries.each { |e| return e if e.insert(key, key_hash, value) }

      entry = Entry.new key, value, @state
      @entries = @entries.insert_at_index @entries.size, entry

      entry
    end

    def add(entry, key, key_hash, value)
      @entries[0] = entry
      @entries[1] = Entry.new key, value, @state
    end

    def delete(key, key_hash)
      @entries.each_with_index do |e, i|
        if e.delete key, key_hash
          @entries = @entries.delete_at_index i
          return e
        end
      end
      nil
    end
  end

  # An Array Mapped Trie
  class Trie
    attr_accessor :bmp
    attr_accessor :entries
    attr_accessor :level
    attr_accessor :state

    def initialize(state, level)
      @state = state
      @level = level
      @bmp = 0
      @entries = nil
    end

    def empty?
      not @entries
    end

    def lookup(key, key_hash)
      return unless index = entry_index(key_hash)
      @entries[index].lookup key, key_hash
    end

    def insert(key, key_hash, value)
      if index = entry_index(key_hash)
        entry = @entries[index]
        unless entry.insert key, key_hash, value
          trie = Trie.new @state, @level + 1
          trie.add entry, key, key_hash, value
          @entries[index] = trie
          return trie
        else
          return entry
        end
      else
        @bmp = set_bitmap key_hash
        index = entry_index key_hash
        entry = Entry.new key, value, @state
        @entries = @entries.insert_at_index index, entry
        return entry
      end
    end

    def add(entry, key, key_hash, value)
      entry_hash = entry.key_hash
      bmp = @bmp = set_bitmap(entry_hash)
      @bmp = set_bitmap key_hash

      if bmp == @bmp
        if entry_hash == key_hash
          list = List.new @state, key_hash
          list.add entry, key, key_hash, value
          @entries = Vector[list]
        else
          trie = Trie.new @state, @level + 1
          trie.add entry, key, key_hash, value
          @entries = Vector[trie]
        end
      else
        @entries = Vector.new 2
        @entries[entry_index(entry_hash)] = entry
        @entries[entry_index(key_hash)] = Entry.new key, value, @state
      end
    end

    def delete(key, key_hash)
      return unless index = entry_index(key_hash)

      entry = @entries[index]
      if de = entry.delete(key, key_hash)
        if entry.empty?
          @bmp = unset_bitmap key_hash

          @entries = @entries.delete_at_index index
        end

        return de
      end
    end

    def entry_index(key_hash)
      Rubinius.invoke_primitive :vm_hash_trie_entry_index, key_hash, @level, @bmp
    end

    def set_bitmap(key_hash)
      Rubinius.invoke_primitive :vm_hash_trie_set_bitmap, key_hash, @level, @bmp
    end

    def unset_bitmap(key_hash)
      Rubinius.invoke_primitive :vm_hash_trie_unset_bitmap, key_hash, @level, @bmp
    end
  end

  class Table
    attr_accessor :entries
    attr_accessor :state

    def initialize(state)
      @state = state
      @entries = Vector.new 64
    end

    def entry_index(key_hash)
      key_hash & 0b111111
    end

    def lookup(key, key_hash)
      if entry = @entries[entry_index(key_hash)]
        entry.lookup key, key_hash
      end
    end

    def insert(key, key_hash, value)
      index = entry_index key_hash
      if entry = @entries[index]
        unless entry.insert key, key_hash, value
          trie = Trie.new @state, 0
          trie.add entry, key, key_hash, value
          @entries[index] = trie
        end
      else
        entry = Entry.new key, value, @state
        @entries[index] = entry
      end
    end

    def delete(key, key_hash)
      index = entry_index key_hash
      return unless entry = @entries[index]

      if de = entry.delete(key, key_hash)
        if entry.empty?
          @entries[index] = nil
        end
        return de
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

  def self.[](*args)
    total = args.size
    if total == 1
      obj = args.first
      if obj.kind_of? Hash
        return allocate.replace(obj)
      elsif obj.respond_to? :to_hash
        return allocate.replace(Rubinius::Type.coerce_to(obj, Hash, :to_hash))
      elsif obj.kind_of? Array # See redmine # 1385
        h = allocate
        args.first.each do |arr|
          next unless arr.respond_to? :to_ary
          arr = arr.to_ary
          next unless (1..2).include? arr.size
          h[arr.at(0)] = arr.at(1)
        end
        return h
      end
    end

    return allocate if total == 0

    if total & 1 == 1
      raise ArgumentError, "Expected an even number, got #{total}"
    end

    hash = allocate
    i = 0

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
    allocate
  end

  def initialize(default=undefined, &block)
    Rubinius.check_frozen

    if !default.equal?(undefined) and block
      raise ArgumentError, "Specify a default value or a block, not both"
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

  def initialize_copy(other)
    replace other
  end
  private :initialize_copy

  def [](key)
    if @table and entry = @table.lookup(key, key.hash)
      return entry.value
    end

    default key
  end

  def []=(key, value)
    Rubinius.check_frozen

    unless @table
      @state = State.new unless @state
      @table = Table.new @state
    end

    @table.insert key, key.hash, value

    value
  end

  alias_method :store, :[]=

  def assoc(key)
    each_entry { |e| return e.key, e.value if key == e.key }
    nil
  end

  def clear
    @state = State.from @state
    @table = nil
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

    if @table
      if entry = @table.delete(key, key.hash)
        return entry.value
      end
    end

    return yield(key) if block_given?

    nil
  end

  def delete_if(&block)
    Rubinius.check_frozen

    return to_enum(:delete_if) unless block_given?

    each_entry { |e| delete e.key if yield(e.key, e.value) }

    self
  end

  def each
    return to_enum(:each) unless block_given?

    if @state
      entry = @state.head
      while entry
        yield [entry.key, entry.value]
        entry = entry.next
      end
    end

    self
  end

  alias_method :each_pair, :each

  def each_entry
    return unless @state

    entry = @state.head
    while entry
      yield entry
      entry = entry.next
    end
  end

  def each_key
    return to_enum(:each_key) unless block_given?

    each_entry { |e| yield e.key }

    self
  end

  def each_value
    return to_enum(:each_value) unless block_given?

    each_entry { |e| yield e.value }

    self
  end

  def empty?
    size == 0
  end

  def eql?(other)
    return true if self.equal? other

    unless other.kind_of? Hash
      return false unless other.respond_to? :to_hash
      return other.eql?(self)
    end

    return false unless other.size == size

    Thread.detect_recursion self, other do
      other.each_entry do |e|

        return false unless entry = @table.lookup(e.key, e.key.hash)

        # Order of the comparison matters! We must compare our value with
        # the other Hash's value and not the other way around.
        return false unless entry.value.eql?(e.value)
      end
    end

    true
  end

  alias_method :==, :eql?

  def fetch(key, default=undefined)
    unless empty?
      if entry = @table.lookup(key, key.hash)
        return entry.value
      end
    end

    return yield(key) if block_given?
    return default unless default.equal?(undefined)
    raise IndexError, 'key not found'
  end

  def flatten(level=1)
    to_a.flatten(level)
  end

  def initialize_copy(other)
    replace other
  end
  private :initialize_copy

  def inspect
    out = []
    return '{...}' if Thread.detect_recursion self do
      each_entry do |e|
        str =  e.key.inspect
        str << '=>'
        str << e.value.inspect
        out << str
      end
    end
    "{#{out.join ', '}}"
  end

  alias_method :to_s, :inspect

  def invert
    h = Hash.allocate
    each_entry { |e| h[e.value] = e.key }
    h
  end

  def keep_if(&block)
    Rubinius.check_frozen

    return to_enum(:keep_if) unless block_given?

    each_entry { |e| delete e.key unless yield(e.key, e.value) }

    self
  end

  def key(value)
    each_entry { |e| return e.key if e.value == value }

    nil
  end

  alias_method :index, :key

  def key?(key)
    if @table and @table.lookup(key, key.hash)
      true
    else
      false
    end
  end

  alias_method :has_key?, :key?
  alias_method :include?, :key?
  alias_method :member?, :key?

  def keys
    ary = []
    each_entry { |e| ary << e.key }
    ary
  end

  def merge(other, &block)
    dup.merge!(other, &block)
  end

  def merge!(other)
    Rubinius.check_frozen

    other = Rubinius::Type.coerce_to other, Hash, :to_hash

    return self if other.empty?

    unless @table
      @state = State.new unless @state
      @table = Table.new @state
    end

    if block_given?
      other.each_entry do |e|
        key = e.key
        if entry = @table.lookup(key, key.hash)
          entry.value = yield(key, entry.value, e.value)
        else
          @table.insert key, key.hash, e.value
        end
      end
    else
      other.each_entry do |e|
        key = e.key
        @table.insert key, key.hash, e.value
      end
    end

    self
  end

  alias_method :update, :merge!

  def rassoc(value)
    each_entry { |e| return e.key, e.value if value == e.value }
    nil
  end

  def rehash
    Rubinius.check_frozen

    return if empty?

    entry = @state.head
    @state = State.from @state
    table = Table.new @state

    while entry
      table.insert entry.key, entry.key.hash, entry.value
      entry = entry.next
    end

    @table = table

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
      size = @state.size
      delete_if(&block)
      return self if size != @state.size
    end

    nil
  end

  def replace(other)
    Rubinius.check_frozen

    other = Rubinius::Type.coerce_to other, Hash, :to_hash
    return self if self.equal? other

    @state = State.from @state
    @state.compare_by_identity if other.compare_by_identity?
    @table = Table.new @state

    other.each_entry do |e|
      key = e.key
      @table.insert key, key.hash, e.value
    end

    @default = other.default
    @default_proc = other.default_proc

    self
  end

  def select
    Rubinius.check_frozen

    return to_enum(:select) unless block_given?

    hsh = Hash.allocate

    each_entry do |e|
      key = e.key
      value = e.value
      hsh[key] = value if yield(key, value)
    end

    hsh
  end

  def select!
    Rubinius.check_frozen

    return to_enum(:select!) unless block_given?

    return nil if empty?

    size = @state.size
    each_entry { |e| delete e.key unless yield(e.key, e.value) }
    return nil if size == @state.size

    self
  end

  def shift
    Rubinius.check_frozen

    return default(nil) if empty?

    entry = @state.head
    @table.delete entry.key, entry.key.hash

    return entry.key, entry.value
  end

  def size
    @state ? @state.size : 0
  end

  alias_method :length, :size

  def to_a
    ary = []
    each_entry { |e| ary << [e.key, e.value] }
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
    each_entry { |e| return true if value == e.value }
    false
  end

  alias_method :has_value?, :value?

  def values
    ary = []
    each_entry { |e| ary << e.value }
    ary
  end

  def values_at(*args)
    if empty?
      args.map { |key| default key }
    else
      args.map do |key|
        if entry = @table.lookup(key, key.hash)
          entry.value
        else
          default key
        end
      end
    end
  end
end
end
