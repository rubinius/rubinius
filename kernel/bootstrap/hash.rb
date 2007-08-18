# Requires: Object#hash

class Hash
  def set_by_hash(hsh, key, val)
    Ruby.primitive :hash_set
    raise PrimitiveFailure, "Hash#set_by_hash failed."    
  end
  
  def get_by_hash(hsh, key)
    Ruby.primitive :hash_get
    raise PrimitiveFailure, "Hash#get_by_hash failed."
  end
  
  def [](key)
    code, hk, val, nxt = get_by_hash(key.hash, key)
    return nil unless code
    return val
  end
  
  def []=(key, val)
    set_by_hash key.hash, key, val
  end
end
