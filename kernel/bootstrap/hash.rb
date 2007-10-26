# Requires: Object#hash

class Hash
  ivar_as_index :__ivars__ => 0, :keys => 1, :values => 2, :bins => 3, :entries => 4, :default => 5, :default_proc => 6

  def self.[](*args)
    hsh = new()
    i = 0

    while i < args.size
      k = args[i]
      i += 1

      v = args[i]
      i += 1

      hsh[k] = v
    end
    hsh
  end

  def initialize
    @keys = Tuple.new(16)
    @values = Tuple.new(16)
    @bins = 16
    @entries = 0
  end

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
