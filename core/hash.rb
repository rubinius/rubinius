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

    private :initialize

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
    end

    def match?(this_key, this_hash, other_key, other_hash)
      other_hash == this_hash and
        (Rubinius::Type::object_equal(other_key, this_key) or
          other_key.eql?(this_key))
    end
  end

  class Item
    attr_accessor :previous
    attr_accessor :next
    attr_accessor :key
    attr_accessor :key_hash
    attr_accessor :value

    def initialize(state, key, key_hash, value)
      if key.kind_of?(String) and !key.frozen? and !state.compare_by_identity?
        key = key.dup
        key.freeze
      end

      @key = key
      @key_hash = key_hash
      @value = value

      if tail = state.tail
        @previous = tail
        state.tail = tail.next = self
      else
        state.head = state.tail = self
      end

      state.size += 1
    end

    private :initialize

    def empty?
      not @key_hash
    end

    def lookup(state, key, key_hash)
      return self if state.match? @key, @key_hash, key, key_hash
    end

    def insert(state, key, key_hash, value)
      return unless state.match? @key, @key_hash, key, key_hash
      @value = value
      self
    end

    def delete(state, key, key_hash)
      if state.match? @key, @key_hash, key, key_hash

        if @previous
          @previous.next = @next
        else
          state.head = @next
        end

        if @next
          @next.previous = @previous
        else
          state.tail = @previous
        end

        state.size -= 1
        @key_hash = nil

        self
      end
    end

    def inspect
      "\#<#{self.class}:0x#{object_id.to_s(16)} @key=#{@key} @key_hash=#{@key_hash} @value=#{@value} @previous=\#<#{@previous.class}:0x#{@previous.object_id.to_s(16)}> @next=\#<#{@next.class}:0x#{@next.object_id.to_s(16)}>>"
    end
  end

  class List
    attr_accessor :key_hash
    attr_accessor :entries

    def initialize(key_hash)
      @key_hash = key_hash
      @entries = Vector.new 2
    end

    private :initialize

    def empty?
      not @entries
    end

    def lookup(state, key, key_hash)
      @entries.each { |e| return e if e.lookup(state, key, key_hash) }
      nil
    end

    def insert(state, key, key_hash, value)
      @entries.each { |e| return e if e.insert(state, key, key_hash, value) }

      item = Item.new state, key, key_hash, value
      @entries = @entries.insert_at_index @entries.size, item

      item
    end

    def add(state, item, key, key_hash, value)
      @entries[0] = item
      @entries[1] = Item.new state, key, key_hash, value
    end

    def delete(state, key, key_hash)
      @entries.each_with_index do |e, i|
        if e.delete state, key, key_hash
          @entries = @entries.delete_at_index i
          return e
        end
      end
      nil
    end
  end

  # An Array Mapped Trie
  class Trie
    MAX_LEVELS = 10

    attr_accessor :bmp
    attr_accessor :entries
    attr_accessor :level

    def self.from_key(state, key, key_hash, value)
      trie = new 0
      trie.add_key state, key, key_hash, value
      trie
    end

    def self.from_item(state, level, item, key, key_hash, value)
      if level < MAX_LEVELS
        trie = new level + 1
        trie.add_item state, item, key, key_hash, value
        trie
      else
        list = List.new key_hash
        list.add state, item, key, key_hash, value
        list
      end
    end

    def self.from_none
      trie = new 0
      trie.entries = Vector[]
      trie
    end

    def initialize(level)
      @level = level
      @bmp = 0
      @entries = nil
    end

    private :initialize

    def empty?
      not @entries
    end

    def lookup(state, key, key_hash)
      return unless index = item_index(key_hash)
      @entries[index].lookup state, key, key_hash
    end

    def insert(state, key, key_hash, value)
      if index = item_index(key_hash)
        item = @entries[index]
        if item.insert state, key, key_hash, value
          return item
        else
          obj = Trie.from_item state, @level, item, key, key_hash, value
          @entries[index] = obj
          return obj
        end
      else
        @bmp = set_bitmap key_hash
        index = item_index key_hash
        item = Item.new state, key, key_hash, value
        @entries = @entries.insert_at_index index, item
        return item
      end
    end

    def add_key(state, key, key_hash, value)
      @bmp = set_bitmap key_hash
      item = Item.new state, key, key_hash, value
      @entries = Vector[item]
    end

    def add_item(state, item, key, key_hash, value)
      item_hash = item.key_hash
      bmp = @bmp = set_bitmap(item_hash)
      @bmp = set_bitmap key_hash

      if bmp == @bmp
        if item_hash == key_hash
          list = List.new key_hash
          list.add state, item, key, key_hash, value
          @entries = Vector[list]
        else
          obj = Trie.from_item state, @level, item, key, key_hash, value
          @entries = Vector[obj]
        end
      else
        @entries = Vector.new 2
        @entries[item_index(item_hash)] = item
        @entries[item_index(key_hash)] = Item.new state, key, key_hash, value
      end
    end

    def delete(state, key, key_hash)
      return unless index = item_index(key_hash)

      item = @entries[index]
      if de = item.delete(state, key, key_hash)
        if item.empty?
          @bmp = unset_bitmap key_hash

          @entries = @entries.delete_at_index index
        end

        return de
      end
    end

    def inspect
      s = "\#<#{self.class}:0x#{object_id.to_s(16)} @bmp=#{@bmp} @level=#{@level} @entries="
      if @entries.size > 0
        s << "[ "
        s << @entries.map { |e| "\#<#{e.class}:0x#{e.object_id.to_s(16)}>" }.join(", ")
        s << " ]"
      end
      s << ">"
    end

    def item_index(key_hash)
      Rubinius.invoke_primitive :vm_hash_trie_item_index, key_hash, @level, @bmp
    end

    def set_bitmap(key_hash)
      Rubinius.invoke_primitive :vm_hash_trie_set_bitmap, key_hash, @level, @bmp
    end

    def unset_bitmap(key_hash)
      Rubinius.invoke_primitive :vm_hash_trie_unset_bitmap, key_hash, @level, @bmp
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

    private :initialize

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

  def self.[](*args)
    total = args.size
    if total == 1
      obj = args.first
      if hash = Rubinius::Type.check_convert_type(obj, Hash, :to_hash)
        new_hash = allocate.replace(hash)
        new_hash.default = nil
        return new_hash
      elsif array = Rubinius::Type.check_convert_type(obj, Array, :to_ary)
        hash = allocate
        array.each do |a|
          a = Rubinius::Type.check_convert_type a, Array, :to_ary
          next unless a

          size = a.size
          unless size >= 1 and size <= 2
            raise ArgumentError, "invalid number of elements (#{size} for 1..2)"
          end
          hash[a[0]] = a[1]
        end

        return hash
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

    if !undefined.equal?(default) and block
      raise ArgumentError, "Specify a default value or a block, not both"
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

  def hash
    val = size
    Thread.detect_outermost_recursion self do
      each_item do |item|
        Rubinius.privately { val ^= item.key.hash }
        Rubinius.privately { val ^= item.value.hash }
      end
    end

    return val
  end

  def initialize_copy(other)
    replace other
  end
  private :initialize_copy

  def [](key)
    if @trie
      Rubinius.privately { key_hash = key.hash }
      if item = @trie.lookup(@state, key, key_hash)
        return item.value
      end
    end

    default key
  end

  def []=(key, value)
    Rubinius.check_frozen

    Rubinius.privately { key_hash = key.hash }

    if @trie
      @trie.insert @state, key, key_hash, value
    else
      @state = State.new unless @state
      @trie = Trie.from_key @state, key, key_hash, value
    end

    value
  end

  alias_method :store, :[]=

  def <(other)
    other = Rubinius::Type.coerce_to(other, Hash, :to_hash)
    other > self
  end

  def <=(other)
    other = Rubinius::Type.coerce_to(other, Hash, :to_hash)
    other >= self
  end

  def >(other)
    other = Rubinius::Type.coerce_to(other, Hash, :to_hash)

    return false if size <= other.size

    self >= other
  end

  def >=(other)
    other = Rubinius::Type.coerce_to(other, Hash, :to_hash)

    return false if size < other.size

    other.each do |other_key, other_val|
      val = fetch(other_key, undefined)
      return false if undefined.equal?(val) || val != other_val
    end

    true
  end

  def assoc(key)
    each_item { |e| return e.key, e.value if key == e.key }
    nil
  end

  def clear
    @state = State.from @state
    @trie = nil
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
    if @default_proc and !undefined.equal?(key)
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
    Rubinius.check_frozen
    unless prc.nil?
      prc = Rubinius::Type.coerce_to prc, Proc, :to_proc

      if prc.lambda? and prc.arity != 2
        raise TypeError, "default proc must have arity 2"
      end
    end

    @default = nil
    @default_proc = prc
  end

  def delete(key)
    Rubinius.check_frozen

    if @trie
      Rubinius.privately { key_hash = key.hash }
      if item = @trie.delete(@state, key, key_hash)
        @trie = nil if @trie.empty?
        return item.value
      end
    end

    return yield(key) if block_given?

    nil
  end

  def delete_if
    return to_enum(:delete_if) { size } unless block_given?

    Rubinius.check_frozen

    each_item { |e| delete e.key if yield(e.key, e.value) }

    self
  end

  def dig(key, *remaining_keys)
    item = self[key]
    return item if remaining_keys.empty? || item.nil?

    raise TypeError, "#{item.class} does not have #dig method" unless item.respond_to?(:dig)

    item.dig(*remaining_keys)
  end

  def each
    return to_enum(:each) { size } unless block_given?

    if @state
      item = @state.head
      while item
        yield [item.key, item.value]
        item = item.next
      end
    end

    self
  end

  alias_method :each_pair, :each

  def each_item
    return unless @state

    item = @state.head
    while item
      yield item
      item = item.next
    end
  end

  def each_key
    return to_enum(:each_key) { size } unless block_given?

    each_item { |e| yield e.key }

    self
  end

  def each_value
    return to_enum(:each_value) { size } unless block_given?

    each_item { |e| yield e.value }

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
      other.each_item do |e|

        Rubinius.privately { key_hash = e.key.hash }
        return false unless item = @trie.lookup(@state, e.key, key_hash)

        # Order of the comparison matters! We must compare our value with
        # the other Hash's value and not the other way around.
        return false unless item.value.eql?(e.value)
      end
    end

    true
  end

  def ==(other)
    return true if self.equal? other

    unless other.kind_of? Hash
      return false unless other.respond_to? :to_hash
      return other == self
    end

    return false unless other.size == size

    Thread.detect_recursion self, other do
      other.each_item do |e|

        Rubinius.privately { key_hash = e.key.hash }
        item = @trie.lookup(@state, e.key, key_hash)

        return false unless item

        # Order of the comparison matters! We must compare our value with
        # the other Hash's value and not the other way around.
        unless Rubinius::Type::object_equal(item.value, e.value) or
               item.value == e.value
          return false
        end
      end
    end

    true
  end

  def fetch(key, default=undefined)
    if @trie
      Rubinius.privately { key_hash = key.hash }
      if item = @trie.lookup(@state, key, key_hash)
        return item.value
      end
    end

    return yield(key) if block_given?
    return default unless undefined.equal?(default)
    raise KeyError, "key #{key} not found"
  end

  def fetch_values(*keys, &block)
    keys.map { |key| fetch(key, &block) }
  end

  # Searches for an item matching +key+. Returns the item
  # if found. Otherwise returns +nil+. Called from the C-API.
  def find_item(key)
    if @trie
      Rubinius.privately { key_hash = key.hash }
      if item = @trie.lookup(@state, key, key_hash)
        return item
      end
    end

    nil
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
      each_item do |e|
        str =  Rubinius::Type.coerce_inspect(e.key)
        str << '=>'
        str << Rubinius::Type.coerce_inspect(e.value)
        out << str
      end
    end

    ret = "{#{out.join ', '}}"
    Rubinius::Type.infect(ret, self) unless empty?
    ret
  end

  alias_method :to_s, :inspect

  def invert
    h = Hash.allocate
    each_item { |e| h[e.value] = e.key }
    h
  end

  def keep_if
    return to_enum(:keep_if) { size } unless block_given?

    Rubinius.check_frozen

    each_item { |e| delete e.key unless yield(e.key, e.value) }

    self
  end

  def key(value)
    each_item { |e| return e.key if e.value == value }

    nil
  end

  alias_method :index, :key

  def key?(key)
    if @trie
      Rubinius.privately { key_hash = key.hash }
      if @trie.lookup(@state, key, key_hash)
        return true
      end
    end

    false
  end

  alias_method :has_key?, :key?
  alias_method :include?, :key?
  alias_method :member?, :key?

  def keys
    return [] unless @state

    ary = Array.new @state.size

    i = 0
    item = @state.head

    while item
      ary[i] = item.key
      i += 1
      item = item.next
    end

    ary
  end

  def merge(other, &block)
    dup.merge!(other, &block)
  end

  def merge!(other)
    Rubinius.check_frozen

    other = Rubinius::Type.coerce_to other, Hash, :to_hash

    return self if other.empty?

    unless @trie
      @state = State.new unless @state
      @trie = Trie.from_none
    end

    if block_given?
      other.each_item do |e|
        key = e.key
        Rubinius.privately { key_hash = key.hash }

        if item = @trie.lookup(@state, key, key_hash)
          item.value = yield(key, item.value, e.value)
        else
          @trie.insert @state, key, key_hash, e.value
        end
      end
    else
      other.each_item do |e|
        key = e.key
        Rubinius.privately { key_hash = key.hash }
        @trie.insert @state, key, key_hash, e.value
      end
    end

    self
  end

  alias_method :update, :merge!

  def rassoc(value)
    each_item { |e| return e.key, e.value if value == e.value }
    nil
  end

  def rehash
    Rubinius.check_frozen

    return if empty?

    item = @state.head
    @state = State.from @state
    @trie = Trie.from_none

    while item
      Rubinius.privately { key_hash = item.key.hash }
      @trie.insert @state, item.key, key_hash, item.value
      item = item.next
    end

    self
  end

  def reject(&block)
    return to_enum(:reject) { size } unless block_given?

    hsh = dup.delete_if(&block)
    hsh.taint if tainted?
    hsh
  end

  def reject!(&block)
    return to_enum(:reject!) { size } unless block_given?

    Rubinius.check_frozen

    unless empty?
      size = @state.size
      delete_if(&block)
      return self if size != @state.size
    end

    nil
  end

  def replace(other)
    Rubinius.check_frozen

    unless other.kind_of? Hash
      other = Rubinius::Type.coerce_to other, Hash, :to_hash
    end
    return self if self.equal? other

    @state = State.from @state
    @state.compare_by_identity if other.compare_by_identity?
    @trie = Trie.from_none

    other.each_item do |e|
      Rubinius.privately { key_hash = e.key.hash }
      @trie.insert @state, e.key, key_hash, e.value
    end

    @default = other.default
    @default_proc = other.default_proc

    self
  end

  def select
    return to_enum(:select) { size } unless block_given?

    hsh = Hash.allocate

    each_item do |e|
      key = e.key
      value = e.value
      hsh[key] = value if yield(key, value)
    end

    hsh
  end

  def select!
    return to_enum(:select!) { size } unless block_given?

    Rubinius.check_frozen

    return if empty?

    size = @state.size
    each_item { |e| delete e.key unless yield(e.key, e.value) }
    return if size == @state.size

    self
  end

  def shift
    Rubinius.check_frozen

    return default(nil) if empty?

    item = @state.head
    Rubinius.privately { key_hash = item.key.hash }
    @trie.delete @state, item.key, key_hash
    @trie = nil if @trie.empty?

    return item.key, item.value
  end

  def size
    @state ? @state.size : 0
  end

  alias_method :length, :size

  def to_a
    if @state
      ary = Array.new size

      i = 0
      item = @state.head

      while item
        ary[i] = [item.key, item.value]
        i += 1
        item = item.next
      end
    else
      ary = []
    end

    Rubinius::Type.infect ary, self
    ary
  end

  # Returns an external iterator for the bins. See +Iterator+
  def to_iter
    Iterator.new @state
  end

  def to_h
    if instance_of? Hash
      self
    else
      Hash.allocate.replace(to_hash)
    end
  end

  def to_hash
    self
  end

  def to_proc
    method(:[]).to_proc
  end

  def value?(value)
    each_item { |e| return true if value == e.value }
    false
  end

  alias_method :has_value?, :value?

  def values
    return [] unless @state

    ary = Array.new size

    i = 0
    item = @state.head

    while item
      ary[i] = item.value
      i += 1
      item = item.next
    end

    ary
  end

  def values_at(*args)
    if empty?
      args.map { |key| default key }
    else
      args.map do |key|
        Rubinius.privately { key_hash = key.hash }
        if item = @trie.lookup(@state, key, key_hash)
          item.value
        else
          default key
        end
      end
    end
  end

  # Used internally in Rubinius to get around subclasses redefining #[]=
  alias_method :__store__, :[]=
end
