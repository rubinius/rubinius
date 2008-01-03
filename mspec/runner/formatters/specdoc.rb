require 'mspec/expectations'
require 'mspec/runner/formatters/base'

class SpecdocFormatter < BaseFormatter
  def before_describe(msg)
    super
    @describe_printed = false
  end
  
  def print_describe
    unless @describe_printed
      @out.print "\n" + @describe.to_s + "\n"
      @describe_printed = true
    end
  end
  
  def before_it(msg)
    super
    print_describe
    @out.print "- " + msg
  end
  
  def after_it(msg)
    @out.print "\n"
  end
  
  def exception(e)
    super
    count = @tally.failures + @tally.errors
    if failure?(e)
      @out.print " (FAILED - #{count})"
    else
      @out.print " (ERROR - #{count})"
    end
  end
end
