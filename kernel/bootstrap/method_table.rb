class MethodTable
  def names(strings=true)
    return keys.to_a unless strings
    ary = []
    keys.each do |ent|
      ary << ent.to_s unless ent.nil?
    end
    return ary
  end
  
  alias :to_a :names
end
