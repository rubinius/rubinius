require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The fixnum_mul primitive sent two fixnums" do
  specify "should return the product of its arguments" do
    run_primitive(:fixnum_mul, 5, 5).should == 25
    run_primitive(:fixnum_mul, 0, 0).should == 0
    run_primitive(:fixnum_mul, -11, -11).should == 121
    run_primitive(:fixnum_mul, 5, 20).should == 100
    run_primitive(:fixnum_mul, 10, 1).should == 10
  end
end

context "The fixnum_mul primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_mul, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_mul, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_mul, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_mul, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_mul, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_mul, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_mul, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_mul primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_mul, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_mul, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_mul, 1).should_raise(ArgumentError) # 0 args
  end
end

