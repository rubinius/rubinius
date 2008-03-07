class LookupTable
  ivar_as_index :values => 1, :bins => 2, :entries => 3
  def values;  @values  ; end
  def bins;    @bins    ; end
  def size;    @entries ; end

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

  # This method is required by MethodTable until
  # that can be fixed, then this can be removed.
  def setup
    @values = Tuple.new 16
    @bins = 16
    @entries = 0
  end

  def dup
    Ruby.primitive :lookuptable_dup
    raise PrimitiveFailure, "LookupTable#dup primitive failed"
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

  def entries
    Ruby.primitive :lookuptable_entries
    raise PrimitiveFailure, "LookupTable#entries primitive failed"
  end

  def each
    raise LocalJumpError, "no block given" unless block_given? or @entries == 0

    entries.each do |entry|
      yield [entry.at(0), entry.at(1)]
    end
    self
  end
end
