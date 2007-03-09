require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
module Mocks
context "PartialMockUsingMocksDirectly" do
    setup do
      
        klass=Class.new
        klass.class_eval do
          def existing_method
            :original_value
          end
        end
        @obj = klass.new
      
    end
    specify "should clear expectations on verify" do
        @obj.should_receive(:msg)
        @obj.msg
        @obj.__verify
        lambda do
          @obj.msg
        end.should_raise(NoMethodError)
      
    end
    specify "should fail when expected message is not received" do
        @obj.should_receive(:msg)
        lambda do
          @obj.__verify
        end.should_raise(MockExpectationError)
      
    end
    specify "should fail when message is received with incorrect args" do
        @obj.should_receive(:msg).with(:correct_arg)
        lambda do
          @obj.msg(:incorrect_arg)
        end.should_raise(MockExpectationError)
        @obj.msg(:correct_arg)
      
    end
    specify "should pass when expected message is received" do
        @obj.should_receive(:msg)
        @obj.msg
        @obj.__verify
      
    end
    specify "should pass when message is received with correct args" do
        @obj.should_receive(:msg).with(:correct_arg)
        @obj.msg(:correct_arg)
        @obj.__verify
      
    end
    specify "should revert to original method if existed" do
        @obj.existing_method.should equal(:original_value)
        @obj.should_receive(:existing_method).and_return(:mock_value)
        @obj.existing_method.should equal(:mock_value)
        @obj.__verify
        @obj.existing_method.should equal(:original_value)
      
    end
  
end
end
end