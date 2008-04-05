require 'mspec/expectations'
require 'mspec/runner/formatters/dotted'

class SpinnerFormatter < DottedFormatter
  attr_reader :length

  # This bit of conditional cruft results from MRI
  # requiring a file more than once if the path
  # component differs, causing a redefinition warning
  # when running the MSpec specs. The formatter
  # constants are needed in different specs and there
  # is not a useful way to mock constants in RSpec.
  unless const_defined? :Spins
    Spins = %w!| / - \\!
    HOUR = 3600
    MIN = 60
  end
  
  def initialize
    @states = []
    @which = 0
    @count = 0
    self.length = 40
    @percent = 0
    @start = Time.now

    term = ENV['TERM']
    @color = (term != "dumb")
    @fail_color  = "32"
    @error_color = "32"
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

  def etr
    return "00:00:00" if @percent == 0
    elapsed = Time.now - @start
    remain = (100 * elapsed / @percent) - elapsed

    hour = remain >= HOUR ? (remain / HOUR).to_i : 0
    remain -= hour * HOUR
    min = remain >= MIN ? (remain / MIN).to_i : 0
    sec = remain - min * MIN

    "%02d:%02d:%02d" % [hour, min, sec]
  end

  def percentage
    @percent = @count * 100 / @total
    bar = ("=" * (@percent / @ratio)).ljust @length
    label = "%d%%" % @percent
    bar[@position, label.size] = label
    bar
  end

  def spin
    @which = (@which + 1) % Spins.size
    if @color
      print "\r[%s | %s | %s] \033[0;#{@fail_color}m%6dF \033[0;#{@error_color}m%6dE\033[0m" %
          [Spins[@which], percentage, etr, @tally.failures, @tally.errors]
    else
      print "\r[%s | %s | %s] %6dF %6dE" %
          [Spins[@which], percentage, etr, @tally.failures, @tally.errors]
    end
  end

  def start
    @total = MSpec.retrieve(:files).size
  end

  def load
    @count += 1
  end

  def after(state)
    if state.exception?
      @fail_color =  "31" if @tally.failures > 0
      @error_color = "33" if @tally.errors > 0
      @states << state
    end

    spin
  end
end
