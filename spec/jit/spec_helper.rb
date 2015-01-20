require File.expand_path('../../spec_helper', __FILE__)

class Object
  def jit(obj, sym, block=nil)
    yield if block_given?

    Rubinius::JIT.compile obj, obj.method(sym).executable, block
  end
end
