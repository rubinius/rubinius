require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The noop primitive " do
  specify "should pop the stack" do
    run_asm('push 5\npush true\nsend_primitive noop 0').
      should == 5
  end

  specify "should complain if num_args is wrong" do
    run_primitive(:noop, true, true).should_raise(ArgumentError)
    run_primitive(:noop, true, true, true).should_raise(ArgumentError)
  end

  specify "should complain if true is not on top of stack" do
    run_primitive(:noop, false).should_raise(PrimitiveFailure)
    run_primitive(:noop, 20).should_raise(PrimitiveFailure)
  end
end

