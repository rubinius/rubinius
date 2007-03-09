require File.dirname(__FILE__) + '/../../spec_helper'

module Spec
  module Mocks
    context "a mock" do
      setup do
        @mock = mock("mock", :null_object => true)
      end
      specify "should answer false for received_message? when no messages received" do
        @mock.received_message?(:message).should be(false)
      end
      specify "should answer true for received_message? when message received" do
        @mock.message
        @mock.received_message?(:message).should be(true)
      end
      specify "should answer true for received_message? when message received with correct args" do
        @mock.message 1,2,3
        @mock.received_message?(:message, 1,2,3).should be(true)
      end
      specify "should answer false for received_message? when message received with incorrect args" do
        @mock.message 1,2,3
        @mock.received_message?(:message, 1,2).should be(false)
      end
    end
  end
end