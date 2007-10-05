# depends on: enumerable.rb

class Hash
  ivar_as_index :__ivars__ => 0, :keys => 1, :values => 2, :bins => 3, :entries => 4, :default => 5, :default_proc => 6

  include Enumerable

  def self.new(*args, &block)
    hsh = {} # This makes it impossible to create instances of subclasses of Hash...
    hsh.initialize(*args, &block)
    hsh
  end

  def self.[](*args)
    if args.first.kind_of? Hash and args.length == 1
      return new.replace args.first
    end

    raise ArgumentError, "Expected an even number, got #{args.length}" if args.length % 2 == 1

    hsh = {}
    while args.length >= 2
      k = args.shift
      v = args.shift
      hsh[k] = v
    end
    hsh
  end

  def initialize(*args, &block)
    if block
      raise ArgumentError, "Expected 0, got #{args.length}" if args.length > 0
      @default = block
      @default_proc = true
    else
      raise ArgumentError, "Expected 1, got #{args.length}" if args.length > 1
      @default = args.first
      @default_proc = false
    end
  end

  def dup()
    self.class.new.replace self
  end

  def access_key_cv(key)
    code, hk, val, nxt = get_by_hash key.hash, key

    unless code
      return nil unless @default
      if @default_proc
        return @default.call(self, key)
      else
        return @default
      end
    end

    if hk.eql? key
      return val
    end

    return nil
  end

  def modify_key_cv(key, val)
    key = key.dup.freeze if String === key
    set_by_hash key.hash, key, val
  end

  def self.after_loaded
    alias_method :[],  :access_key_cv
    alias_method :[]=, :modify_key_cv
  end

  def default(key = nil)
    @default_proc ? @default.call(self, key) : @default
  end

  def find_unambigious(key)
    code, hk, val, nxt = get_by_hash key.hash, key
    if code
      return Tuple[true, val]
    else
      return Tuple[false, nil]
    end
  end

  def size
    @entries
  end

  def empty?
    @entries == 0
  end

  def values_data
    @values
  end

  def keys
    out = []
    @values.each do |tup|
      while tup
        out << tup.at(1)
        tup = tup.at(3)
      end
    end
    return out
  end

  def key?(key)
    tup = get_by_hash key.hash, key
    return tup ? true : false
  end

  def values
    out = []
    @values.each do |tup|
      while tup
        out << tup.at(2)
        tup = tup.at(3)
      end
    end
    return out
  end

  def each
    @values.each do |tup|
      while tup
        yield tup.at(1), tup.at(2)
        tup = tup.at(3)
      end
    end
    return self
  end

  def inspect
    ary = []
    @values.each do |tup|
      while tup
        str =  tup.at(1).inspect
        str << "=>"
        str << tup.at(2).inspect

        ary << str
        tup = tup.at(3)
      end
    end
    str = "{#{ary.join(", ")}}"
    return str
  end

  alias_method :has_key?,   :key?
  alias_method :include?,   :key?
  alias_method :member?,    :key?
  alias_method :each_pair,  :each
  alias_method :length,     :size
  alias_method :store,      :modify_key_cv

  def default_proc
    @default_proc ? @default : nil
  end

  def default=(nd)
    @default_proc = false
    @default = nd
  end

  def value?(val)
    values.include?(val)
  end
  alias_method :has_value?, :value?

  def delete(key)
    retval = nil
    found, val = find_unambigious(key)
    if found
      retval = delete_by_hash key.hash, key
    else
      retval = yield(key) if block_given?
    end
    retval
  end

  def delete_if
    # Do this in 2 steps, so we're not altering the structure
    # while we walk it.
    to_delete = []
    each { |k, v| to_delete << k if yield(k, v) }
    to_delete.each { |k| delete(k) }
    return self
  end

  def clear
    delete_if {true}
  end

  def each_key(&block)
    keys.each(&block)
    self
  end

  def each_value(&block)
    values.each(&block)
    self
  end

  def index(val)
    each {|k, v| return k if v == val}
    nil
  end

  def merge!(other)
    other_hash = Type.coerce_to(other, Hash, :to_hash)
    other_hash.each do |k, v|
      if block_given? && self.key?(k)
        self[k] = yield(k, self[k], v)
      else
        self[k] = v
      end
    end
    self
  end
  alias_method :update, :merge!

  def shift
    out = nil
    if empty?
      out = default_proc ? default_proc.call(self, nil) : default
    else
      i = 0
      tup = nil
      while !tup
        tup = @values.at(i)
        i += 1
      end
      key = tup.at(1)
      out = [key, self[key]]
      delete(key)
    end
    out
  end

  def values_at(*args)
    args.collect { |a| self[a] }
  end
  alias_method :indexes, :values_at
  alias_method :indices, :values_at

  def merge(other, &block)
    dup.merge!(other, &block)
  end

  def reject(&block)
    dup.delete_if(&block)
  end

  def reject!(&block)
    old_size = size
    delete_if(&block)
    old_size == size ? nil : self
  end

  def invert
    out = {}
    each {|k, v| out[v] = k}
    out
  end

  def to_hash
    self
  end

  def replace(other)
    other_hash = Type.coerce_to(other, Hash, :to_hash)
    unless self.equal?(other_hash)
      clear
      other_hash.each {|k, v| self[k] = v}
      # This makes sure a default proc isn't called instead of being read
      self.put(5, other_hash.at(5))
      self.put(6, other_hash.at(6))
    end
    self
  end
  alias_method :initialize_copy, :replace

  def rehash
    out = {}
    each {|k, v| out[k] = v}
    replace(out)
  end

  def ==(other)
    return true if self.equal?(other)
    unless other.kind_of?(Hash)
      return other.respond_to?(:to_hash) ? self == other.to_hash : false
    end
    return false unless other.size == size
    #pickaxe claims that defaults are compared, but MRI 1.8.4 doesn't actually do that
    #return false unless other.default == default
    each do |k, v|
      return false unless other[k] == self[k]
    end
    true
  end

  def fetch(key, *rest)
    raise ArgumentError, "wrong number of arguments (#{rest.size + 1} for 2)" if rest.size > 1
    found, val = find_unambigious(key)
    unless found
      if rest.size == 0 && !block_given?
        raise IndexError, 'key not found'
      elsif block_given?
        val = yield key
      else
        val = rest.first
      end
    end

    val
  end

  def to_a
    a = []
    each {|k,v| a << [k,v] }
    a
  end

  def to_s
    to_a.join
  end

  def sort(&block)
    to_a.sort(&block)
  end

  def find_all(&block)
    a = []
    each {|k,v| a << [k,v] if yield(k,v) }
    a
  end
  alias_method :select, :find_all
end
