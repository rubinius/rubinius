require File.dirname(__FILE__) + '/../primitives_spec_helper'

context "The fixnum_to_s primitive sent self and a base (as a fixnum)" do
  include PrimitiveSpecHelper

  specify "should return the representation of the number in decimal" do
    run_primitive(:fixnum_to_s, 5, 10).should == '5'
    run_primitive(:fixnum_to_s, 31, 10).should == '31'
  end

  specify "should return the representation of the number in hex" do
    run_primitive(:fixnum_to_s, 5, 16).should == '5'
    run_primitive(:fixnum_to_s, 31, 16).should == '1f'
  end

  specify "should return the representation of the number in octal" do
    run_primitive(:fixnum_to_s, 5, 8).should == '5'
    run_primitive(:fixnum_to_s, 31, 8).should == '37'
  end

  specify "should return the representation of the number in binary" do
    run_primitive(:fixnum_to_s, 5, 2).should == '101'
    run_primitive(:fixnum_to_s, 31, 2).should == '11111'
  end
end

context "The fixnum_to_s primitive sent non-fixnums" do
  include PrimitiveSpecHelper

  specify "should raise PrimitiveFailure with fixnum, non-fixnum" do
    run_primitive(:fixnum_to_s, 3, "foo").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_to_s, 3, 7.9).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, fixnum" do
    run_primitive(:fixnum_to_s, "foo", 80).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_to_s, 100.0, 80).should_raise(PrimitiveFailure)
  end

  specify "should raise PrimitiveFailure with non-fixnum, non-fixnum" do
    run_primitive(:fixnum_to_s, "foo", "bar").should_raise(PrimitiveFailure)
    run_primitive(:fixnum_to_s, "foo", 38.8).should_raise(PrimitiveFailure)
    run_primitive(:fixnum_to_s, 8.8, 38.8).should_raise(PrimitiveFailure)
  end
end

context "The fixnum_to_s primitive sent the wrong number of args" do
  include PrimitiveSpecHelper

  specify "should raise ArgumentError with more than 1 args" do
    run_primitive(:fixnum_to_s, 1, 2, 3).should_raise(ArgumentError) # 2 args
    run_primitive(:fixnum_to_s, 1, 2, 3, 4).should_raise(ArgumentError) # 3 args
  end

  specify "should raise ArgumentError with less than 1 args" do
    run_primitive(:fixnum_to_s, 1).should_raise(ArgumentError) # 0 args
  end
end

