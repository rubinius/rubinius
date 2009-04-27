##
# Interface to the custom ruby dtrace(1) probe.

module Dtrace
  def self.fire(arg1 = "", arg2 = "")
    do_fire arg1, arg2
  end

  def self.do_fire(arg1, arg2)
    Ruby.primitive :dtrace_fire_ruby_probe
    raise PrimitiveFailure, "primitive failed"      
  end
end
