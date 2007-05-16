class Float
  def __ivars__; Ruby.asm "push self\npush 0\nfetch_field"; end
  ivar_as_index :__ivars__ => 0
end

