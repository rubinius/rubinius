require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Mocks
    context "A method stub" do
      setup do
        @class = Class.new do
          def self.existing_class_method
            :original_value
          end

          def existing_instance_method
            :original_value
          end
        end
        @obj = @class.new
      end

      specify "should allow for a mock expectation to temporarily replace a method stub on a mock" do
        mock = Spec::Mocks::Mock.new("a mock")
        mock.stub!(:msg).and_return(:stub_value)
        mock.should_receive(:msg).with(:arg).and_return(:mock_value)
        mock.msg(:arg).should equal(:mock_value)
        mock.msg.should equal(:stub_value)
        mock.msg.should equal(:stub_value)
        mock.__verify
      end

      specify "should allow for a mock expectation to temporarily replace a method stub on a non-mock" do
        @obj.stub!(:msg).and_return(:stub_value)
        @obj.should_receive(:msg).with(:arg).and_return(:mock_value)
        @obj.msg(:arg).should equal(:mock_value)
        @obj.msg.should equal(:stub_value)
        @obj.msg.should equal(:stub_value)
        @obj.__verify
      end

      specify "should ignore when expected message is not received" do
        @obj.stub!(:msg)
        lambda do
          @obj.__verify
        end.should_not_raise
      end
      
      specify "should clear itself on __verify" do
        @obj.stub!(:this_should_go).and_return(:blah)
        @obj.this_should_go.should_equal :blah
        @obj.__verify
        lambda do
          @obj.this_should_go
        end.should_raise
      end

      specify "should ignore when expected message is received" do
        @obj.stub!(:msg)
        @obj.msg
        @obj.__verify
      end

      specify "should ignore when message is received with args" do
        @obj.stub!(:msg)
        @obj.msg(:an_arg)
        @obj.__verify
      end

      specify "should not support with" do
        lambda do
          Spec::Mocks::Mock.new("a mock").stub!(:msg).with(:arg)
        end.should_raise(NoMethodError)
      end
      
      specify "should return expected value when expected message is received" do
        @obj.stub!(:msg).and_return(:return_value)
        @obj.msg.should equal(:return_value)
        @obj.__verify
      end

      specify "should return values in order to consecutive calls" do
        return_values = ["1",2,Object.new]
        @obj.stub!(:msg).and_return(return_values[0],return_values[1],return_values[2])
        @obj.msg.should_eql return_values[0]
        @obj.msg.should_eql return_values[1]
        @obj.msg.should_eql return_values[2]
      end

      specify "should keep returning last value in consecutive calls" do
        return_values = ["1",2,Object.new]
        @obj.stub!(:msg).and_return(return_values[0],return_values[1],return_values[2])
        @obj.msg.should_eql return_values[0]
        @obj.msg.should_eql return_values[1]
        @obj.msg.should_eql return_values[2]
        @obj.msg.should_eql return_values[2]
        @obj.msg.should_eql return_values[2]
      end

      specify "should revert to original instance method if existed" do
        @obj.existing_instance_method.should equal(:original_value)
        @obj.stub!(:existing_instance_method).and_return(:mock_value)
        @obj.existing_instance_method.should equal(:mock_value)
        @obj.__verify
        # TODO JRUBY: This causes JRuby to fail with:
        # NativeException in 'Stub should revert to original instance method if existed'
        # java.lang.ArrayIndexOutOfBoundsException: 0
        # org.jruby.internal.runtime.methods.IterateCallable.internalCall(IterateCallable.java:63)
        # org.jruby.internal.runtime.methods.AbstractCallable.call(AbstractCallable.java:64)
        # org.jruby.runtime.ThreadContext.yieldInternal(ThreadContext.java:574)
        # org.jruby.runtime.ThreadContext.yieldSpecificBlock(ThreadContext.java:549)
        # org.jruby.runtime.Block.call(Block.java:158)
        # org.jruby.RubyProc.call(RubyProc.java:118)
        # org.jruby.internal.runtime.methods.ProcMethod.internalCall(ProcMethod.java:69)
        # org.jruby.internal.runtime.methods.AbstractMethod.call(AbstractMethod.java:58)
        # org.jruby.RubyObject.callMethod(RubyObject.java:379)
        # org.jruby.RubyObject.callMethod(RubyObject.java:331)
        # org.jruby.evaluator.EvaluationState.evalInternal(EvaluationState.java:472)
        # org.jruby.evaluator.EvaluationState.evalInternal(EvaluationState.java:462)
        # org.jruby.evaluator.EvaluationState.evalInternal(EvaluationState.java:390)
        # org.jruby.evaluator.EvaluationState.eval(EvaluationState.java:133)
        @obj.existing_instance_method.should equal(:original_value)
      end
      
      specify "should revert to original class method if existed" do
        @class.existing_class_method.should equal(:original_value)
        @class.stub!(:existing_class_method).and_return(:mock_value)
        @class.existing_class_method.should equal(:mock_value)
        @class.__verify
        @class.existing_class_method.should equal(:original_value)
      end

      specify "should clear itself on __verify" do
        @obj.stub!(:this_should_go).and_return(:blah)
        @obj.this_should_go.should_equal :blah
        @obj.__verify
        lambda do
          @obj.this_should_go
        end.should_raise
      end
      
      specify "should support yielding" do
        @obj.stub!(:method_that_yields).and_yield(:yielded_value)
        current_value = :value_before
        @obj.method_that_yields {|val| current_value = val}
        current_value.should_equal :yielded_value
        @obj.__verify
      end

      specify "should throw when told to" do
        @mock.stub!(:something).and_throw(:blech)
        lambda do
          @mock.something
        end.should_throw(:blech)
      end
      
      specify "should support overriding w/ a new stub" do
        @stub.stub!(:existing_instance_method).and_return(:updated_stub_value)
        @stub.existing_instance_method.should == :updated_stub_value
      end
    end
  end
end