require 'mspec/runner/actions/filter'

class GdbAction < ActionFilter
  def before(state)
    Kernel.yield_gdb(true) if @filter === state.description
  end
  
  def register
    MSpec.register :before, self
  end
  
  def unregister
    MSpec.unregister :before, self
  end
end
