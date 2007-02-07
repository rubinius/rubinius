require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The compare primitive sent two fixnums" do
  include PrimitiveSpecHelper

  specify "should return 1 if x > y" do
    run_primitive(:compare, 30, 20).should == 1
    run_primitive(:compare, 10, -5).should == 1
    run_primitive(:compare, -10, -500).should == 1
  end

  specify "should return 0 if x == y" do
    run_primitive(:compare, 500, 500).should == 0
    run_primitive(:compare, 0, 0).should == 0
    run_primitive(:compare, -11, -11).should == 0
  end

  specify "should return -1 if x < y" do
    run_primitive(:compare, 3, 500).should == -1
    run_primitive(:compare, -8, 3).should == -1
    run_primitive(:compare, -1000, -20).should == -1
  end

end

context "The compare primitive sent non-fixnums" do
  include PrimitiveSpecHelper

  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:compare, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:compare, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:compare, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:compare, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:compare, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:compare, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:compare, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The compare primitive sent the wrong number of args" do
  include PrimitiveSpecHelper

  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:compare, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:compare, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:compare, 1).should_raise(ArgumentError) # 0 args
  end
end

