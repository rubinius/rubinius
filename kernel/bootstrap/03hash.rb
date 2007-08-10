class Hash
  ivar_as_index :__ivars__ => 0, :keys => 1, :values => 2, :bins => 3, :entries => 4, :default => 5, :default_proc => 6
  def __ivars__   ; @__ivars__    ; end
  def keys        ; @keys         ; end
  def values      ; @values       ; end
  def bins        ; @bins         ; end
  def entries     ; @entries      ; end
  def default_proc; @default_proc ; end
  
  def self.new(default_value=nil,&block)
    hsh = {}
    raise ArgumentError, 'wrong number of arguments' if default_value && block
    hsh.put(5, (default_value || block))
    hsh.put(6, (block != nil))
    return hsh
  end
  
  def get_by_hash(hsh, key)
    Ruby.primitive :hash_get
    exc = ArgumentError.new("Unable to fetch hash element for '#{key}' (#{hsh})")
    raise exc
  end
  
  def set_by_hash(hsh, key, val)
    Ruby.primitive :hash_set
  end
  
  def delete_by_hash(hsh, key)
    Ruby.primitive :hash_delete
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
  
  def [](key)
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
  
  def []=(key, val)
    set_by_hash key.hash, key, val
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
end
