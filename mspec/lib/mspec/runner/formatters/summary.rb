require 'mspec/expectations/expectations'
require 'mspec/runner/formatters/dotted'

class SummaryFormatter < DottedFormatter
  def after(state)
    @states << state if state.exception?
  end
end
