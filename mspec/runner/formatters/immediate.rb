require 'mspec/expectations'
require 'mspec/runner/formatters/base'

class ImmediateFormatter < BaseFormatter
  def after_it(msg)
    if @current.exception
      @out.print 'F'
      @out.print "\n" + @current.describe + " " + @current.it + " FAILED\n"
      print_backtrace(@current.exception)
    else
      @out.print '.'
    end
  end
  
  def summary
    unless @summarized
      @out.print "\n\n"
      print_summary
      @summarized = true
    end
  end
end
