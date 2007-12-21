module BreakpointSpecs

  class Debuggee
    def simple_method
      a = 10
      b = 45
      c = a * b
      puts "#{a} * #{b} = #{c}"
    end
  end

end
