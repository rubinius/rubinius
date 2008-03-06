class LookupTable
  ivar_as_index :values => 1, :bins => 2, :entries => 3
  def values;  @values  ; end
  def bins;    @bins    ; end
  def entries; @entries ; end

  def self.allocate
    Ruby.primitive :allocate_table
    raise PrimitiveFailure, "LookupTable.allocate primitive failed"
  end
  
  def initialize(hash=nil)
    return unless hash
    hash.each do |k,v|
      self[k] = v
    end
  end
  
  def __key_error__(key)
    raise TypeError, "LookupTable keys must be strings or symbols: #{key.inspect}"
  end

  def [](key)
    Ruby.primitive :lookuptable_fetch
    __key_error__ key
  end

  def []=(key, val)
    Ruby.primitive :lookuptable_store
    __key_error__ key
  end
  
  def key?(key)
    Ruby.primitive :lookuptable_has_key
    __key_error__ key
  end

  def delete(key)
    Ruby.primitive :lookuptable_delete
    __key_error__ key
  end

  def keys
    Ruby.primitive :lookuptable_keys
    raise PrimitiveFailure, "LookupTable#keys primitive failed"
  end

  def values
    Ruby.primitive :lookuptable_values
    raise PrimitiveFailure, "LookupTable#keys primitive failed"
  end

  def each
    raise LocalJumpError, "no block given" unless block_given? or @entries == 0

    @values.each do |tup|
      while tup
        yield [tup.at(0), tup.at(1)]
        tup = tup.at(2)
      end
    end
    self
  end

  def size
    @entries
  end
end
