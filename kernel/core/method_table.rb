class MethodTable

  def public_names
    filter_names :public
  end
  
  def private_names
    filter_names :private
  end
  
  def protected_names
    filter_names :protected
  end
  
  alias_method :to_a, :public_names
  
  def filter_names(filter, format=:to_s)
    ary = Array.new
    keys.each do |meth|
      m = self[meth]

      if m.is_a?(AccessVarMethod) || m && m.first == filter
        ary << meth.__send__(format)
      end
    end
    return ary
  end
end
