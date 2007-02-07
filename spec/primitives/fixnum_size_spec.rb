require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The fixnum_size primitive sent a fixnums" do
  include PrimitiveSpecHelper

  specify "should return the implementation byte length of fixnum" do
    run_primitive(:fixnum_size, 5).should == 4
    run_primitive(:fixnum_size, 0).should == 4
    run_primitive(:fixnum_size, -11).should == 4
  end
end

context "The fixnum_size primitive sent a fixnum" do
  include PrimitiveSpecHelper

  specify "should raise PrimitiveFailure with a non-fixnum" do
    run_primitive(:fixnum_size, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_size, 7.9).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_size primitive sent the wrong number of args" do
  include PrimitiveSpecHelper

  specify "should raise ArgumentError with more than 0 args" do
    run_primitive(:fixnum_size, 1, 2).should_raise(ArgumentError) # 1 args
    run_primitive(:fixnum_size, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_size, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end
end

