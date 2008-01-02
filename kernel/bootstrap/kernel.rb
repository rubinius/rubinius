module Kernel
  def yield_gdb(obj)
    Ruby.primitive :yield_gdb
  end
end
