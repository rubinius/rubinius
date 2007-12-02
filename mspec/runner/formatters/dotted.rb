require 'mspec/expectations'
require 'mspec/runner/formatters/base'

class DottedFormatter < BaseFormatter
  def after_it(msg)
    if @report.exception
      if @report.exception.is_a?(ExpectationNotMetError)
        @out.print 'F'
      else
        @out.print 'E'
      end
    else
      @out.print '.'
    end
  end
end
