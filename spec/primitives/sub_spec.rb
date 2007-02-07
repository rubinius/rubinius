require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The sub primitive sent two fixnums" do
  specify "should return the difference of its arguments" do
    run_primitive(:sub, 500, 500).should == 0
    run_primitive(:sub, 0, 0).should == 0
    run_primitive(:sub, -11, -11).should == 0
    run_primitive(:sub, 5, 20).should == -15
    run_primitive(:sub, 10, 1).should == 9
  end
end

context "The sub primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:sub, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:sub, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:sub, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:sub, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:sub, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:sub, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:sub, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The sub primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:sub, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:sub, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:sub, 1).should_raise(ArgumentError) # 0 args
  end
end

