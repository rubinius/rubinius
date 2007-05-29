class MethodTable
  
  def names
    ary = []
    keys.each do |meth|
      vis, obj = self[meth]
      if vis == :public
        ary << meth
      end
    end
    
    return ary
  end
  
  def all_names
    return keys.to_a unless strings
  end
  
  alias :to_a :names
end
