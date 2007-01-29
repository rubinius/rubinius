class Hash
  include Enumerable
  alias :has_key?   :key?
  alias :include?   :key?
  alias :member?    :key?
  alias :each_pair  :each
  alias :length     :size
  alias :store      :[]=
  
  def default_proc
    @default.kind_of?(Proc) ? @default : nil
  end
  
  def value?(val)
    self.values.include?(val)
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
    self.each {|k, v| delete(k) if yield(k, v)}
  end
  
  def clear
    self.each {|k, v| delete(k)}
  end
  
  def each_key
    self.keys.each {|k| yield k}
    self
  end
  
  def each_value
    self.values.each {|v| yield v}
    self
  end
  
  def index(val)
    self.each {|k, v| return k if v == val}
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
      if self.default_proc
        self.default_proc.call(nil)
      else
        self.default
      end
    else
      i = 0
      tup = nil
      while !tup
        tup = @values.at(i)
        i += 1
      end
      key = tup.at(1)
      out = [key, self[key]]
      self.delete(key)
    end
    out
  end
  
  def values_at(*args)
    out = []
    args.each { |a| out << self[a]}
    out
  end
  alias :indexes :values_at
  alias :indices :values_at
  
  def merge(other_hash, &block)
    hsh = self.dup
    hsh.merge!(other_hash, &block)
  end
  
  def reject(&block)
    self.dup.delete_if(&block)
  end
  
  def reject!(&block)
    old_size = self.size
    self.delete_if(&block)
    old_size == self.size ? nil : self
  end
  
  def invert
    out = {}
    self.each {|k, v| out[v] = k}
    out
  end
  
  def to_hash
    self
  end
  
  def replace(other_hash)
    unless other_hash.equal? self
      self.clear
      other_hash.each {|k, v| self[k] = v}
      #set default
    end
    self
  end
  
  def rehash
    out = {}
    self.each {|k, v| out[k] = v}
    self.replace(out)
  end
  
  def ==(other)
    return true if self.equal? other
    unless other.kind_of?(Hash)
      return other.respond_to?(:to_hash) ? self == other.to_hash : false
    end
    return false unless other.size == self.size && self.default == other.default
    self.each {|k, v| return false unless other.key?(k) && other[k] == v}
    true
  end
  
  def dup
    {}.replace(self)
  end

  def fetch(key, *rest)
    raise ArgumentError, "wrong number of arguments (#{rest.size + 1} for 2)" if rest.size > 1
    val = self.get_by_hash(key.hash, key)
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
   
end
