module BreakpointSpecs

  class Debuggee
    cm = def simple_method
      a = 10
      b = 45
      c = a * b
      puts "#{a} * #{b} = #{c}"
    end

    sm_cm = def step_method
      a = 10
      tot = 0
      1.upto(a) do |i|
        tot += a * i
        break if tot > 50
      end
      puts "Total is #{tot}"
    end

    if_cm = def if_method
      if false
        'f'
      else
        't'
        'ohoh'
      end
    end

    def initialize
      @counter = 5
    end

    attr_accessor :counter
    count_cm = def count
      @counter += 10
    end
  end


  class Debugger
    def initialize
      @debug_channel = Channel.new
      Rubinius::VM.debug_channel = @debug_channel
    end

    def wait_for_breakpoint(&prc)
      @breakpoint_listener = Thread.new do
        @debug_thread = @debug_channel.receive
        #begin
          prc.call
        #ensure
          @debug_thread.control_channel.send nil
        #end
      end
    end
  end

end
