class MethodTable
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  def keys; Ruby.asm "push self\npush 1\nfetch_field"; end
  def values; Ruby.asm "push self\npush 2\nfetch_field"; end
  def bins; Ruby.asm "push self\npush 3\nfetch_field"; end
  def entries; Ruby.asm "push self\npush 4\nfetch_field"; end
  def default; Ruby.asm "push self\npush 5\nfetch_field"; end
  def default_proc; Ruby.asm "push self\npush 6\nfetch_field"; end
  ivar_as_index :__ivars__ => 0, :keys => 1, :values => 2, :bins => 3, :entries => 4, :default => 5, :default_proc => 6
  
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
