class Hash
  def self.new(default=nil,&block)
    hsh = {}
    hsh.put 5, (default or block)
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
  
  def default
    at(5)
  end
  
  def [](key)
    out = get_by_hash key.hash, key
    if out.undef?
      dfl = self.default
      return nil unless dfl
      if BlockContext === dfl
        out = dfl.call(self, key)
      else
        out = dfl
        self[key] = out
      end
    end
    return out
  end
  
  def []=(key, val)
    set_by_hash key.hash, key, val
  end
  
  def values_data
    at(2)
  end
  
  def keys
    out = []
    values_data.each do |tup|
      while tup
        out << tup.at(1)
        tup = tup.at(3)
      end
    end
    return out
  end
  
  def values
    out = []
    values_data.each do |tup|
      while tup
        out << tup.at(2)
        tup = tup.at(3)
      end
    end
    return out
  end
  
  def each
    values_data.each do |tup|
      while tup
        yield tup.at(0), tup.at(1)
        tup = tup.at(3)
      end
    end
    return self
  end
  
  def inspect
    ary = []
    values_data.each do |tup|
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