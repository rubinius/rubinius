class Hash
  ivar_as_index :__ivars__ => 0, :keys => 1, :values => 2, :bins => 3, :entries => 4
  def __ivars__   ; @__ivars__    ; end
  def keys        ; @keys         ; end
  def values      ; @values       ; end
  def bins        ; @bins         ; end
  def entries     ; @entries      ; end

  def self.allocate()
    result = super
    bins = 11
    result.put(1, Tuple.new(bins))
    result.put(2, Tuple.new(bins))
    result.put(3, bins)
    result.put(4, 0)
    return result
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
  
  def find_unambigious(key)
    code, hk, val, nxt = get_by_hash key.hash, key
    if code
      return Tuple[true, val]
    else
      return Tuple[false, nil]
    end            
  end

  def keys()
    out = []
    @values.each do |tup|
      while tup
        out << tup.at(1)
        tup = tup.at(3)
      end
    end
    return out
  end
  
  def fetch(key, *rest)
    raise ArgumentError, "wrong number of arguments (#{rest.size + 1} for 2)" if rest.size > 1

    found, val = find_unambigious(key)

    unless found
      if block_given?
        val = yield(key)
      elsif rest.size == 1
        val = rest.first
      else
        raise IndexError, 'key not found'
      end
    end
    
    return val
  end

  def []=(key, val)
    set_by_hash key.hash, key, val
  end

  def delete(key)
    found, val = find_unambigious(key)
    
    if found
      delete_by_hash(key.hash, key)
    elsif block_given?
      val = yield(key)
    end
    
    return val
  end
  
  # Note: This is a minimal version without default handling
  def [](key)
    code, hk, val, nxt = get_by_hash key.hash, key
    
    return val if hk.eql? key
  end
  
  def key?(key)
    tup = get_by_hash key.hash, key
    return tup ? true : false
  end
  
  def inspect()
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
