require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The fixnum_div primitive sent two fixnums" do
  include PrimitiveSpecHelper

  specify "should return the quotient of its arguments" do
    run_primitive(:fixnum_div, 5, 5).should == 1
    run_primitive(:fixnum_div, -11, -11).should == 1
    run_primitive(:fixnum_div, -20, 5).should == -4
    run_primitive(:fixnum_div, 10, 1).should == 10
  end

  specify "should discard the remainder of a division" do
    run_primitive(:fixnum_div, 10, 3).should == 3
    run_primitive(:fixnum_div, 3, 10).should == 0
  end

  specify "should raise PrimitiveFailure when division by zero occurs" do
    run_primitive(:fixnum_div, 10, 0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, -10, 0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, -10, -0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, 10, -0).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_div primitive sent non-fixnums" do
  include PrimitiveSpecHelper

  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_div, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_div, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_div, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_div, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_div primitive sent the wrong number of args" do
  include PrimitiveSpecHelper

  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_div, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_div, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_div, 1).should_raise(ArgumentError) # 0 args
  end
end

