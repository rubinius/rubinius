require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The equal primitive sent two fixnums" do
  specify "should return true if x == y" do
    run_primitive(:equal, 500, 500).should == true
    run_primitive(:equal, 0, 0).should == true
    run_primitive(:equal, -11, -11).should == true
  end

  specify "should return false if x != y" do
    run_primitive(:equal, 3, 500).should == false
    run_primitive(:equal, -8, 3).should == false
    run_primitive(:equal, -1000, -20).should == false
  end
end

context "The equal primitive sent non-fixnums" do
  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:equal, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:equal, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:equal, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:equal, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:equal, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:equal, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:equal, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The equal primitive sent the wrong number of args" do
  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:equal, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:equal, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:equal, 1).should_raise(ArgumentError) # 0 args
  end
end

