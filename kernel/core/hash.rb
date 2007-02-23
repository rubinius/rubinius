class Hash
  include Enumerable
  alias :has_key?   :key?
  alias :include?   :key?
  alias :member?    :key?
  alias :each_pair  :each
  alias :length     :size
  alias :store      :[]=

  def self.[](*args)
    unless args.size % 2 == 0 or (args.size == 1 and args[0].is_a?(Hash))
      raise ArgumentError, "odd number of arguments for Hash" 
    end
    hsh = {}
    while args.size >= 2
      k = args.shift
      v = args.shift
      hsh[k] = v
    end
    if args.size == 1
      hsh.merge! args[0]
    end
    hsh
  end

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
  alias :has_value? :value?
  
  def delete_by_hash(hsh, key)
    Ruby.primitive :hash_delete
  end
  
  def delete(key)
    retval = nil
    out = get_by_hash key.hash, key
    if out.undef?
      retval = yield(key) if block_given?
    else
      retval = delete_by_hash key.hash, key
    end
    retval    
  end
  
  def delete_if
    each {|k, v| delete(k) if yield(k, v)}
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
  
  def merge!(other_hash)
   other_hash.each do |k, v| 
     if block_given? && self.key?(k)
       self[k] = yield(k, self[k], v)
     else
       self[k] = v
     end
   end
    self
  end
  alias :update :merge!
  
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
  alias :indexes :values_at
  alias :indices :values_at
  
  def merge(other_hash, &block)
    dup.merge!(other_hash, &block)
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
  
  def replace(other_hash)
    unless self.equal?(other_hash)
      clear
      other_hash.each {|k, v| self[k] = v}
      self.default = other_hash.default
    end
    self
  end
  alias :initialize_copy :replace
  
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
    each {|k, v| return false unless other.get_by_hash(k.hash, k) == get_by_hash(k.hash, k)}
    true
  end
  
  def dup
    {}.replace(self)
  end
  def fetch(key, *rest)
    raise ArgumentError, "wrong number of arguments (#{rest.size + 1} for 2)" if rest.size > 1
    val = get_by_hash(key.hash, key)
    if val.undef?
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
