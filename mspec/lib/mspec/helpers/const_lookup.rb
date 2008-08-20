module Kernel
  def const_lookup(c)
    names = c.split '::'
    names.shift if names.first.empty?
    names.inject(Object) do |m, n|
      m.const_defined?(n) ? m.const_get(n) : m.const_missing(n)
    end
  end
end
