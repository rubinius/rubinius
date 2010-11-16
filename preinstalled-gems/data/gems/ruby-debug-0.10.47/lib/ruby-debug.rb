require 'debugger'

module Kernel

  # Enters the debugger in the current thread.
  #
  # +steps+ is currently ignored.
  def debugger(steps = 1)
    Debugger.global.start(1)
  end
  alias breakpoint debugger unless respond_to?(:breakpoint)
end
