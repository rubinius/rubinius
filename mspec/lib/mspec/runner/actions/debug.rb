require 'mspec/runner/actions/filter'

class DebugAction < ActionFilter
  def initialize(tags=nil, descs=nil)
    super

    if defined?(RUBY_ENGINE) and RUBY_ENGINE == "rbx"
      require 'debugger'
      @use_rubinius = true
    else
      @use_rubinius = false
    end
  end

  def before(state)
    if self === state.description
      if @use_rubinius
        Debugger.here
      else
        Kernel.debugger
      end
    end
  end

  def register
    super
    MSpec.register :before, self
  end

  def unregister
    super
    MSpec.unregister :before, self
  end
end
