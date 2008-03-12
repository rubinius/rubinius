require 'mspec/expectations'
require 'mspec/runner/formatters/dotted'

class SpinnerFormatter < DottedFormatter
  attr_reader :length

  Spins = %w!| / - \\!
  
  def initialize
    @states = []
    @which = 0
    @count = 0
    self.length = 40
  end

  def register
    super
    
    MSpec.register :start, self
    MSpec.register :load, self
  end

  def length=(length)
    @length = length
    @ratio = 100.0 / length
    @position = length / 2 - 2
  end

  def percentage
    percent = @count * 100 / @total
    bar = ("=" * (percent / @ratio)).ljust @length
    label = "%d%%" % percent
    bar[@position, label.size] = label
    bar
  end

  def spin
    @which = (@which + 1) % Spins.size
    print "\r    [%s] \033[0;31m%6dF \033[0;33m%6dE\033[0m [%s]" %
          [Spins[@which], @tally.failures, @tally.errors, percentage]
  end

  def start
    @total = MSpec.retrieve(:files).size
  end

  def load
    @count += 1
  end

  def after(state)
    if state.exception?
      @states << state
    end

    spin
  end
end
