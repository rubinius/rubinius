module Benchmark
  module Helpers

    def fixnum_max
      if Object.const_defined?(:RUBY_ENGINE)
        case RUBY_ENGINE
        when "ruby"
          2 ** (wordsize - 2) - 1
        when "rbx"
          Fixnum::MAX
        when "jruby"
          9223372036854775807
        else
          raise "Maximum Fixnum size now known yet for #{RUBY_ENGINE}"
        end
      else
        2 ** (wordsize - 2) - 1
      end
    end
    module_function :fixnum_max

    def fixnum_min
       if Object.const_defined?(:RUBY_ENGINE)
        case RUBY_ENGINE
        when "ruby"
          - 2 ** (wordsize - 2)
        when "rbx"
          Fixnum::MIN
        when "jruby"
          -9223372036854775808
        else
          raise "Maximum Fixnum size now known yet for #{RUBY_ENGINE}"
        end
      else
        - 2 ** (wordsize - 2)
      end
   end
    module_function :fixnum_min

    def wordsize
      8 * 1.size
    end
    module_function :wordsize

  end
end
