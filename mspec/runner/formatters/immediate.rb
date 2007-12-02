require 'mspec/expectations'
require 'mspec/runner/formatters/base'

class ImmediateFormatter < BaseFormatter
  def after_it(msg)
    if @report.exception
      @out.print 'F'
      @out.print "\n" + @report.describe + " " + @report.it + " FAILED\n"
      print_backtrace(@report.exception)
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
