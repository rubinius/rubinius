require 'mspec/expectations'
require 'mspec/runner/formatters/dotted'

class SpecdocFormatter < DottedFormatter
  def after(state)
    unless @describe == state.describe
      print "#{@describe = state.describe}\n"
    end
    desc = "- #{state.it}"
    if state.exception?
      @states << state
      count = @tally.failures + @tally.errors - state.exceptions.size
      state.exceptions.each do |exc|
        outcome = state.failure?(exc) ? "FAILED" : "ERROR"
        print "#{desc} (#{outcome} - #{count += 1})\n"
      end
    else
      print "#{desc}\n"
    end
  end
end
