class DebugAction < ActionFilter
  def initialize(tags=nil, descs=nil)
    super
    require 'rubinius/debugger'
  end

  def before(state)
    Rubinius::Debugger.start if self === state.description
  end

  def register
    MSpec.register :before, self
  end

  def unregister
    MSpec.unregister :before, self
  end
end
