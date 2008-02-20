require 'mspec/runner/actions/filter'

class DebugAction < ActionFilter
  def before(state)
    Kernel.debugger if self === state.description
  end
  
  def register
    MSpec.register :before, self
  end
  
  def unregister
    MSpec.unregister :before, self
  end
end
