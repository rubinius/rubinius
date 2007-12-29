require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Kernel.fail" do
  it "should raise an exception" do
    lambda { fail }.should raise_error(RuntimeError)
  end

  it "should accept an Object with an exception method returning an Exception" do
    class Boring
      def self.exception(msg)
        StandardError.new msg
      end
    end
    lambda { fail Boring, "..." }.should raise_error(StandardError)
  end

  it "should instantiate specified exception class" do
    class LittleBunnyFooFoo < RuntimeError; end
    lambda { fail LittleBunnyFooFoo }.should raise_error(LittleBunnyFooFoo)
  end

  it "should use specified message" do
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
