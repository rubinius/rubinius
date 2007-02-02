module Spec
  module Expectations
    module Should # :nodoc:
      class Should < Base

        def initialize(target)
          @target = target
          @be_seen = false
        end
        
        def have(expected_number=nil)
          Have.new(@target, :exactly, expected_number)
        end

        def change(receiver=nil, message=nil, &block)
          Change.new(@target, receiver, message, &block)
        end

        def not
          Not.new(@target)
        end
      
        def satisfy
          return if yield(@target)
          fail_with_message "Supplied expectation was not satisfied"
        end
      
        def be(expected = :___no_arg)
          @be_seen = true
          return self if (expected == :___no_arg)
          fail_with_message(default_message("should be", expected)) unless (@target.equal?(expected))
        end

        def an_instance_of(expected_class)
          fail_with_message(default_message("should be an instance of", expected_class)) unless @target.instance_of? expected_class
        end
  
        def a_kind_of(expected_class)
          fail_with_message(default_message("should be a kind of", expected_class)) unless @target.kind_of? expected_class
        end
  
        def respond_to(message)
          fail_with_message(default_message("should respond to", message)) unless @target.respond_to? message
        end
  
        def __delegate_method_missing_to_target(original_sym, actual_sym, *args)
          return if @target.send(actual_sym, *args)
          message = default_message("should#{@be_seen ? ' be' : ''} #{original_sym}", args[0])
          fail_with_message(message)
        end

        def match(expected)
           fail_with_message(default_message("should match", expected)) unless (@target =~ expected)
        end

        def raise(exception=Exception, message=nil)
          begin
            @target.call
          rescue exception => e
            unless message.nil?
              if message.is_a?(Regexp)
                e.message.should =~ message
              else
                e.message.should == message
              end
            end
            return
          rescue => e
            fail_with_message("#{default_message("should raise", exception)} but raised #{e.inspect}")
          end
          fail_with_message("#{default_message("should raise", exception)} but raised nothing")
        end
  
        def throw(symbol)
          begin
            catch symbol do
              @target.call
              fail_with_message(default_message("should throw", symbol.inspect))
            end
          rescue NameError
            fail_with_message(default_message("should throw", symbol.inspect))
          end
        end
      end
    end
  end
end
