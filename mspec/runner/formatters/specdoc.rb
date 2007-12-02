require File.dirname(__FILE__) + '/../../expectations'
require File.dirname(__FILE__) + '/base'

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
    if e.is_a?(ExpectationNotMetError)
      @out.print " (FAILED - " + @failures.to_s + ")"
    else
      @out.print " (ERROR - " + @failures.to_s + ")"
    end
  end
end
