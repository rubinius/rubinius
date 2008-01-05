require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.fail" do
  it "raises a RuntimeError" do
    lambda { fail }.should raise_error(RuntimeError)
  end

  it "accepts an Object with an exception method returning an Exception" do
    class Boring
      def self.exception(msg)
        StandardError.new msg
      end
    end
    lambda { fail Boring, "..." }.should raise_error(StandardError)
  end

  it "instantiates the specified exception class" do
    class LittleBunnyFooFoo < RuntimeError; end
    lambda { fail LittleBunnyFooFoo }.should raise_error(LittleBunnyFooFoo)
  end

  it "uses the specified message" do
    lambda { 
      begin
        fail "the duck is not irish."
      rescue => e
        e.message.should == "the duck is not irish."
        raise
      else
        raise Exception
      end
    }.should raise_error(RuntimeError)
  end
end
