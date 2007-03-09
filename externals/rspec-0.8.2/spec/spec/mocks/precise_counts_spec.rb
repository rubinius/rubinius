require File.dirname(__FILE__) + '/../../spec_helper.rb'

module Spec
module Mocks
context "PreciseCounts" do
    setup do
        @mock = Mock.new("test mock", {
          :auto_verify => false
        })
      
    end
    specify "should fail when exactly n times method is called less than n times" do
        @mock.should_receive(:random_call).exactly(3).times
        @mock.random_call
        @mock.random_call
        lambda do
          @mock.__verify
        end.should_raise(MockExpectationError)
      
    end
    specify "should fail when exactly n times method is never called" do
        @mock.should_receive(:random_call).exactly(3).times
        lambda do
          @mock.__verify
        end.should_raise(MockExpectationError)
      
    end
    specify "should pass if exactly n times method is called exactly n times" do
        @mock.should_receive(:random_call).exactly(3).times
        @mock.random_call
        @mock.random_call
        @mock.random_call
        @mock.__verify
      
    end
    specify "should pass multiple calls with different args and counts" do
        @mock.should_receive(:random_call).twice.with(1)
        @mock.should_receive(:random_call).once.with(2)
        @mock.random_call(1)
        @mock.random_call(2)
        @mock.random_call(1)
        @mock.__verify
      
    end
    specify "should pass mutiple calls with different args" do
        @mock.should_receive(:random_call).once.with(1)
        @mock.should_receive(:random_call).once.with(2)
        @mock.random_call(1)
        @mock.random_call(2)
        @mock.__verify
      
    end
  
end
end
end