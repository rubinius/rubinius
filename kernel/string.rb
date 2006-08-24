class String
  def to_s
    self
  end
  
  def <<(other)
    out = nil
    Ruby.asm "push other\npush self\nstring_append\nset out"
    return out
  end
  
  def dup
    out = nil
    Ruby.asm "push self\nstring_dup\nset out"
    return out
  end
end