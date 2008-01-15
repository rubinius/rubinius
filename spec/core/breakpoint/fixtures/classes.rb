module BreakpointSpecs

  class Debuggee
    cm = def simple_method
      a = 10
      b = 45
      c = a * b
      puts "#{a} * #{b} = #{c}"
    end

    # Need to do this because the specs modify the instruction sequence
    @orig_bytecodes = cm.bytecodes.dup

    def self.orig_bytecodes
      @orig_bytecodes
    end
  end

end
