require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The fixnum_modulo primitive sent two fixnums" do
  specify "should return the remainder of its arguments" do
    run_primitive(:fixnum_modulo, 5, 5).should == 0
    run_primitive(:fixnum_modulo, -11, -11).should == 0
    run_primitive(:fixnum_modulo, -20, 5).should == 0
    run_primitive(:fixnum_modulo, 10, 1).should == 0
    run_primitive(:fixnum_modulo, 10, 3).should == 1
    run_primitive(:fixnum_modulo, 3, 10).should == 3
  end

  specify "should raise PrimitiveFailure when division by zero occurs" do
    run_primitive(:fixnum_modulo, 10, 0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, -10, 0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, -10, -0).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, 10, -0).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_modulo primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_modulo, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_modulo, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_modulo, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_modulo, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_modulo primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_modulo, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_modulo, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_modulo, 1).should_raise(ArgumentError) # 0 args
  end
end

