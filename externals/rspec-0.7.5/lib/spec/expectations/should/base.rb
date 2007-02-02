module Spec
  module Expectations
    module Should
      class Base
        
        def <(expected)
          __delegate_method_missing_to_target "<", "<", expected
        end
        
        def <=(expected)
          __delegate_method_missing_to_target "<=", "<=", expected
        end
        
        def ==(expected)
          __delegate_method_missing_to_target "==", "==", expected
        end
        
        def =~(expected)
          __delegate_method_missing_to_target "=~", "=~", expected
        end
        
        def >=(expected)
          __delegate_method_missing_to_target ">=", ">=", expected
        end
        
        def >(expected)
          __delegate_method_missing_to_target ">", ">", expected
        end

        def default_message(expectation, expected=nil)
          Spec::Expectations.build_message(@target, expectation, expected)
         end

        def fail_with_message(message)
          Spec::Expectations.fail_with(message)
        end
    
        def find_supported_sym(original_sym)
          ["#{original_sym}?", "#{original_sym}s?"].each do |alternate_sym|
            return alternate_sym.to_s if @target.respond_to?(alternate_sym.to_s)
          end
          return ["<","<=",">=",">","==","=~"].include?(original_sym) ? original_sym : "#{original_sym}?"
        end

        def method_missing(original_sym, *args, &block)
          if original_sym.to_s =~ /^not_/
            return Not.new(@target).__send__(original_sym.to_s[4..-1].to_sym, *args, &block)
          end
          if original_sym.to_s =~ /^be_/
            @be_seen = true
            return __send__(original_sym.to_s[3..-1].to_sym, *args, &block)
          end
          if original_sym.to_s =~ /^have_/
            return have.__send__(original_sym.to_s[5..-1].to_sym, *args, &block)
          end
          __delegate_method_missing_to_target original_sym, find_supported_sym(original_sym), *args
        end
      end
    end
  end
end
