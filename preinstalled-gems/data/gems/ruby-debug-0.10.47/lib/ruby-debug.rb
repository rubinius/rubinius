require 'rubinius/debugger'

module Debugger
  def self.start(options=nil)
    # a noop, started automatically when needed
    @started = true
    yield self if block_given?
  end

  def self.started?
    @started
  end

  def self.stop
    # noop
  end
end

module Kernel

  # Enters the debugger in the current thread.
  #
  # +steps+ is currently ignored.
  def debugger(steps = 1)
    Rubinius::Debugger.global.start(1)
  end
  alias breakpoint debugger unless respond_to?(:breakpoint)
end
