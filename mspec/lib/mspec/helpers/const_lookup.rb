module Kernel
  def const_lookup(c)
    c.split('::').inject(Object) { |k,n| k.const_get n }
  end
end
