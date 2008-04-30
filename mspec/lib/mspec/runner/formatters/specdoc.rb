require 'mspec/expectations/expectations'
require 'mspec/runner/formatters/dotted'

class SpecdocFormatter < DottedFormatter
  def register
    super
    MSpec.register :enter, self
  end

  def enter(describe)
    print "\n#{describe}\n"
  end

  def after(state)
    desc = "- #{state.it}"
    if state.exception?
      @states << state
      count = @counter.failures + @counter.errors - state.exceptions.size
      state.exceptions.each do |msg, exc|
        outcome = state.failure?(exc) ? "FAILED" : "ERROR"
        print "#{desc} (#{outcome} - #{count += 1})\n"
      end
    else
      print "#{desc}\n"
    end
  end
end
