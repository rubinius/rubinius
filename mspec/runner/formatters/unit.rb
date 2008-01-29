require 'mspec/expectations'
require 'mspec/runner/formatters/base'

class UnitdiffFormatter < BaseFormatter
  def after_it(msg)
    if @current.exception
      if failure? @current.exception
        @out.print 'F'
      else
        @out.print 'E'
      end
    else
      @out.print '.'
    end
  end
  
  def summary
    unless @summarized
      @out.print "\n\n"
      print_time
      @exceptions.each_with_index do |r,i|
        print_failure(i+1,r)
        print_backtrace(r.exception)
      end
      print_summary
      @summarized = true
    end
  end
end
