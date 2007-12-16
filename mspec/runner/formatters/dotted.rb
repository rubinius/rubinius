require 'mspec/expectations'
require 'mspec/runner/formatters/base'

class DottedFormatter < BaseFormatter
  def after_it(msg)
    if @current.exception
      if @current.exception.is_a?(ExpectationNotMetError)
        @out.print 'F'
      else
        @out.print 'E'
      end
    else
      @out.print '.'
    end
  end
end
