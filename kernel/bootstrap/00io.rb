class IO
  def descriptor; Ruby.asm "push self\npush 0\nfetch_field"; end
  def __ivars__; nil; end
  ivar_as_index :descriptor => 0

  def write(str)
    Ruby.primitive :io_write
    exc = IOError.new("Unable to write '#{str}' via #{self}")
    raise exc
  end
end

