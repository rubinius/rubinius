require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
  module Mocks
    context "a mock acting as a NullObject" do
      setup do
        @mock = Mock.new("null_object", :null_object => true)
      end

      specify "should allow explicit expectation" do
        @mock.should_receive(:something)
        @mock.something
      end

      specify "should fail verification when explicit exception not met" do
        lambda do
          @mock.should_receive(:something)
          @mock.__verify
        end.should_raise(MockExpectationError)
      end

      specify "should ignore unexpected methods" do
        @mock.random_call("a", "d", "c")
        @mock.__verify
      end

      specify "should expected message with different args first" do
        @mock.should_receive(:message).with(:expected_arg)
        @mock.message(:unexpected_arg)
        @mock.message(:expected_arg)
      end

      specify "should expected message with different args second" do
        @mock.should_receive(:message).with(:expected_arg)
        @mock.message(:expected_arg)
        @mock.message(:unexpected_arg)
      end
    end
  end
end
