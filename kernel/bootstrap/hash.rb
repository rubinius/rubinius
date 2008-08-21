# Requires: Object#hash

class Hash

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
    @bins = 16
    @values = Tuple.new(@bins)
    @entries = 0
  end

  def set_by_hash(hsh, key, val)
    key = key.dup if key.kind_of?(String)

    #redistribute # HACK

    tmp = hash_entry key # HACK splat not working
    entry = tmp[0]
    hash = tmp[1]
    bin = tmp[2]

    lst = nil

    while entry
      cur_hash = entry[0]
      cur_key = entry[1]
      cur_val = entry[2]
      nxt = entry[3]

      cur_hash == hash
      key.eql? cur_key

      # Check if this entry is for the key in question
      if cur_hash == hash and key.eql?(cur_key)
        entry.put 2, val
        return val
      end

      lst = entry
      entry = nxt
    end

    entry = Tuple.new(4)
    entry.put 0, hash
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

  def get_by_hash(hsh, key)
    Ruby.primitive :hash_get
    if hsh.kind_of? Integer
      # This magic value is the fixnum max.
      return get_by_hash(hsh % 536870911, key)
    end

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

  def redistribute
    Ruby.primitive :hash_redistribute
    raise PrimitiveFailure, "Hash#redistribute failed"
  end

  def delete_by_hash(hsh, key)
    Ruby.primitive :hash_delete
    raise PrimitiveFailure, "Hash#delete_by_hash failed"
  end

  def each
    i = 0
    while i < @values.fields
      tup = @values[i]
      while tup
        yield tup.at(1), tup.at(2)
        tup = tup.at(3)
      end
      i += 1
    end
    self
  end

  def hash_entry(obj)
    hash = obj.hash
    hash = hash % HASH_MAX unless hash.kind_of? Fixnum

    bin = hash & (@bins - 1)

    entry = @values.at bin

    return entry, hash, bin
  end

end
