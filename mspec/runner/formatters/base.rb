class SpecExecution
  def exception=(e)
    @exception = e
  end
  
  def exception
    @exception
  end
  
  def describe=(d)
    @describe = d.to_s
  end
  
  def describe
    @describe
  end
  
  def it=(i)
    @it = i.to_s
  end
  
  def it
    @it
  end
  
  def full_message
    "#{@describe} #{@it}"
  end
end

class BaseFormatter
  def initialize(out=$stdout)
    self.out = out
    @examples = 0
    @failures = 0
    @current = SpecExecution.new
    @exceptions = []
  end
  
  def out=(out)
    if out.respond_to?(:print)
      @out = out
    else
      @out = File.open(out, "w")
    end
  end
  
  def out
    @out
  end
  
  def current
    @current
  end
  
  def start_timer
    @start = Time.now
  end
  
  def stop_timer
    "%f" % (Time.now - @start)
  end
  
  def before_describe(msg)
    @describe = msg
  end
  
  def after_describe(msg) end
  
  def before_it(msg)
    @current = SpecExecution.new
    @current.describe = @describe
    @current.it = msg
    @examples += 1
  end
  
  def after_it(msg) end

  def exception(e)
    @failures += 1
    @current.exception = e
    @exceptions.push(@current)
  end

  def summarized=(flag)
    @summarized = flag
  end
  
  def print_start
    @out.print "Started\n"
  end
  
  def print_failure(i,r)
    @out.print i.to_s + ")\n" + r.describe + " " + r.it + " FAILED\n"
  end
  
  def print_backtrace(e)
    if e.message != ""
      @out.print e.message + ": \n"

      begin
        @out.print e.backtrace.show
      rescue Exception
        bt = e.backtrace
        if bt
          @out.print bt.join("\n")
        end
      end

      @out.print "\n"
    else
      @out.print "<No message>"
    end
    @out.print "\n"
  end
  
  def print_time
    @out.print "Finished in #{stop_timer} seconds\n\n"
  end

  def print_summary
    @out.print @examples.to_s + " examples, " + @failures.to_s + " failures\n"
  end
  
  def summary
    unless @summarized
      @out.print "\n\n"
      @exceptions.each_with_index do |r,i|
        print_failure(i+1,r)
        print_backtrace(r.exception)
      end
      print_time
      print_summary
      @summarized = true
    end
  end
end
