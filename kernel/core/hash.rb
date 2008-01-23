# depends on: enumerable.rb misc.rb class.rb

class Hash
  include Enumerable

  def self.[](*args)
    if args.first.kind_of? Hash and args.length == 1
      return new.replace args.first
    end

    raise ArgumentError, "Expected an even number, got #{args.length}" if args.length % 2 == 1

    hsh = new()
    while args.length >= 2
      k = args.shift
      v = args.shift
      hsh[k] = v
    end
    hsh
  end

  def initialize(default = Undefined, &block)
    @keys = Tuple.new(16)
    @values = Tuple.new(16)
    @bins = 16
    @entries = 0
    
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
    replace(other)
  end
  private :initialize_copy

  def ==(other)
    return true if self.equal? other
    return false unless other.kind_of? Hash or other.respond_to? :to_hash
    other = Type.coerce_to(other, Hash, :to_hash)
    return false unless other.size == size
    # Pickaxe claims that defaults are compared, but MRI 1.8.[46] doesn't actually do that
    # return false unless other.default == default
    each_pair do |k, v|
      return false unless other[k] == self[k]
    end
    true
  end

  def get_key_cv(key)
    hsh = key.hash
    bin = hsh % @bins

    entry = @values.at(bin)

    while entry
      cur_hash, cur_key, cur_val, nxt = *entry

      # Check if this entry is for the key in question
      if cur_hash == hsh and key.eql?(cur_key)
        return cur_val
      end

      entry = nxt
    end

    return default(key) if @default
    nil
  end

  def set_key_cv(key, val)
    key = key.dup if key.kind_of?(String)

    hsh = key.hash
    bin = hsh % @bins

    entry = @values.at(bin)
    lst = nil

    while entry
      cur_hash, cur_key, cur_val, nxt = *entry

      # Check if this entry is for the key in question
      if cur_hash == hsh and key.eql?(cur_key)
        entry.put 2, val
        return val
      end

      lst = entry
      entry = nxt
    end

    entry = Tuple.new(4)
    entry.put 0, hsh
    entry.put 1, key
    entry.put 2, val
    entry.put 3, nil

    if lst
      lst.put 3, entry
    else
      @values.put bin, entry
    end

    @entries += 1
    return val
  end
  alias_method :store, :set_key_cv

  def self.after_loaded()
    alias_method :[],  :get_key_cv
    alias_method :[]=, :set_key_cv
  end

  def clear()
    delete_if { true }
  end

  def clone
    new_hash = dup
    new_hash.freeze if self.frozen?
    new_hash
  end

  def default(key = Undefined)
    # current MRI documentation comment is wrong.  Actual behavior is:
    # Hash.new { 1 }.default #=> nil
    if @default_proc
      return key.equal?(Undefined) ? nil : @default.call(self, key)
    else
      return @default
    end
  end

  def default=(val)
    @default_proc = false
    @default = val
  end

  def default_proc()
    return @default if @default_proc
    nil
  end

  def delete(key)
    hsh = key.hash
    bin = hsh % @bins

    entry = @values.at(bin)
    lst = nil

    while entry
      cur_hash, cur_key, val, nxt = *entry

      # Check if this entry is for the key in question
      if cur_hash == hsh and key.eql?(cur_key)

        # Ok, relink the other entries, leaving this one out.
        if lst
          lst.put 3, nxt
        else
          @values.put bin, nxt
        end

        @entries = @entries - 1

        return val
      end

      lst = entry
      entry = nxt
    end
    
    return yield(key) if block_given?
    nil
  end

  def delete_if()
    raise LocalJumpError, "no block given" unless block_given? or empty?

    # Do this in 2 steps, so we're not altering the structure while we walk it.
    # TODO: I'd like to write it like this:
    # select(&block).each { |k, v| delete k }
    to_del = []
    each_pair { |k, v| to_del << k if yield(k, v) }
    to_del.each { |k| delete k }
    self
  end

  def dup
    new_hash = self.class.new
    new_hash.send :initialize_copy, self
    new_hash.taint if self.tainted?
    new_hash
  end

  def each_key(&block)
    keys.each(&block)
    self
  end

  def each
    raise LocalJumpError, "no block given" unless block_given? or empty?

    @values.each do |tup|
      while tup
        yield([tup.at(1), tup.at(2)])
        tup = tup.at(3)
      end
    end
    self
  end

  def each_pair
    raise LocalJumpError, "no block given" unless block_given? or empty?

    @values.each do |tup|
      while tup
        yield(tup.at(1), tup.at(2))
        tup = tup.at(3)
      end
    end
    self
  end

  def each_value(&block)
    values.each(&block)
    self
  end

  def empty?()
    @entries == 0
  end

  def fetch(key, default = Undefined)
    found, val = find_unambiguous key
    return val if found

    return yield(key) if block_given?
    return default if !default.equal?(Undefined)
    raise IndexError, 'Key not found'
  end

  def index(val)
    each_pair { |k, v| return k if v == val }
    nil
  end

  def inspect()
    # recursively_inspect
    return '{...}' if RecursionGuard.inspecting?(self)
    
    out = []
    RecursionGuard.inspect(self) do
      each_pair do |key, val|
        str =  key.inspect
        str << '=>'
        str << val.inspect
        out << str
      end
    end
    
    "{#{out.join ', '}}"
  end

  def invert()
    out = {}
    each_pair { |k, v| out[v] = k }
    out
  end

  def key?(key)
    found, val = find_unambiguous key
    found
  end
  alias_method :has_key?, :key?
  alias_method :include?, :key?
  alias_method :member?, :key?

  def keys()
    out = []
    @values.each do |tup|
      while tup
        out << tup.at(1)
        tup = tup.at(3)
      end
    end
    out
  end

  def merge(other, &block)
    dup.merge!(other, &block)
  end
  
  def merge!(other)
    other = Type.coerce_to(other, Hash, :to_hash)
    other.each_pair do |k, v|
      if block_given? and self.key? k
        self[k] = yield(k, self[k], v)
      else
        self[k] = v
      end
    end
    self
  end
  alias_method :update, :merge!

  def rehash()
    out = {}
    each_pair { |k, v| out[k] = v }
    replace out
  end
  
  def reject(&block)
    dup.delete_if(&block)
  end
  
  def reject!(&block)
    old_size = size
    delete_if(&block)
    return nil if old_size == size
    self
  end
  
  def replace(other)
    other = Type.coerce_to(other, Hash, :to_hash)
    return self if self.equal? other
    clear
    other.each_pair { |k, v| self[k] = v }
    if other.default_proc
      @default = other.default_proc
      @default_proc = true
    else
      @default = other.default
      @default_proc = false
    end
    self
  end

  def select()
    raise LocalJumpError, "no block given" unless block_given? or empty?

    out = []
    each_pair { |k, v| out << [k, v] if yield(k, v) }
    out
  end

  def shift()
    return default(nil) if empty?

    # TODO: keep around for efficiency?  It's not much faster though.
    # i = 0
    # tup = nil
    # while !tup
    #   tup = @values.at(i)
    #   i += 1
    # end
    # key = tup.at(1)
    # out = [key, self[key]]
    # delete(key)
    # out

    key = keys.first
    out = [key, self[key]]
    delete key
    out
  end

  def size()
    @entries
  end
  alias_method :length, :size

  def sort(&block)
    to_a.sort(&block)
  end

  def to_a()
    select { true }
  end

  def to_hash()
    self
  end

  def to_s()
    to_a.join
  end

  def value?(val)
    values.include?(val)
  end
  alias_method :has_value?, :value?

  def values()
    out = []
    @values.each do |tup|
      while tup
        out << tup.at(2)
        tup = tup.at(3)
      end
    end
    out
  end

  def values_at(*args)
    args.collect { |key| self[key] }
  end
  alias_method :indexes, :values_at
  alias_method :indices, :values_at
  
  def find_unambiguous(key)
    code, hk, val, nxt = get_by_hash key.hash, key
    return Tuple[true, val] if code
    Tuple[false, nil]
  end
end
