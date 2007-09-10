require File.dirname(__FILE__) + '/../../spec_helper'

context "Kernel.fail()" do
  specify "should raise an exception" do
    should_raise(RuntimeError) { fail }
  end

  specify "should accept an Object with an exception method returning an Exception" do
    class Boring
      def self.exception(msg)
        StandardError.new msg
      end
    end
    should_raise(StandardError) { fail Boring, "..." }
  end

  specify "should instantiate specified exception class" do
    class LittleBunnyFooFoo < RuntimeError; end
    should_raise(LittleBunnyFooFoo) { fail LittleBunnyFooFoo }
  end

  specify "should use specified message" do
    should_raise(RuntimeError) { 
      begin
        fail "the duck is not irish."
      rescue => e
        e.message.should == "the duck is not irish."
        raise
      else
        raise Exception
      end
    }
  end
end
