require 'mspec/expectations'
require 'mspec/runner/actions/timer'
require 'mspec/runner/actions/tally'

class SpinnerFormatter
  attr_reader :timer, :tally

  Spins = %w!| / - \\!
  
  def initialize
    @states = []
    @which = 0
    @errors = 0
    @failures = 0
    @total = 0
  end

  def register
    @timer = TimerAction.new
    @timer.register
    @tally = TallyAction.new
    @tally.register
    
    MSpec.register :after, self
    MSpec.register :finish, self
  end

  def spin
    @which = (@which + 1) % Spins.size
    print "\r    [%s] \033[0;31m%6dF \033[0;33m%6dE\033[0m" % [Spins[@which], @failures, @errors]
  end
  
  def after(state)
    @total += 1
    if state.exception?
      @states << state
      state.exceptions.each do |msg, exc|
        if state.failure?(exc)
          @failures += 1
        else
          @errors += 1
        end
      end
    end
      
    spin
  end
  
  def finish
    print "\n"
    count = 0
    @states.each do |state|
      state.exceptions.each do |msg, exc|
        outcome = failure?(state) ? "FAILED" : "ERROR"
        print "\n#{count += 1})\n#{state.description} #{outcome}\n"
        print "Exception occurred during: #{msg}\n" if msg
        print (exc.message.empty? ? "<No message>" : exc.message) + ": \n"
        print backtrace(exc)
        print "\n"
      end
    end
    print "\n#{@timer.format}\n\n#{@tally.format}\n"
  end
    
  def failure?(state)
    state.exceptions.all? { |msg, exc| state.failure?(exc) }
  end
  private :failure?
  
  def backtrace(exc)
    begin
      return exc.awesome_backtrace.show
    rescue Exception
      return exc.backtrace && exc.backtrace.join("\n")
    end
  end
  private :backtrace
end
