require 'mspec/expectations/expectations'
require 'mspec/runner/formatters/dotted'

class UnitdiffFormatter < DottedFormatter
  def finish
    print "\n\n#{@timer.format}\n"
    count = 0
    @states.each do |state|
      state.exceptions.each do |msg, exc|
        outcome = failure?(state) ? "FAILED" : "ERROR"
        print "\n#{count += 1})\n#{state.description} #{outcome}\n"
        print "Exception occurred during: #{msg}\n" if msg
        print((exc.message.empty? ? "<No message>" : exc.message) + ": \n")
        print backtrace(exc)
        print "\n"
      end
    end
    print "\n#{@tally.format}\n"
  end

  def backtrace(exc)
    exc.backtrace && exc.backtrace.join("\n")
  end
  private :backtrace
end
